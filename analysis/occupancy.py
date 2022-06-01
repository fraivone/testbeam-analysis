import os, sys, pathlib
import argparse

import uproot
import numpy as np
import pandas as pd
import awkward as ak
import scipy
from scipy.optimize import curve_fit

import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input file")
    parser.add_argument('odir', type=pathlib.Path, help="Output directory")
    parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    with uproot.open(args.ifile) as digi_file:
        digi_tree = digi_file["outputtree"]
        if args.verbose: digi_tree.show()

        digi_oh = digi_tree["OH"].array(entry_stop=args.events)
        digi_vfat = digi_tree["VFAT"].array(entry_stop=args.events)
        digi_chamber = digi_tree["digiChamber"].array(entry_stop=args.events)
        digi_eta = digi_tree["digiEta"].array(entry_stop=args.events)
        digi_strip = digi_tree["digiStrip"].array(entry_stop=args.events)

        for chamber in np.unique(ak.flatten(digi_chamber)):
            for eta in np.unique(ak.flatten(digi_eta)):

                chamber_filter = digi_chamber==chamber
                eta_filter = digi_eta==eta
                filtered_strips = ak.flatten(digi_strip[(chamber_filter)&(eta_filter)])
                filtered_oh = ak.flatten(digi_oh[(chamber_filter)&(eta_filter)])
                filtered_vfat = ak.flatten(digi_vfat[(chamber_filter)&(eta_filter)])

                if args.verbose: print(f"chamber {chamber}, eta {eta}, strips:", filtered_strips)
                
                if ak.count(filtered_strips) == 0: continue # no vfats for selected chamber, eta
               
                oh = np.unique(filtered_oh)[0]
                vfats = np.unique(filtered_vfat)

                occupancy_fig, occupancy_ax = plt.figure(figsize=(12,10)), plt.axes()
                for vfat in vfats:
                    filtered_strips_vfat = filtered_strips[filtered_vfat==vfat]
                    if args.verbose: print(f"  chamber {chamber}, eta {eta}, VFAT {vfat}, strips:", filtered_strips_vfat)
                    occupancy_ax.hist(filtered_strips_vfat, bins=358, range=(0,358), label="OH {}, VFAT {}".format(oh, vfat), alpha=0.5)
                occupancy_ax.legend()
                occupancy_ax.set_title("chamber {}, eta {}".format(chamber, eta)) 
                occupancy_ax.set_xlabel("Strip")
                occupancy_fig.savefig(args.odir / f"occupancy_chamber{chamber}_eta{eta}.png")

if __name__=='__main__': main()
