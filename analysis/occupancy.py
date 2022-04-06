import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import scipy
from scipy.optimize import curve_fit

import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import make_axes_locatable
import mplhep as hep
plt.style.use(hep.style.ROOT)

n_chambers = 5
n_eta = 5

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

        print("Reading tree...")
        digi_eta = ak.flatten(digi_tree["digiEta"].array(entry_stop=args.events))
        digi_oh = ak.flatten(digi_tree["OH"].array(entry_stop=args.events))
        digi_chamber = ak.flatten(digi_tree["digiChamber"].array(entry_stop=args.events))
        digi_direction = ak.flatten(digi_tree["digiDirection"].array(entry_stop=args.events))
        digi_strip = ak.flatten(digi_tree["digiStrip"].array(entry_stop=args.events))

        print("digi_eta", digi_eta)
        print("digi_oh", digi_oh)
        print("digi_chamber", digi_chamber)
        print("digi_direction", digi_direction)
        print("digi_strip", digi_strip)

        for chamber in range(n_chambers):
            digi_strip_masked = digi_strip[digi_chamber==chamber]
            digi_eta_masked = digi_eta[digi_chamber==chamber]
            
            print("chamber", chamber)
            print("Strip", digi_strip_masked)
            print("Eta", digi_eta_masked)

            occupancy_fig, occupancy_ax = plt.subplots(figsize=(12, 9))
            for eta in range(n_eta):
                h, xedges, _ = occupancy_ax.hist(
                    digi_strip_masked[digi_eta_masked==eta],
                    range=(0,380),
                    bins=380,
                    histtype="step",
                    label=f"$\eta = {eta}$"
                )
                #print("Chamber", chamber, "eta", eta, "edges", xedges)
                occupancy_ax.set_xlabel("Strip")
                occupancy_ax.legend()
            occupancy_fig.savefig(args.odir / f"occupancy_chamber{chamber}.png")

            occupancy2d_fig, occupancy2d_ax = plt.subplots(figsize=(12, 9))
            h, x, y, img = occupancy2d_ax.hist2d(
                digi_strip_masked,
                digi_eta_masked,
                range=((0,380), (0, 5)),
                bins=100
            )
            occupancy2d_ax.set_xlabel("Strip")
            occupancy2d_ax.set_ylabel("Eta")
            occupancy2d_fig.colorbar(img, ax=occupancy2d_ax)
            occupancy2d_fig.savefig(args.odir / f"occupancy2d_chamber{chamber}.png")

if __name__=='__main__': main()
