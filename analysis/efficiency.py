import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import scipy
from scipy.optimize import curve_fit

import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import mplhep as hep
plt.style.use(hep.style.ROOT)

# enable multi-threading:
from concurrent.futures import ThreadPoolExecutor
executor = ThreadPoolExecutor()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input file")
    parser.add_argument("odir", type=pathlib.Path, help="Output directory")
    parser.add_argument("detector", type=str, help="Detector under test")
    parser.add_argument("bins", type=int, help="Number of bins")
    parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    with uproot.open(args.ifile) as track_file:
        track_tree = track_file["trackTree"]
        if args.verbose: track_tree.show()

        print("Reading tree...")

        if args.detector=="ge21":
            rechit_chamber = track_tree["rechitChamber"].array(entry_stop=args.events)
            prophit_chamber = track_tree["prophitChamber"].array(entry_stop=args.events)
            rechits_eta = track_tree["rechitEta"].array(entry_stop=args.events)
            prophits_eta = ak.flatten(track_tree["prophitEta"].array(entry_stop=args.events))
            rechits_x = track_tree["rechitLocalY"].array(entry_stop=args.events)
            rechits_y = track_tree["rechitLocalX"].array(entry_stop=args.events)
            prophits_x = ak.flatten(track_tree["prophitLocalY"].array(entry_stop=args.events))
            prophits_y = ak.flatten(track_tree["prophitLocalX"].array(entry_stop=args.events))
            residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

            print("Matching...")

            # bin_size = 1.
            # bins_x, bins_y = np.mgrid[-40:40:bin_size, -40:40:bin_size]
            # print(bins_x)
            # print(bins_y)

            # occupancies = 
            # for bin_x, bin_y in zip(bins_x, bins_y):

            print(prophits_x)
            print(rechits_x)

            mask_out = (abs(prophits_x)<40.)&(abs(prophits_y)<40.)
            rechits_x, rechits_y = rechits_x[mask_out], rechits_y[mask_out]
            prophits_x, prophits_y = prophits_x[mask_out], prophits_y[mask_out]

            matches = ak.count(rechits_x, axis=1)>0
            print(matches)
            matched_x, matched_y = prophits_x[matches], prophits_y[matches]

            # matchesX = abs(prophits_x-rechits_x) < matching_cut
            # matchesY = abs(prophits_y-rechits_y) < matching_cut
            # matches = matchesX & matchesY
            # matches = (prophits_x-rechits_x)**2+(prophits_y-rechits_y)**2 < matching_cut**2
            # matched_x, matched_y = prophits_x[matches], prophits_y[matches]

            print("Calculating efficiency map...")
            eff_fig, eff_ax = plt.figure(figsize=(10,9)), plt.axes()
            eff_range = [[min(prophits_x), max(prophits_x)], [min(prophits_y), max(prophits_y)]]
            matched_histogram, matched_bins_x, matched_bins_y = np.histogram2d(matched_x, matched_y, bins=args.bins, range=eff_range)
            total_histogram, total_bins_x, total_bins_y = np.histogram2d(prophits_x, prophits_y, bins=args.bins, range=eff_range)

            print(matched_histogram)
            print(total_histogram)
            print(ak.count(matched_bins_x), matched_bins_x)
            print(ak.count(matched_bins_y), matched_bins_y)

            if not (np.array_equal(matched_bins_x,total_bins_x) and np.array_equal(matched_bins_y,total_bins_y)):
                raise ValueError("Different bins between numerator and denominator")
            efficiency = np.divide(matched_histogram, total_histogram, where=(total_histogram!=0))

            print(ak.count(efficiency), efficiency)
            
            print("Plotting efficiency map...")
            # centers_x = 0.5*(matched_bins_x[1:]+matched_bins_x[:-1])
            # matched_bins_x = matched_bins_x[matched_bins_x < -10]
            # efficiency = efficiency[centers_x < -10]
            # bins_x = (matched_bins_x + 0.5*(matched_bins_x[1]-matched_bins_x[0]))[:-1]
            # bins_y = (matched_bins_y + 0.5*(matched_bins_y[1]-matched_bins_y[0]))[:-1]
            img = eff_ax.imshow(
                efficiency,
                extent=[matched_bins_x[0], matched_bins_x[-1], matched_bins_y[0], matched_bins_y[-1]],
                origin="lower"
            )
            eff_ax.set_xlabel("x (mm)")
            eff_ax.set_ylabel("y (mm)")
            eff_ax.set_title(
                r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
                color='black', weight='normal', loc="left"
            )
            eff_fig.colorbar(img, ax=eff_ax, label="Efficiency")
            #img.set_clim(.85, 1.)
            eff_fig.tight_layout()
            eff_ax.text(eff_range[0][-1]-.5, eff_range[1][-1]+2, "GE2/1", horizontalalignment="right")
            print("Saving result...")
            eff_fig.savefig(os.path.join(args.odir, "ge21.png"))

            """ Calculate angular alignment based on HV sectors """
            # choose only points close to 1 sector:
            centers_x = 0.5*(matched_bins_x[1:]+matched_bins_x[:-1])
            centers_y = 0.5*(matched_bins_y[1:]+matched_bins_y[:-1])
            map_mask = centers_x < -10
            centers_x = centers_x[map_mask]
            efficiency = efficiency[map_mask]

            print(centers_x.shape)
            print(efficiency.shape)

            npoints_y = 10
            step_y = int(ak.count(centers_y)/npoints_y)
            slices_y = centers_y[::step_y]
            print(slices_y.shape)
            print("efficiency", efficiency)
            print("transposed", efficiency.T)
            print("efficiency shape", efficiency.shape)
            efficiency_slices = efficiency.T[::step_y]
            print(efficiency_slices.shape)
            print(efficiency_slices)

            #slices_fig, slices_axs = plt.subplots(nrows=npoints_y, ncols=1, figsize=(12, 10*npoints_y))
            slices_fig, slices_ax = plt.figure(), plt.subplot(projection="3d")
            for slice_y, eff_slice in zip(slices_y, efficiency_slices):
                #print(centers_x.shape)
                #print(eff_slice.shape)
                slices_ax.plot(centers_x, slice_y*np.ones(ak.count(centers_x)), eff_slice)
                eff_min = ak.min(eff_slice)
                x_min = centers_x[eff_slice==eff_min] # where efficiency minimum is
                #print(slice_y, x_min)
            slices_ax.set_xlabel("x (mm)")
            slices_ax.set_ylabel("y (mm)")
            slices_ax.set_zlabel("Efficiency")
            slices_fig.savefig(os.path.join(args.odir, "ge21_slices.png"))

        if args.detector=="me0":
            rechit_chamber = track_tree["rechitChamber"].array(entry_stop=args.events)
            prophit_chamber = track_tree["prophitChamber"].array(entry_stop=args.events)
            rechits_eta = track_tree["rechitEta"].array(entry_stop=args.events)
            prophits_eta = ak.flatten(track_tree["prophitEta"].array(entry_stop=args.events))
            rechits_x = track_tree["rechitLocalY"].array(entry_stop=args.events)
            rechits_y = track_tree["rechitLocalX"].array(entry_stop=args.events)
            prophits_x = track_tree["prophitGlobalY"].array(entry_stop=args.events)
            prophits_y = track_tree["prophitGlobalX"].array(entry_stop=args.events)

            print(rechit_chamber)
            print(rechits_x)
            me0_chamber = 5
            prophits_x, prophits_y = ak.flatten(prophits_x[prophit_chamber==me0_chamber]), ak.flatten(prophits_y[prophit_chamber==me0_chamber])
            rechits_x, rechits_y = rechits_x[rechit_chamber==me0_chamber], rechits_y[rechit_chamber==me0_chamber]
            print(rechits_x)

            print("Matching...")
            mask_out = (abs(prophits_x)<40.)&(abs(prophits_y)<40.)
            rechits_x, rechits_y = rechits_x[mask_out], rechits_y[mask_out]
            prophits_x, prophits_y = prophits_x[mask_out], prophits_y[mask_out]
            matches = ak.count(rechits_x, axis=1)>0
            matched_x, matched_y = prophits_x[matches], prophits_y[matches]

            print("Calculating efficiency map...")
            eff_fig, eff_ax = plt.figure(figsize=(10,9)), plt.axes()
            eff_range = [[min(prophits_x), max(prophits_x)], [min(prophits_y), max(prophits_y)]]
            matched_histogram, matched_bins_x, matched_bins_y = np.histogram2d(matched_x, matched_y, bins=args.bins, range=eff_range)
            total_histogram, total_bins_x, total_bins_y = np.histogram2d(prophits_x, prophits_y, bins=args.bins, range=eff_range)

            print(matched_histogram)
            print(total_histogram)
            print(ak.count(matched_bins_x), matched_bins_x)
            print(ak.count(matched_bins_y), matched_bins_y)

            if not (np.array_equal(matched_bins_x,total_bins_x) and np.array_equal(matched_bins_y,total_bins_y)):
                raise ValueError("Different bins between numerator and denominator")
            efficiency = np.divide(matched_histogram, total_histogram, where=(total_histogram!=0))

            print(ak.count(efficiency), efficiency)
            
            print("Plotting efficiency map...")
            img = eff_ax.imshow(
                efficiency,
                extent=[matched_bins_x[0], matched_bins_x[-1], matched_bins_y[0], matched_bins_y[-1]],
                origin="lower"
            )
            eff_ax.set_xlabel("x (mm)")
            eff_ax.set_ylabel("y (mm)")
            eff_ax.set_title(
                r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
                color='black', weight='normal', loc="left"
            )
            # cax = eff_fig.add_axes([
            #     eff_ax.get_position().x1+0.01,
            #     eff_ax.get_position().y0,
            #     0.02,eff_ax.get_position().height
            # ])
            eff_fig.colorbar(img, ax=eff_ax, label="Efficiency")
            eff_fig.tight_layout()
            eff_ax.text(eff_range[0][-1]-.5, eff_range[1][-1]+2, "ME0", horizontalalignment="right")
            print("Saving result...")
            eff_fig.savefig(os.path.join(args.odir, "me0.png"))

        elif args.detector=="tracker":
            rechits_chamber = track_tree["rechits2D_Chamber"].array(entry_stop=args.events)
            rechits_x = track_tree["rechits2D_X"].array(entry_stop=args.events)
            rechits_y = track_tree["rechits2D_Y"].array(entry_stop=args.events)
            prophits_x = track_tree["prophits2D_X"].array(entry_stop=args.events)
            prophits_y = track_tree["prophits2D_Y"].array(entry_stop=args.events)

            print("chambers", rechits_chamber)
            print("prophits", prophits_x)
            print("rechits", rechits_x)

            eff_fig, eff_axs = plt.subplots(nrows=1, ncols=4, figsize=(48,9))
            for tested_chamber in range(4):
                prophits_x_chamber = prophits_x[:,tested_chamber]
                prophits_y_chamber = prophits_y[:,tested_chamber]

                # stay in chamber boundary:
                mask_out = (abs(prophits_x_chamber)<40.)&(abs(prophits_y_chamber)<40.)
                rechits_chamber_inside = rechits_chamber[mask_out]
                prophits_x_chamber, prophits_y_chamber = prophits_x_chamber[mask_out], prophits_y_chamber[mask_out]
                rechits_x_chamber, rechits_y_chamber = rechits_x[mask_out], rechits_y[mask_out]

                # choose only events with at least 3 hits:
                mask_3hit = ak.count_nonzero(rechits_chamber_inside>=0, axis=1)>2
                rechits_chamber_inside = rechits_chamber_inside[mask_3hit]
                rechits_x_chamber, rechits_y_chamber = rechits_x_chamber[mask_3hit], rechits_y_chamber[mask_3hit]
                prophits_x_chamber, prophits_y_chamber = prophits_x_chamber[mask_3hit], prophits_y_chamber[mask_3hit]

                # list only events with a rechit in tested chamber, otherwise None:
                rechit_mask = ak.count_nonzero(rechits_chamber_inside==tested_chamber, axis=1)>0
                rechits_x_chamber, rechits_y_chamber = rechits_x_chamber.mask[rechit_mask], rechits_y_chamber.mask[rechit_mask]

                # for good events, list only rechit in tested chamber
                chamber_mask = rechits_chamber_inside==tested_chamber
                # to refine matching with angle later:
                # rechits_x_chamber = ak.sum(rechits_x_chamber.mask[chamber_mask], axis=1)
                # rechits_y_chamber = ak.sum(rechits_y_chamber.mask[chamber_mask], axis=1)
                chamber_mask = chamber_mask[chamber_mask]
                matched_x_chamber = prophits_x_chamber[chamber_mask]
                matched_y_chamber = prophits_y_chamber[chamber_mask]

                print("Calculating efficiency map...")
                eff_range = [[min(prophits_x_chamber), max(prophits_x_chamber)], [min(prophits_y_chamber), max(prophits_y_chamber)]]
                matched_histogram, matched_bins_x, matched_bins_y = np.histogram2d(matched_x_chamber, matched_y_chamber, bins=args.bins, range=eff_range)
                total_histogram, total_bins_x, total_bins_y = np.histogram2d(prophits_x_chamber, prophits_y_chamber, bins=args.bins, range=eff_range)

                if not (np.array_equal(matched_bins_x,total_bins_x) and np.array_equal(matched_bins_y,total_bins_y)):
                    raise ValueError("Different bins between numerator and denominator")
                efficiency = np.divide(matched_histogram, total_histogram, where=(total_histogram!=0))

                print(efficiency)
                
                print("Plotting efficiency map...")
                # bins_x = (matched_bins_x + 0.5*(matched_bins_x[1]-matched_bins_x[0]))[:-1]
                # bins_y = (matched_bins_y + 0.5*(matched_bins_y[1]-matched_bins_y[0]))[:-1]
                #plt.contourf(bins_x, bins_y, efficiency)
                img = eff_axs[tested_chamber].imshow(
                    efficiency,
                    #extent = [0, 1, 0, 1],
                    #extent=eff_range[0]+eff_range[1],
                    extent=[matched_bins_x[0], matched_bins_x[-1], matched_bins_y[0], matched_bins_y[-1], ],
                    origin="lower"
                )
                eff_axs[tested_chamber].set_xlabel("x (mm)")
                eff_axs[tested_chamber].set_ylabel("y (mm)")
                eff_axs[tested_chamber].set_title(
                    r"$\bf{CMS}\,\,\it{Muon\,\,R&D}$",
                    color='black', weight='normal', loc="left"
                )
                eff_fig.colorbar(img, ax=eff_axs[tested_chamber], label="Efficiency")
                img.set_clim(.85, 1.)
                eff_axs[tested_chamber].text(eff_range[0][-1]-.5, eff_range[1][-1]+2, f"BARI-0{tested_chamber+1}", horizontalalignment="right")

            print("Saving result...")
            eff_fig.tight_layout()
            eff_fig.savefig(os.path.join(args.odir, "tracker.png"))

if __name__=="__main__": main()
