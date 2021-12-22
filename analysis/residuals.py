import os, sys, logging
from posixpath import join

import uproot
import numpy as np
import scipy
from scipy.optimize import curve_fit

import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def gauss(x, *p):
    A, mu, sigma = p
    return A*np.exp(-(x-mu)**2/(2.*sigma**2))

def gauss2(x, *p):
    A1, mu1, sigma1, A2, mu2, sigma2 = p
    return A1*scipy.stats.norm.pdf(x, loc=mu1, scale=sigma1) + \
    A2*scipy.stats.norm.pdf(x, loc=mu2, scale=sigma2)
    #return gauss(x, A1, mu1, sigma1) + gauss(x, A2, mu2, sigma2)

def main():
    ifile, odir = sys.argv[1], sys.argv[2]
    os.makedirs(odir, exist_ok=True)

    if "-v" in sys.argv or "--verbose" in sys.argv:
        verbose = True
    else: verbose = False

    with uproot.open(ifile) as track_file:
        track_tree = track_file["trackTree"]
        if verbose: track_tree.show()

        print("Reading tree...")
        rechits_x = np.vstack(track_tree["rechits2D_X"].array(library="np", entry_stop=-1)).T
        rechits_y = np.vstack(track_tree["rechits2D_Y"].array(library="np", entry_stop=-1)).T
        prophits_x = np.vstack(track_tree["prophits2D_X"].array(library="np", entry_stop=-1)).T
        prophits_y = np.vstack(track_tree["prophits2D_Y"].array(library="np", entry_stop=-1)).T
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        rechits_x_error = np.vstack(track_tree["rechits2D_X_Error"].array(library="np", entry_stop=-1)).T
        rechits_y_error = np.vstack(track_tree["rechits2D_Y_Error"].array(library="np", entry_stop=-1)).T
        cluster_size_x = np.vstack(track_tree["rechits2D_X_ClusterSize"].array(library="np", entry_stop=-1)).T
        cluster_size_y = np.vstack(track_tree["rechits2D_Y_ClusterSize"].array(library="np", entry_stop=-1)).T
        prophits_x_error = np.vstack(track_tree["prophits2D_X_Error"].array(library="np", entry_stop=-1)).T
        prophits_y_error = np.vstack(track_tree["prophits2D_Y_Error"].array(library="np", entry_stop=-1)).T

        print("Starting plotting...")
        directions = ["x", "y"]
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=4, figsize=(32,14))
        residual_cls_fig, residual_cls_axs = plt.subplots(nrows=2, ncols=4, figsize=(32,14))
        spres_fig, spres_axs = plt.subplots(nrows=1, ncols=4, figsize=(32,7))
        for tested_chamber in range(4):
            print(f"Processing chamber {tested_chamber}...")
            prophits = [prophits_x[tested_chamber], prophits_y[tested_chamber]]
            residuals = [residuals_x[tested_chamber], residuals_y[tested_chamber]]
            cluster_sizes = [cluster_size_x[tested_chamber], cluster_size_y[tested_chamber]]

            properr_fig, properr_ax = plt.figure(figsize=(10,7)), plt.axes()
            space_resolutions = dict()
            residuals2d_fig, residuals2d_ax = plt.subplots(nrows=2, ncols=1, figsize=(10,12))
            cluster_size_cuts = [0]
            cluster_size_cuts = list(range(1,10))
            for idirection in range(2):
                direction = directions[idirection]
                space_resolutions[direction] = list()
                cluster_size = cluster_sizes[idirection]
                for cls in cluster_size_cuts:
                #for parity in [0, 1]:
                    print("Cluster size", cls)
                    #even_or_odd = "even"*(parity==0) + "odd"*(parity==1)
                    #data = residuals[idirection][cluster_size%2==parity]
                    data = residuals[idirection][cluster_size==cls]
                    #data = residuals[idirection]

                    points, bins = np.histogram(data, bins=50, range=(-1.2, 1.2))
                    bins = bins[:-1]+ 0.5*(bins[1:] - bins[:-1])
                    
                    # gaussian fit
                    coeff = [len(data), data.mean(), data.std()]
                    coeff += [len(data)*0.1, data.mean(), 10*data.std()]
                    coeff, var_matrix = curve_fit(gauss2, bins, points, p0=coeff)
                    #print(f"Before fit: {p0}. Fit results: {coeff}")
                    space_resolution = 1e3*coeff[2]
                    space_resolutions[direction].append(space_resolution)
                    
                    # plot data and fit
                    label = f"size {cls} - {space_resolution:1.0f} µm"
                    residual_cls_axs[idirection][tested_chamber].hist(
                        data, bins=50, range=(-1.2, 1.2),
                        histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k",
                        label=label
                    )
                    #residual_cls_axs[idirection][tested_chamber].scatter(bins, points, marker="o", label=label)
                    xvalues = np.linspace(bins[0], bins[-1], 1000)
                    residual_cls_axs[idirection][tested_chamber].plot(xvalues, gauss2(xvalues, *coeff))
                    residual_cls_axs[idirection][tested_chamber].set_xlabel(f"{directions[idirection]} residual (mm)")
                    #residual_cls_axs[idirection][tested_chamber].legend()

                    if idirection==0:
                        residual_cls_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1}")

                    # residual_cls_axs[idirection][tested_chamber].text(
                    #     2, (1-0.1*parity)*1e6,
                    #     f"Space resolution {space_resolution:1.0f} µm",
                    #     horizontalalignment="right",
                    #     fontsize=20
                    # )

                spres_axs[tested_chamber].plot(cluster_size_cuts, space_resolutions[direction], marker="o", label=direction)
                
                residuals2d_ax[idirection].hist2d(prophits[idirection], residuals[idirection], bins=100, range=[[-40, 40],[-1, 1]])
                residuals2d_ax[idirection].set_title(f"BARI-0{tested_chamber+1} direction {direction}")
                residuals2d_ax[idirection].set_xlabel("Propagated position (mm)")
                residuals2d_ax[idirection].set_ylabel("Residual (mm)")

            # bins_x = (matched_bins_x + 0.5*(matched_bins_x[1]-matched_bins_x[0]))[:-1]
            # bins_y = (matched_bins_y + 0.5*(matched_bins_y[1]-matched_bins_y[0]))[:-1]
            # #plt.contourf(bins_x, bins_y, efficiency)
            # plt.imshow(efficiency, extent=eff_range[0]+eff_range[1], origin="lower")
            # plt.xlabel("x (mm)")
            # plt.ylabel("y (mm)")
            # plt.colorbar(label="Efficiency")
            # plt.tight_layout()
            # plt.text(eff_range[0][-1]-.5, eff_range[1][-1]+2, "GEM-10x10-380XY-BARI-04", horizontalalignment="right")

            # plot propagation errors
            properr_ax.hist(prophits_x_error[tested_chamber], bins=70, label="x", alpha=0.3)
            properr_ax.hist(prophits_y_error[tested_chamber], bins=70, label="y", alpha=0.3)
            properr_ax.set_xlim(0, 0.7)
            properr_ax.set_xlabel("Extrapolation uncertainty (mm)")
            properr_ax.set_title(f"BARI-0{tested_chamber+1}")
            properr_ax.legend()
            properr_fig.savefig(os.path.join(odir, f"extrapolation_error_{tested_chamber}.png"))

            spres_axs[tested_chamber].set_xlabel("Cluster size")            
            spres_axs[tested_chamber].set_ylabel(f"Space resolution (µm)")
            spres_axs[tested_chamber].legend()

            residuals2d_fig.tight_layout()
            residuals2d_fig.savefig(os.path.join(odir, f"residuals2d_{tested_chamber}.png"))

        spres_fig.tight_layout()
        spres_fig.savefig(os.path.join(odir, "space_resolution.png"))

        residual_cls_fig.tight_layout()
        residual_cls_fig.savefig(os.path.join(odir, "residuals.png"))

if __name__=='__main__': main()
