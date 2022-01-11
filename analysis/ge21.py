import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import scipy
from scipy.optimize import curve_fit

import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def linear_function(x, *p):
    q, m = p
    return q + m*x

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
        rechit_chamber = track_tree["rechitChamber"].array(entry_stop=args.events)
        prophit_chamber = track_tree["prophitChamber"].array(entry_stop=args.events)
        rechits_eta = track_tree["rechitEta"].array(entry_stop=args.events)
        prophits_eta = ak.flatten(track_tree["prophitEta"].array(entry_stop=args.events))
        rechits_x = track_tree["rechitLocalX"].array(entry_stop=args.events)
        rechits_y = track_tree["rechitLocalY"].array(entry_stop=args.events)
        prophits_x = ak.flatten(track_tree["prophitLocalX"].array(entry_stop=args.events))
        prophits_y = ak.flatten(track_tree["prophitLocalY"].array(entry_stop=args.events))
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        vec_size = np.vectorize(lambda x: x.size)
        vec_argmin = np.vectorize(lambda x:x.argmin())
        nonzero_mask = ak.count(rechits_x, axis=1)!=0
        good_events = len(rechits_x[nonzero_mask])
        print(f"Efficiency {good_events/len(rechits_x):1.2f}")

        eta_fig, eta_axs = plt.subplots(nrows=1, ncols=3, figsize=(36,9))
        #prophits_eta = 4 - np.floor(prophits_y/107.7)
        eta_mask = ak.count(rechits_eta, axis=1)==1
        rechits_eta_single, prophits_eta_single = ak.flatten(rechits_eta[eta_mask]), prophits_eta[eta_mask]
        eta_axs[0].hist(
            rechits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        eta_axs[1].hist(
            prophits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        eta_axs[2].hist(
            prophits_eta_single-rechits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        print("Mean", ak.mean(rechits_eta_single-prophits_eta_single))
        eta_fig.tight_layout()
        eta_fig.savefig(os.path.join(args.odir, "eta.png"))

        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=1, figsize=(12,18))
        residual_rechit_fig, residual_rechit_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        residual_prophit_fig, residual_prophit_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        ranges = [(-200, -100), (-300, -150)]
        for idirection,residuals in enumerate([residuals_x, residuals_y]):
            direction = ["x", "y"][idirection]
            idirection_other = int(not idirection)
            direction_other = ["x", "y"][idirection_other]

            rechits = [rechits_x,rechits_y][idirection]
            prophits = [prophits_x,prophits_y][idirection]
            # prophits = [prophits_x,prophits_y][idirection][ak.count(residuals, axis=1)>0]
            # rechits = [rechits_x,rechits_y][idirection][ak.count(residuals, axis=1)>0]
            # residuals = residuals[ak.count(residuals, axis=1)>0]
            # min_residual_mask = ak.flatten(ak.argmin(abs(residuals), axis=1), axis=0)
            # residuals = residuals[np.arange(ak.num(residuals, axis=0)),min_residual_mask]
            # rechits = rechits[np.arange(ak.num(rechits, axis=0)),min_residual_mask]

            single_hit_mask = ak.count(rechits_x, axis=1)==1
            prophits = prophits[single_hit_mask]
            rechits = ak.flatten(rechits[single_hit_mask])
            residuals = ak.flatten(residuals[single_hit_mask])

            residual_axs[idirection].hist(
                residuals, bins=100, range=ranges[idirection],
                histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
            )
            residual_axs[idirection].set_xlabel(f"Residual {direction} (mm)")

            # plot x(y) residuals vs x(y) coordinate
            residual_rechit_axs[idirection][0].hist2d(rechits, residuals, bins=100)
            residual_rechit_axs[idirection][0].set_xlabel(f"Rechit {direction} (mm)")
            residual_rechit_axs[idirection][0].set_ylabel(f"Residual {direction} (mm)")
            residual_prophit_axs[idirection][0].hist2d(prophits, residuals, bins=100)
            residual_prophit_axs[idirection][0].set_xlabel(f"Propagated {direction} (mm)")
            residual_prophit_axs[idirection][0].set_ylabel(f"Residual {direction} (mm)")
            
            # plot x(y) residuals vs y(x) coordinate
            residual_rechit_axs[idirection][1].hist2d(
                ak.flatten([rechits_x,rechits_y][idirection_other][single_hit_mask]), residuals, bins=100
            )
            residual_rechit_axs[idirection][1].set_xlabel(f"Rechit {direction_other} (mm)")
            residual_rechit_axs[idirection][1].set_ylabel(f"Residual {direction} (mm)")
            residual_prophit_axs[idirection][1].hist2d(
                [prophits_x,prophits_y][idirection_other][single_hit_mask], residuals, bins=100
            )
            residual_prophit_axs[idirection][1].set_xlabel(f"Propagated {direction_other} (mm)")
            residual_prophit_axs[idirection][1].set_ylabel(f"Residual {direction} (mm)")

        residual_fig.tight_layout()
        residual_fig.savefig(os.path.join(args.odir, "residuals.png"))

        residual_rechit_fig.tight_layout()
        residual_rechit_fig.savefig(os.path.join(args.odir, "residuals_rechits.png"))

        residual_prophit_fig.tight_layout()
        residual_prophit_fig.savefig(os.path.join(args.odir, "residuals_prophits.png"))

        return 0

        print("Starting plotting...")
        directions = ["x", "y"]
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        residual_cls_fig, residual_cls_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))
        spres_fig, spres_axs = plt.subplots(nrows=1, ncols=4, figsize=(32,7))
        rotation_fig, rotation_axs = plt.subplots(nrows=2, ncols=4, figsize=(50,18))

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
                            (-1.2, 1.2), 100,
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
                    prophit_means.append(prophits[idirection][selection].mean())
                    residual_means.append(residuals[int(not idirection)][selection].mean())
                    prophit_errors.append(prophits[idirection][selection].std())
                    residual_errors.append(residuals[int(not idirection)][selection].std()/np.sqrt(residuals[idirection][selection].size))
                   
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
                #theta = np.arccos(1-abs(m))*1e3
                theta = np.arcsin(m)*1e3
                x_fit = np.linspace(-30, 30, 100)
                rotation_axs[idirection][tested_chamber].plot(x_fit, linear_function(x_fit, *coeff), color="red")
                rotation_axs[idirection][tested_chamber].text(
                    0.75, 0.8,
                    f"m = {m:1.1e}\nq = {q*1e3:1.2f} µm\nϑ = {theta:1.1f} mrad",
                    transform=rotation_axs[idirection][tested_chamber].transAxes,
                    bbox=dict(boxstyle="square, pad=0.5", ec="black", fc="none")
                )

                #rotation_axs[idirection][tested_chamber].set_xlim(-30, 30)
                #rotation_axs[idirection][tested_chamber].set_ylim(-0.06, 0.06)
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
