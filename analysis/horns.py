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
            print("Events: ", len(track_tree["trackFitChi2"].array(library="np")))
            track_tree.show()

        print("Reading tree...")
        prophits_x, prophits_y = track_tree["prophitX"].array(library="np"), track_tree["prophitY"].array(library="np")
        rechits_x, rechits_y = track_tree["rechitX"].array(library="np"), track_tree["rechitY"].array(library="np")
        cluster_size_x, cluster_size_y = track_tree["rechitXClusterSize"].array(library="np"), track_tree["rechitYClusterSize"].array(library="np")

        print("Applying angular alignment...")
        phi = np.arctan(0.2/0.2)
        prophits_x_corrected = prophits_x*np.cos(phi) - prophits_y*np.sin(phi)
        prophits_y_corrected = prophits_x*np.sin(phi) + prophits_y*np.cos(phi)
        prophits_x, prophits_y = prophits_x_corrected, prophits_y_corrected

        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        chosen_cluster_sizes = [1, 2, 3, 4, 5, 6]
        residuals = [residuals_x, residuals_y]
        cluster_sizes = [cluster_size_x, cluster_size_y]
        rechits = [rechits_x, rechits_y]
        prophits = [prophits_x, prophits_y]
        directions = ["x", "y"]

        horns_fig, horns_axs = plt.subplots(
            nrows=2, ncols=len(chosen_cluster_sizes),
            figsize=(10*len(chosen_cluster_sizes),14)
        )
        horns2d_fig, horns2d_axs = plt.subplots(
            nrows=2, ncols=len(chosen_cluster_sizes),
            figsize=(10*len(chosen_cluster_sizes),14)
        )
        print("Plotting horns...")
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
        horns_fig.savefig(f"{odir}/horns.png")

        horns2d_fig.tight_layout()
        horns2d_fig.savefig(f"{odir}/horns2d.png")

if __name__=='__main__': main()
