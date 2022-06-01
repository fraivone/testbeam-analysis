import os, sys, pathlib
import argparse
from tqdm import tqdm

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

from concurrent.futures import ThreadPoolExecutor
executor = ThreadPoolExecutor(8)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input track file")
    parser.add_argument('odir', type=pathlib.Path, help="Output directory")
    parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyze")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    with uproot.open(args.ifile) as track_file:
        track_tree = track_file["trackTree"]
        if args.verbose: track_tree.show()

        print("Reading tree...")
        rechits_chamber = track_tree["rechits2D_Chamber"].array(entry_stop=args.events)
        rechits_x = track_tree["rechits2D_X"].array(entry_stop=args.events)
        rechits_y = track_tree["rechits2D_Y"].array(entry_stop=args.events)
        prophits_x = track_tree["prophits2D_X"].array(entry_stop=args.events)
        prophits_y = track_tree["prophits2D_Y"].array(entry_stop=args.events)
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        #rechits_x_error = track_tree["rechits2D_X_Error"].array(entry_stop=args.events)
        #rechits_y_error = track_tree["rechits2D_Y_Error"].array(entry_stop=args.events)
        cluster_size_x = track_tree["rechits2D_X_ClusterSize"].array(entry_stop=args.events)
        cluster_size_y = track_tree["rechits2D_Y_ClusterSize"].array(entry_stop=args.events)
        prophits_x_error = track_tree["prophits2D_X_Error"].array(entry_stop=args.events)
        prophits_y_error = track_tree["prophits2D_Y_Error"].array(entry_stop=args.events)
        
        # choose only events with hits in all chambers:
        mask_4hit = ak.count_nonzero(rechits_chamber>=0, axis=1)>3
        rechits_chamber = rechits_chamber[mask_4hit]
        rechits_x, rechits_y = rechits_x[mask_4hit], rechits_y[mask_4hit]
        prophits_x, prophits_y = prophits_x[mask_4hit], prophits_y[mask_4hit]
        residuals_x, residuals_y = residuals_x[mask_4hit], residuals_y[mask_4hit]
        cluster_size_x, cluster_size_y = cluster_size_x[mask_4hit], cluster_size_y[mask_4hit]
        prophits_x_error, prophits_y_error = prophits_x_error[mask_4hit], prophits_y_error[mask_4hit]
        
        # Preparing figures:
        print("Starting plotting...")
        directions = ["x", "y"]
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        profile_fig, profile_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        residual_cls_fig, residual_cls_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        spres_fig, spres_axs = plt.subplots(nrows=1, ncols=4, figsize=(32,7))
        rotation_fig, rotation_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        properr_fig, properr_axs = plt.subplots(nrows=1, ncols=4, figsize=(32,7))
        prophits_fig, prophits_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        residuals2d_xx_fig, residuals2d_xx_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        residuals2d_xy_fig, residuals2d_xy_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))

        angles, err_angles = np.ndarray((4,2)), np.ndarray((4,2))
        for tested_chamber in range(4):
            print(f"Processing chamber {tested_chamber}...")
            rechits = [rechits_x[:,tested_chamber], rechits_y[:,tested_chamber]]
            # apply angular correction to rechits:
            # rechits_corrected = [
            #     rechits[0]*np.cos(ANGLES[tested_chamber]) + rechits[1]*np.sin(ANGLES[tested_chamber]),
            #     -rechits[0]*np.sin(ANGLES[tested_chamber]) + rechits[1]*np.cos(ANGLES[tested_chamber])
            # ]
            # rechits = rechits_corrected

            prophits = [prophits_x[:,tested_chamber], prophits_y[:,tested_chamber]]
            residuals = [prophits[0]-rechits[0], prophits[1]-rechits[1]]
            cluster_sizes = [cluster_size_x[:,tested_chamber], cluster_size_y[:,tested_chamber]]

            space_resolutions = dict()
            cluster_size_cuts = list(range(1,10))

            for idirection in range(2):
                direction = directions[idirection]
                space_resolutions[direction] = list()
                cluster_size = cluster_sizes[idirection]

                # plot propagated hits
                prophits_axs[idirection][tested_chamber].hist(
                    prophits[idirection], bins=200, range=(-0.5,0.5),
                    histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k",
                )
                prophits_axs[idirection][tested_chamber].set_title(
                    f"BARI-0{tested_chamber+1} {direction}"
                )
                binsize = 1/200*1e3
                prophits_axs[idirection][tested_chamber].set_xlabel("Propagated hit position (mm)")
                prophits_axs[idirection][tested_chamber].set_ylabel(f"Events/{int(binsize)} µm")

                profile_axs[idirection][tested_chamber].plot(rechits[idirection])
                profile_axs[idirection][tested_chamber].set_xlabel("Reconstructed x (mm)")

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
                residual_cls_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1} {direction}")
                
                # plot residuals for cluster sizes separately:
                for cls in tqdm(cluster_size_cuts):
                    space_resolutions[direction].append(
                        analyse_residuals(
                            residuals[idirection][cluster_size==cls],
                            (-0.45, 0.45), 200,
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
                prophit_bins = np.linspace(-30, 30, 15)
                prophit_means, residual_means = list(), list()
                prophit_errors, residual_errors = list(), list()
                for i,b in enumerate(prophit_bins[:-1]):
                    b_min, b_max = b, prophit_bins[i+1]
                    selection = (prophits[idirection]>b_min) & (prophits[idirection]<b_max)
                    prophit_means.append(ak.mean(prophits[idirection][selection]))
                    residual_means.append(ak.mean(residuals[idirection-1][selection]))
                    prophit_errors.append(ak.std(prophits[idirection][selection]))
                    residual_errors.append(ak.std(residuals[idirection-1][selection])/np.sqrt(ak.num(residuals[idirection][selection], axis=0)))
                   
                """ determine rotation corrections: """
                rotation_axs[idirection][tested_chamber].errorbar(
                    prophit_means, residual_means, xerr=prophit_errors, yerr=residual_errors, fmt="o"
                )
                # fit with line and plot result:
                coeff = [
                    0.5*(residual_means[0]+residual_means[-1]),
                    (residual_means[-1]+residual_means[0])/(prophit_means[-1]+prophit_means[0])
                ]
                coeff, var_matrix = curve_fit(linear_function, prophit_means, residual_means, p0=coeff, method="lm")
                q, m = coeff
                err_q, err_m = np.sqrt(np.diag(var_matrix))

                # calculate angles:
                theta = np.arcsin(m)
                err_theta = err_m/np.sqrt(1-m**2)
                angles[tested_chamber][idirection] = (-1)**idirection*theta
                err_angles[tested_chamber][idirection] = err_theta

                x_fit = np.linspace(-30, 30, 100)
                rotation_axs[idirection][tested_chamber].plot(x_fit, linear_function(x_fit, *coeff), color="red")
                rotation_axs[idirection][tested_chamber].text(
                    0.7, 0.8,
                    f"m = {m:1.1e} $\pm$ {err_m:1.1f}\n"+
                    f"q = {q*1e3:1.2f} $\pm$ {err_q*1e3:1.1f} µm\n"+
                    f"ϑ = {theta*1e3:1.1f} $\pm$ {err_theta*1e3:1.1f} mrad",
                    transform=rotation_axs[idirection][tested_chamber].transAxes,
                    bbox=dict(boxstyle="square, pad=0.5", ec="black", fc="none")
                )

                #rotation_axs[idirection][tested_chamber].set_xlim(-30, 30)
                #rotation_axs[idirection][tested_chamber].set_ylim(-0.06, 0.06)
                rotation_axs[idirection][tested_chamber].set_xlabel(f"Propagated hit {direction} (mm)")
                rotation_axs[idirection][tested_chamber].set_ylabel(f"Residual {directions[idirection-1]} (mm)")
                rotation_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1}")

                # plot 2D distribution of residuals vs propagated position:
                residuals2d_xy_axs[idirection][tested_chamber].hist2d(prophits[idirection], residuals[idirection-1], bins=100, range=[[-40, 40],[-0.6, 0.6]])
                residuals2d_xy_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
                residuals2d_xy_axs[idirection][tested_chamber].set_xlabel(f"Propagated position {direction} (mm)")
                residuals2d_xy_axs[idirection][tested_chamber].set_ylabel(f"Residual {directions[idirection-1]} (mm)")

                # plot 2D distribution of residuals vs propagated position:
                residuals2d_xx_axs[idirection][tested_chamber].hist2d(prophits[idirection], residuals[idirection], bins=100, range=[[-40, 40],[-0.6, 0.6]])
                residuals2d_xx_axs[idirection][tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
                residuals2d_xx_axs[idirection][tested_chamber].set_xlabel(f"Propagated position {direction} (mm)")
                residuals2d_xx_axs[idirection][tested_chamber].set_ylabel(f"Residual {direction} (mm)")

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
            properr_axs[tested_chamber].hist(prophits_x_error[:,tested_chamber], bins=200, label="x", alpha=0.3)
            properr_axs[tested_chamber].hist(prophits_y_error[:,tested_chamber], bins=200, label="y", alpha=0.3)
            properr_axs[tested_chamber].set_xlim(0, 1)
            properr_axs[tested_chamber].set_xlabel("Extrapolation uncertainty (mm)")
            properr_axs[tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
            properr_axs[tested_chamber].legend()

            spres_axs[tested_chamber].set_xlabel("Cluster size")            
            spres_axs[tested_chamber].set_ylabel(f"Residual sigma (µm)")
            spres_axs[tested_chamber].set_title(f"BARI-0{tested_chamber+1}")
            spres_axs[tested_chamber].legend()

        print("Saving plots...")
        
        spres_fig.tight_layout()
        spres_fig.savefig(os.path.join(args.odir, "space_resolution.png"))

        residual_fig.tight_layout()
        residual_fig.savefig(os.path.join(args.odir, "residuals.png"))

        profile_fig.tight_layout()
        profile_fig.savefig(os.path.join(args.odir, "profiles.png"))

        residual_cls_fig.tight_layout()
        residual_cls_fig.savefig(os.path.join(args.odir, "residuals_cls.png"))

        properr_fig.tight_layout()
        properr_fig.savefig(os.path.join(args.odir, "extrapolation_error.png"))
        
        rotation_fig.tight_layout()
        rotation_fig.savefig(os.path.join(args.odir, "rotation.png"))

        prophits_fig.tight_layout()
        prophits_fig.savefig(os.path.join(args.odir, "prophits.png"))

        residuals2d_xx_fig.tight_layout()
        residuals2d_xx_fig.savefig(os.path.join(args.odir, "residuals2d_xx.png"))

        residuals2d_xy_fig.tight_layout()
        residuals2d_xy_fig.savefig(os.path.join(args.odir, "residuals2d_xy.png"))

        # combine x and y angle corrections, then save:
        correction_angles = {
            "angle": np.sum(angles/err_angles**2, axis=1)/np.sum(1/err_angles**2, axis=1),
            "error": np.sqrt(1/np.sum(1/err_angles**2, axis=1))
        }
        pd.DataFrame.from_dict(correction_angles).T.to_csv(
            os.path.join(args.odir, "angles.txt"), sep=" "
        )

if __name__=='__main__': main()
