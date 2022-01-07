import os, sys, pathlib
import argparse
from tqdm import tqdm

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

def analyse_residuals(residuals, range, nbins, ax, legend, xlabel):
    points, bins = np.histogram(residuals, bins=nbins, range=range)
    bins = bins[:-1]+ 0.5*(bins[1:] - bins[:-1])
    
    # gaussian fit
    coeff = [len(residuals), residuals.mean(), residuals.std()]
    coeff += [len(residuals)*0.1, residuals.mean(), 10*residuals.std()]
    try:
        coeff, var_matrix = curve_fit(gauss2, bins, points, p0=coeff, method="lm")
    except RuntimeError:
        print("Fit failed, using RMS instead...")
    space_resolution = 1e3*coeff[2]
    
    # plot data and fit
    ax.hist(
        residuals, bins=nbins, range=range,
        histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k",
        label = legend
    )
    #ax.scatter(bins, points, marker="o", label=label)
    xvalues = np.linspace(bins[0], bins[-1], 1000)
    ax.plot(xvalues, gauss2(xvalues, *coeff))
    ax.set_xlabel(xlabel)
    #residual_cls_axs[idirection][tested_chamber].legend()

    return space_resolution

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input file")
    parser.add_argument('odir', type=pathlib.Path, help="Output directory")
    parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    with uproot.open(args.ifile) as track_file:
        track_tree = track_file["trackTree"]
        if args.verbose: track_tree.show()

        print("Reading tree...")
        rechits_x = np.vstack(track_tree["rechits2D_X"].array(library="np", entry_stop=args.events)).T
        rechits_y = np.vstack(track_tree["rechits2D_Y"].array(library="np", entry_stop=args.events)).T
        prophits_x = np.vstack(track_tree["prophits2D_X"].array(library="np", entry_stop=args.events)).T
        prophits_y = np.vstack(track_tree["prophits2D_Y"].array(library="np", entry_stop=args.events)).T
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        rechits_x_error = np.vstack(track_tree["rechits2D_X_Error"].array(library="np", entry_stop=args.events)).T
        rechits_y_error = np.vstack(track_tree["rechits2D_Y_Error"].array(library="np", entry_stop=args.events)).T
        cluster_size_x = np.vstack(track_tree["rechits2D_X_ClusterSize"].array(library="np", entry_stop=args.events)).T
        cluster_size_y = np.vstack(track_tree["rechits2D_Y_ClusterSize"].array(library="np", entry_stop=args.events)).T
        prophits_x_error = np.vstack(track_tree["prophits2D_X_Error"].array(library="np", entry_stop=args.events)).T
        prophits_y_error = np.vstack(track_tree["prophits2D_Y_Error"].array(library="np", entry_stop=args.events)).T

        print("Starting plotting...")
        directions = ["x", "y"]
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=4, figsize=(32,14))
        residual_cls_fig, residual_cls_axs = plt.subplots(nrows=2, ncols=4, figsize=(32,14))
        spres_fig, spres_axs = plt.subplots(nrows=1, ncols=4, figsize=(32,7))
        rotation_fig, rotation_axs = plt.subplots(nrows=2, ncols=4, figsize=(32,14))

        for tested_chamber in range(4):
            print(f"Processing chamber {tested_chamber}...")
            rechits = [rechits_x[tested_chamber], rechits_y[tested_chamber]]
            prophits = [prophits_x[tested_chamber], prophits_y[tested_chamber]]
            residuals = [residuals_x[tested_chamber], residuals_y[tested_chamber]]
            cluster_sizes = [cluster_size_x[tested_chamber], cluster_size_y[tested_chamber]]

            space_resolutions = dict()
            properr_fig, properr_ax = plt.figure(figsize=(10,7)), plt.axes()
            residuals2d_fig, residuals2d_ax = plt.subplots(nrows=2, ncols=1, figsize=(10,12))
            residual_cls_axs[0][tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
            cluster_size_cuts = list(range(1,10))
            for idirection in range(2):
                direction = directions[idirection]
                space_resolutions[direction] = list()
                cluster_size = cluster_sizes[idirection]

                # plot residuals for all cluster sizes:
                space_resolution = analyse_residuals(
                    residuals[idirection],
                    (-0.45, 0.45), 200,
                    residual_axs[idirection][tested_chamber],
                    None, f"{directions[idirection]} residual (mm)"
                )
                residual_axs[idirection][tested_chamber].set_title(
                    f"BARI-0{tested_chamber+1} {direction} - {space_resolution:1.0f} µm"
                )
                
                # plot residuals for cluster sizes separately:
                for cls in tqdm(cluster_size_cuts):
                    space_resolutions[direction].append(
                        analyse_residuals(
                            residuals[idirection][cluster_size==cls],
                            (-1.2, 1.2), 50,
                            residual_cls_axs[idirection][tested_chamber],
                            f"cluster size {cls}",
                            f"{directions[idirection]} residual (mm)"
                        )
                    )

                    # residual_cls_axs[idirection][tested_chamber].text(
                    #     2, (1-0.1*parity)*1e6,
                    #     f"Space resolution {space_resolution:1.0f} µm",
                    #     horizontalalignment="right",
                    #     fontsize=20
                    # )
                
                # plot residuals vs propagated position:
                prophit_bins = np.linspace(-30, 30, 7)
                prophit_means, residual_means = list(), list()
                prophit_errors, residual_errors = list(), list()
                for i,b in enumerate(prophit_bins[:-2]):
                    b_min, b_max = b, prophit_bins[i+1]
                    selection = (prophits[idirection]>b_min) & (prophits[idirection]<b_max)
                    prophit_means.append(prophits[idirection][selection].mean())
                    residual_means.append(residuals[idirection][selection].mean())
                    prophit_errors.append(prophits[idirection][selection].std())
                    residual_errors.append(residuals[idirection][selection].std()/np.sqrt(residuals[idirection][selection].size))
                rotation_axs[idirection][tested_chamber].errorbar(
                    prophit_means, residual_means, xerr=prophit_errors, yerr=residual_errors, fmt="o"
                )
                rotation_axs[idirection][tested_chamber].set_ylim(-0.06, 0.06)
                rotation_axs[idirection][tested_chamber].set_xlabel("Propagated hit (mm)")
                rotation_axs[idirection][tested_chamber].set_ylabel("Residual (mm)")
                rotation_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1} {direction}")

                # plot 2D distribution of residuals vs propagated position:
                residuals2d_ax[idirection].hist2d(prophits[idirection], residuals[idirection], bins=100, range=[[-40, 40],[-1, 1]])
                residuals2d_ax[idirection].set_title(f"BARI-0{tested_chamber+1} direction {direction}")
                residuals2d_ax[idirection].set_xlabel("Propagated position (mm)")
                residuals2d_ax[idirection].set_ylabel("Residual (mm)")

                spres_axs[tested_chamber].plot(cluster_size_cuts, space_resolutions[direction], marker="o", label=direction)
                

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
            properr_fig.savefig(os.path.join(args.odir, f"extrapolation_error_{tested_chamber}.png"))

            spres_axs[tested_chamber].set_xlabel("Cluster size")            
            spres_axs[tested_chamber].set_ylabel(f"Space resolution (µm)")
            spres_axs[tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
            spres_axs[tested_chamber].legend()

            residuals2d_fig.tight_layout()
            residuals2d_fig.savefig(os.path.join(args.odir, f"residuals2d_{tested_chamber}.png"))

        print("Saving plots...")
        
        spres_fig.tight_layout()
        spres_fig.savefig(os.path.join(args.odir, "space_resolution.png"))

        residual_fig.tight_layout()
        residual_fig.savefig(os.path.join(args.odir, "residuals.png"))

        residual_cls_fig.tight_layout()
        residual_cls_fig.savefig(os.path.join(args.odir, "residuals_cls.png"))

        rotation_fig.tight_layout()
        rotation_fig.savefig(os.path.join(args.odir, "rotation.png"))

if __name__=='__main__': main()
