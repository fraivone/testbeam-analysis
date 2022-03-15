import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import pandas as pd

import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("idir", type=pathlib.Path, help="Input directory")
    parser.add_argument("odir", type=pathlib.Path, help="Output directory")
    parser.add_argument("runs", type=pathlib.Path, help="csv file with run list")
    parser.add_argument("detector", type=str, help="Detector under test")
    parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    run_df = pd.read_csv(args.runs, sep=";", comment="#")

    if args.detector=="ge21":
        efficiencies = list()
        err_efficiencies = list()
        cluster_sizes = [list(), list(), list(), list()]
        err_cluster_sizes = [list(), list(), list(), list()]
    if args.detector=="tracker":
        efficiencies = [list(), list(), list(), list()]
        err_efficiencies = [list(), list(), list(), list()]
        cluster_sizes_x = [list(), list(), list(), list()]
        cluster_sizes_y = [list(), list(), list(), list()]
        err_cluster_sizes_x = [list(), list(), list(), list()]
        err_cluster_sizes_y = [list(), list(), list(), list()]
    
    run_bias, run_names = list(run_df["hv"].astype(int)), list(run_df["name"])
    for hv,run_name in zip(run_bias, run_names):
        print("Analysing", run_name, "at", hv, "µA")

        ifile = os.path.join(args.idir, f"{run_name}.root")

        with uproot.open(ifile) as track_file:
            track_tree = track_file["trackTree"]
            if args.verbose: track_tree.show()

            print("    Reading tree...")
            if args.detector=="ge21":
                rechits_x = track_tree["rechitLocalY"].array(entry_stop=args.events)
                rechits_y = track_tree["rechitLocalX"].array(entry_stop=args.events)
                prophits_x = ak.flatten(track_tree["prophitLocalY"].array(entry_stop=args.events))
                prophits_y = ak.flatten(track_tree["prophitLocalX"].array(entry_stop=args.events))
                cluster_size = track_tree["rechitClusterSize"].array(entry_stop=args.events)

                n_events = ak.count(prophits_x, axis=0)
                print(f"    Tree contains {n_events} events.")

                print("    Matching...")
                mask_out = (abs(prophits_x)<40.)&(abs(prophits_y)<40.)
                rechits_x, rechits_y = rechits_x[mask_out], rechits_y[mask_out]
                prophits_x, prophits_y = prophits_x[mask_out], prophits_y[mask_out]
                cluster_size = cluster_size[mask_out]

                matches = ak.count(rechits_x, axis=1)>0
                #matched_x, matched_y = prophits_x[matches], prophits_y[matches]
                good_events = ak.count(matches)
                cluster_size = cluster_size[matches]

                print("   ", good_events, "matches over", n_events, "events")
                efficiency = good_events/n_events
                err_efficiency = efficiency*np.sqrt(1/good_events + 1/n_events)
                efficiencies.append(efficiency)
                err_efficiencies.append(err_efficiency)
                print("   ", hv, "µA", efficiency, "+/-", err_efficiency)
                print("")

                print("    Calculating cluster size...")
                cluster_sizes[tested_chamber].append(ak.mean(cluster_size))
                err_cluster_sizes[tested_chamber].append(
                    ak.std(cluster_size)/np.sqrt(ak.size(cluster_size))
                )
                
            elif args.detector=="tracker":
                rechits_chamber = track_tree["rechits2D_Chamber"].array(entry_stop=args.events)
                rechits_x = track_tree["rechits2D_X"].array(entry_stop=args.events)
                rechits_y = track_tree["rechits2D_Y"].array(entry_stop=args.events)
                cluster_size_x = track_tree["rechits2D_X_ClusterSize"].array(entry_stop=args.events)
                cluster_size_y = track_tree["rechits2D_Y_ClusterSize"].array(entry_stop=args.events)
                prophits_x = track_tree["prophits2D_X"].array(entry_stop=args.events)
                prophits_y = track_tree["prophits2D_Y"].array(entry_stop=args.events)

                for tested_chamber in range(4):
                    prophits_x_chamber = prophits_x[:,tested_chamber]
                    prophits_y_chamber = prophits_y[:,tested_chamber]

                    # stay in chamber boundary:
                    mask_out = (abs(prophits_x_chamber)<40.)&(abs(prophits_y_chamber)<40.)
                    rechits_chamber_inside = rechits_chamber[mask_out]
                    prophits_x_chamber, prophits_y_chamber = prophits_x_chamber[mask_out], prophits_y_chamber[mask_out]
                    rechits_x_chamber, rechits_y_chamber = rechits_x[mask_out], rechits_y[mask_out]
                    cluster_size_x_chamber, cluster_size_y_chamber = cluster_size_x[mask_out], cluster_size_y[mask_out]

                    # choose only events with at least 3 hits:
                    mask_3hit = ak.count_nonzero(rechits_chamber_inside>=0, axis=1)>2
                    rechits_chamber_inside = rechits_chamber_inside[mask_3hit]
                    rechits_x_chamber, rechits_y_chamber = rechits_x_chamber[mask_3hit], rechits_y_chamber[mask_3hit]
                    cluster_size_x_chamber, cluster_size_y_chamber = cluster_size_x_chamber[mask_3hit], cluster_size_y_chamber[mask_3hit]
                    prophits_x_chamber, prophits_y_chamber = prophits_x_chamber[mask_3hit], prophits_y_chamber[mask_3hit]

                    # list only events with a rechit in tested chamber, otherwise None:
                    rechit_matches = ak.count_nonzero(rechits_chamber_inside==tested_chamber, axis=1)>0
                    rechits_x_chamber, rechits_y_chamber = rechits_x_chamber[rechit_matches], rechits_y_chamber[rechit_matches]
                    cluster_size_x_chamber, cluster_size_y_chamber = cluster_size_x_chamber[rechit_matches], cluster_size_y_chamber[rechit_matches]

                    print("   ", rechit_matches)
                    print("   ", rechits_x_chamber)
                    print("    Calculating efficiency...")
                    n_matches = ak.num(rechits_x_chamber, axis=0)
                    n_events = ak.num(prophits_x_chamber, axis=0)
                    print("   ", n_matches, "matches over", n_events, "events")
                    efficiency = n_matches/n_events
                    err_efficiency = efficiency*np.sqrt(1/n_matches + 1/n_events)
                    efficiencies[tested_chamber].append(efficiency)
                    err_efficiencies[tested_chamber].append(err_efficiency)
                    print("   ", hv, efficiency, "+/-", err_efficiency)

                    print("    Calculating cluster size...")
                    cluster_sizes_x[tested_chamber].append(ak.mean(cluster_size_x_chamber))
                    cluster_sizes_y[tested_chamber].append(ak.mean(cluster_size_y_chamber))
                    err_cluster_sizes_x[tested_chamber].append(
                        ak.std(cluster_size_x_chamber)/np.sqrt(ak.size(cluster_size_x_chamber))
                    )
                    err_cluster_sizes_y[tested_chamber].append(
                        ak.std(cluster_size_y_chamber)/np.sqrt(ak.size(cluster_size_y_chamber))
                    )


    print("Plotting efficiency and cluster size...")
    if args.detector=="ge21":
        efficiency_fig, efficiency_ax = plt.figure(figsize=(12, 10)), plt.axes()
        efficiency_ax.errorbar(
            run_bias, efficiencies, yerr=err_efficiencies, fmt="o", color="black", marker="o", markersize=12
        )
        efficiency_ax.set_xlabel("Equivalent divider current (µA)")
        efficiency_ax.set_ylabel("Efficiency")
        efficiency_ax.set_ylim(.7, 1.)
        efficiency_ax.set_title(
            r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
            color='black', weight='normal', loc="left"
        )
        fig.savefig(os.path.join(args.odir, "efficiency.png"))

        cluster_size_fig, cluster_size_ax = plt.figure(figsize=(12, 10)), plt.axes()
        cluster_size_ax.errorbar(
                run_bias, cluster_sizes[tested_chamber], yerr=err_cluster_sizes[tested_chamber],
                marker="o", markersize=12,
            )
        cluster_size_ax.set_xlabel("Equivalent divider current (µA)")
        cluster_size_ax.set_ylabel("Cluster size")
        cluster_size_ax.set_title(
            r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
            color='black', weight='normal', loc="left"
        )
        cluster_size_ax.legend()
        cluster_size_fig.savefig(os.path.join(args.odir, "cluster_size.png"))

    elif args.detector=="tracker":
        efficiency_fig, efficiency_ax = plt.figure(figsize=(12, 10)), plt.axes()
        cluster_size_fig, cluster_size_ax = plt.subplots(1, 2, figsize=(28, 12))
        for tested_chamber in range(4):
            efficiency_ax.errorbar(
                run_bias, efficiencies[tested_chamber], yerr=err_efficiencies[tested_chamber],
                marker="o", markersize=12,
                label=f"BARI-0{tested_chamber+1}"
            )
            cluster_size_ax[0].errorbar(
                run_bias, cluster_sizes_x[tested_chamber], yerr=err_cluster_sizes_x[tested_chamber],
                marker="o", markersize=12,
                label=f"BARI-0{tested_chamber+1}"
            )
            cluster_size_ax[1].errorbar(
                run_bias, cluster_sizes_y[tested_chamber], yerr=err_cluster_sizes_y[tested_chamber],
                marker="o", markersize=12,
                label=f"BARI-0{tested_chamber+1}"
            )
        efficiency_ax.set_xlabel("Equivalent divider current (µA)")
        efficiency_ax.set_ylabel("Efficiency")
        efficiency_ax.set_ylim(.7, 1.)
        efficiency_ax.set_title(
            r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
            color='black', weight='normal', loc="left"
        )
        efficiency_ax.legend()
        efficiency_fig.savefig(os.path.join(args.odir, "tracker.png"))

        for idir,direction in enumerate(["x", "y"]):
            cluster_size_ax[idir].set_xlabel("Equivalent divider current (µA)")
            cluster_size_ax[idir].set_ylabel(f"Cluster size {direction}")
            cluster_size_ax[idir].set_title(
                r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
                color='black', weight='normal', loc="left"
            )
            cluster_size_ax[idir].legend()
        cluster_size_fig.savefig(os.path.join(args.odir, "cluster_size_tracker.png"))

if __name__=="__main__": main()
