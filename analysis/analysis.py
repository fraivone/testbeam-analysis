import os, sys

from tqdm import tqdm
import numpy as np
import pandas as pd
import uproot

import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def main():
    ifile, odir = sys.argv[1], sys.argv[2]
    os.makedirs(odir, exist_ok=True)

    if "-v" in sys.argv or "--verbose" in sys.argv:
        verbose = True
    else: verbose = False

    matching_cut = 2
    with uproot.open(ifile) as track_file:
        track_tree = track_file["trackTree"]
        if verbose: track_tree.show()

        prophits_x, prophits_y = track_tree["prophitX"].array(library="np"), track_tree["prophitY"].array(library="np")
        rechits_x, rechits_y = track_tree["rechitX"].array(library="np"), track_tree["rechitY"].array(library="np")

        print("Matching...")
        # matchesX = abs(prophits_x-rechits_x) < matching_cut
        # matchesY = abs(prophits_y-rechits_y) < matching_cut
        # matches = matchesX & matchesY
        matches = (prophits_x-rechits_x)**2+(prophits_y-rechits_y)**2 < matching_cut**2
        matched_x, matched_y = prophits_x[matches], prophits_y[matches]

        print("Saving efficiency map...")
        eff_fig = plt.figure(figsize=(10,9))

        eff_range = [[min(rechits_x), max(rechits_x)], [min(rechits_y), max(rechits_y)]]
        matched_histogram, matched_bins_x, matched_bins_y, _ = plt.hist2d(matched_x, matched_y, bins=50, range=eff_range)
        total_histogram, total_bins_x, total_bins_y, _ = plt.hist2d(prophits_x, prophits_y, bins=50, range=eff_range)

        if not (np.array_equal(matched_bins_x,total_bins_x) and np.array_equal(matched_bins_y,total_bins_y)):
            raise ValueError("Different bins between numerator and denominator")
        efficiency = np.divide(matched_histogram, total_histogram, where=(total_histogram!=0))
        
        bins_x = (matched_bins_x + 0.5*(matched_bins_x[1]-matched_bins_x[0]))[:-1]
        bins_y = (matched_bins_y + 0.5*(matched_bins_y[1]-matched_bins_y[0]))[:-1]
        #plt.contourf(bins_x, bins_y, efficiency)
        plt.imshow(efficiency, extent=eff_range[0]+eff_range[1], origin="lower")
        plt.xlabel("x (mm)")
        plt.ylabel("y (mm)")
        plt.colorbar(label="Efficiency")
        plt.tight_layout()
        plt.text(eff_range[0][-1]-.5, eff_range[1][-1]+2, "GEM-10x10-380XY-BARI-04", horizontalalignment="right")
        eff_fig.savefig(f"{odir}/efficiency.png")

if __name__=='__main__': main()
