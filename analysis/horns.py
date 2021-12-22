import os, sys

import uproot
import numpy as np
import scipy
from scipy.optimize import curve_fit

import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def main():
    ifile, odir = sys.argv[1], sys.argv[2]
    os.makedirs(odir, exist_ok=True)

    if "-v" in sys.argv or "--verbose" in sys.argv: verbose = True
    else: verbose = False

    with uproot.open(ifile) as track_file:
        track_tree = track_file["trackTree"]
        if verbose:
            print("Events: ", len(track_tree["trackFitChi2"].array(library="np", entry_stop=1000000)))
            track_tree.show()

        print("Reading tree...")

        rechits_x = np.vstack(track_tree["rechits2D_X"].array(library="np", entry_stop=1000000)).T
        rechits_y = np.vstack(track_tree["rechits2D_Y"].array(library="np", entry_stop=1000000)).T
        prophits_x = np.vstack(track_tree["prophits2D_X"].array(library="np", entry_stop=1000000)).T
        prophits_y = np.vstack(track_tree["prophits2D_Y"].array(library="np", entry_stop=1000000)).T
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y
        cluster_size_x = np.vstack(track_tree["rechits2D_X_ClusterSize"].array(library="np", entry_stop=1000000)).T
        cluster_size_y = np.vstack(track_tree["rechits2D_Y_ClusterSize"].array(library="np", entry_stop=1000000)).T

        # print("Applying angular alignment...")
        # phi = np.arctan(0.2/0.2)
        # prophits_x_corrected = prophits_x*np.cos(phi) - prophits_y*np.sin(phi)
        # prophits_y_corrected = prophits_x*np.sin(phi) + prophits_y*np.cos(phi)
        # prophits_x, prophits_y = prophits_x_corrected, prophits_y_corrected
        # residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        chosen_cluster_sizes = [1, 2, 3, 4, 5, 6]
        directions = ["x", "y"]
        for chamber in range(4):
            residuals = [residuals_x[chamber], residuals_y[chamber]]
            cluster_sizes = [cluster_size_x[chamber], cluster_size_y[chamber]]
            rechits = [rechits_x[chamber], rechits_y[chamber]]
            prophits = [prophits_x[chamber], prophits_y[chamber]]

            horns_fig, horns_axs = plt.subplots(
                nrows=2, ncols=len(chosen_cluster_sizes),
                figsize=(10*len(chosen_cluster_sizes),14)
            )
            horns2d_fig, horns2d_axs = plt.subplots(
                nrows=2, ncols=len(chosen_cluster_sizes),
                figsize=(10*len(chosen_cluster_sizes),14)
            )
            print(f"Plotting horns for chamber {chamber}...")
            for icol,cls in enumerate(chosen_cluster_sizes):
                #cluster_sizes = list(range(1,10))
                for iplot in range(2):
                    direction = directions[iplot]
                    cluster_size = cluster_sizes[iplot]
                    residual = residuals[iplot]
                    prophit = prophits[iplot]

                    data = prophit[cluster_size==cls]
                    # points, bins = np.histogram(data, bins=1000, range=(-.5, .5))
                    # bins = bins[:-1]+ 0.5*(bins[1:] - bins[:-1])
                    # horns_axs[iplot].plot(bins, points)
                    
                    horns_axs[iplot][icol].hist(data, bins=100, range=[-.5, .5])
                    horns_axs[iplot][icol].set_title(f"Cluster size {cls}, direction {direction}")
                    horns_axs[iplot][icol].set_xlabel("Propagated hit position (mm)")

                    counts, xedges, yedges, im = horns2d_axs[iplot][icol].hist2d(data, residual[cluster_size==cls], bins=100, range=[[-.5, .5],[-.1, .1]])
                    horns2d_axs[iplot][icol].set_title(f"Cluster size {cls}, direction {direction}")
                    horns2d_axs[iplot][icol].set_xlabel("Propagated hit position (mm)")
                    horns2d_axs[iplot][icol].set_ylabel("Residual (mm)")
                    horns2d_fig.colorbar(im, ax=horns2d_axs[iplot][icol])

            horns_fig.tight_layout()
            horns_fig.savefig(f"{odir}/horns_{chamber}.png")

            horns2d_fig.tight_layout()
            horns2d_fig.savefig(f"{odir}/horns2d_{chamber}.png")

if __name__=='__main__': main()
