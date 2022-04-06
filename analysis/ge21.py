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
        # 1D branches:
        track_x_chi2 = track_tree["trackChi2X"].array(entry_stop=args.events)
        track_y_chi2 = track_tree["trackChi2Y"].array(entry_stop=args.events)
        rechit_chamber = track_tree["rechitChamber"].array(entry_stop=args.events)
        prophit_chamber = track_tree["prophitChamber"].array(entry_stop=args.events)
        rechits_eta = track_tree["rechitEta"].array(entry_stop=args.events)
        prophits_eta = track_tree["prophitEta"].array(entry_stop=args.events)
        rechits_x = track_tree["rechitGlobalX"].array(entry_stop=args.events)
        rechits_y = track_tree["rechitGlobalY"].array(entry_stop=args.events)
        rechits_cluster_center = track_tree["rechitClusterCenter"].array(entry_stop=args.events)
        rechits_cluster_size = track_tree["rechitClusterSize"].array(entry_stop=args.events)
        digi_strip = track_tree["rechitDigiStrip"].array(entry_stop=args.events)
        raw_channel = track_tree["rechitRawChannel"].array(entry_stop=args.events)
        track_intercept_x = track_tree["trackInterceptX"].array(entry_stop=args.events)
        track_intercept_y = track_tree["trackInterceptY"].array(entry_stop=args.events)
        prophits_x = -track_tree["prophitGlobalY"].array(entry_stop=args.events)
        prophits_y = track_tree["prophitGlobalX"].array(entry_stop=args.events)

        # 2D branches:
        rechits2d_chamber = track_tree["rechits2D_Chamber"].array(entry_stop=args.events)
        rechits2d_x = track_tree["rechits2D_X"].array(entry_stop=args.events)
        rechits2d_y = track_tree["rechits2D_Y"].array(entry_stop=args.events)
        prophits2d_x = track_tree["prophits2D_X"].array(entry_stop=args.events)
        prophits2d_y = track_tree["prophits2D_Y"].array(entry_stop=args.events)

        tracker_test_chamber = 3
        has_chamber2 = ak.count(rechits2d_chamber==tracker_test_chamber, axis=1)>0
        #print("Has chamber 2", has_chamber2)
        #print("Rechits masked with 'has chamber 2'", ak.mask(rechits2d_x, rechits2d_chamber==tracker_test_chamber))
        masked_chamber2 = ak.mask(rechits2d_x, rechits2d_chamber==tracker_test_chamber)
        print("masked_chamber2", masked_chamber2)
        with_chamber2 = ak.count(masked_chamber2!=None, axis=1)>0
        print("with_chamber2", with_chamber2)
        rechits2d_x_chamber2 = ak.mask(masked_chamber2[rechits2d_chamber==tracker_test_chamber], with_chamber2)
        rechits2d_x_chamber2 = ak.fill_none(rechits2d_x_chamber2, [0])
        rechits2d_x_chamber2 = ak.flatten(rechits2d_x_chamber2)
        print("rechits2d_x_chamber2", rechits2d_x_chamber2)

        # print("Rechits 2D x")
        # for i in range(20):
        #     print("    chamber: ", rechits2d_chamber[i])
        #     print("    x:       ", rechits2d_x[i])

        mask_chi2 = (track_x_chi2>0.1)&(track_x_chi2<2)&(track_y_chi2>0.1)&(track_y_chi2<2)
        rechit_chamber = rechit_chamber[mask_chi2]
        prophit_chamber = prophit_chamber[mask_chi2]
        rechits_eta = rechits_eta[mask_chi2]
        rechits_cluster_center = rechits_cluster_center[mask_chi2]
        rechits_cluster_size = rechits_cluster_size[mask_chi2]
        digi_strip = digi_strip[mask_chi2]
        raw_channel = raw_channel[mask_chi2]
        prophits_eta = prophits_eta[mask_chi2]
        rechits_x = rechits_x[mask_chi2]
        rechits_y = rechits_y[mask_chi2]
        prophits_x = prophits_x[mask_chi2]
        prophits_y = prophits_y[mask_chi2]
        track_intercept_x = track_intercept_x[mask_chi2]
        track_intercept_y = track_intercept_y[mask_chi2]
        track_x_chi2 = track_x_chi2[mask_chi2]
        track_y_chi2 = track_y_chi2[mask_chi2]
        rechits2d_x_chamber2 = rechits2d_x_chamber2[mask_chi2]

        ge21_chamber = 4
        prophits_x, prophits_y = ak.flatten(prophits_x[prophit_chamber==ge21_chamber]), ak.flatten(prophits_y[prophit_chamber==ge21_chamber])
        prophits_eta = ak.flatten(prophits_eta[prophit_chamber==ge21_chamber])
        rechits_x, rechits_y = rechits_x[rechit_chamber==ge21_chamber], rechits_y[rechit_chamber==ge21_chamber]
        rechits_eta = rechits_eta[rechit_chamber==ge21_chamber]
        rechits_cluster_size = rechits_cluster_size[rechit_chamber==ge21_chamber]
        digi_strip = digi_strip[rechit_chamber==ge21_chamber]
        raw_channel = raw_channel[rechit_chamber==ge21_chamber]
        rechits_cluster_center = rechits_cluster_center[rechit_chamber==ge21_chamber]
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y
        
        print("track intercept x:", track_intercept_x)
        print("rechit chamber:", rechit_chamber)
        print("cluster center:", rechits_cluster_center)
        print("cluster size:", rechits_cluster_size)
        print("digi strip:", digi_strip)
        print("raw channel:", raw_channel)
        print("prophits global x:", prophits_x)
        print("prophits global y:", prophits_y)
        print("rechits global x:", rechits_x)
        print("rechits global y:", rechits_y)
        print("residuals x:", residuals_x)
        print("residuals y:", residuals_y)
        print("Average x residual:", ak.mean(residuals_x))
        print("Average y residual:", ak.mean(residuals_y))

        occupancy_fig, occupancy_axs = plt.subplots(nrows=1, ncols=2, figsize=(20,9))
        #single_hit_mask = ak.count(rechits_x, axis=1)==1
        occupancy_axs[0].hist2d(
            ak.flatten(rechits_x),
            ak.flatten(rechits_y),
            bins=40
        )
        occupancy_axs[0].set_xlabel("Rechit x")
        occupancy_axs[0].set_ylabel("Rechit y")
        occupancy_axs[1].hist2d(
            prophits_x,
            prophits_y,
            bins=40
        )
        occupancy_axs[1].set_xlabel("Prophit x")
        occupancy_axs[1].set_ylabel("Prophit y")
        #for ax in occupancy_axs: ax.legend()
        occupancy_fig.tight_layout()
        occupancy_fig.savefig(args.odir/"occupancy.png")

        occupancy_cls_fig, occupancy_cls_ax = plt.subplots(figsize=(12,9))
        #single_hit_mask = ak.count(rechits_x, axis=1)==1
        h, x, y, img = occupancy_cls_ax.hist2d(
            ak.flatten(rechits_x),
            ak.flatten(rechits_cluster_size),
            range=((-100, 100), (0, 10)),
            bins=40
        )
        occupancy_cls_ax.set_xlabel("Rechit x")
        occupancy_cls_ax.set_ylabel("Cluster size")
        occupancy_cls_fig.colorbar(img, ax=occupancy_cls_ax)
        occupancy_cls_fig.tight_layout()
        occupancy_cls_fig.savefig(args.odir/"occupancy_cluster_size.png")

        eta_fig, eta_axs = plt.subplots(nrows=1, ncols=3, figsize=(36,9))
        #prophits_eta = 4 - np.floor(prophits_y/107.7)
        eta_mask = ak.count(rechits_eta, axis=1)==1
        rechits_eta_single, prophits_eta_single = ak.flatten(rechits_eta[eta_mask]), prophits_eta[eta_mask]
        eta_axs[0].hist(
            rechits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        eta_axs[0].set_xlabel("Rechit eta partition")
        eta_axs[1].hist(
            prophits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        eta_axs[1].set_xlabel("Propagated hit eta partition")
        eta_axs[2].hist(
            prophits_eta_single-rechits_eta_single, bins=10, range=(0.5,10.5),
            histtype="stepfilled", linewidth=1, facecolor="none", edgecolor="k"
        )
        eta_axs[2].set_xlabel("Residual eta partition")
        mean_eta = ak.mean(rechits_eta_single-prophits_eta_single)
        print("Mean eta", mean_eta)
        eta_axs[2].text(
            0.6, 0.8,
            f"mean residual {mean_eta:1.2f}",
            transform=eta_axs[2].transAxes,
            bbox=dict(boxstyle="square, pad=0.5", ec="black", fc="none")
        )
        eta_fig.tight_layout()
        eta_fig.savefig(args.odir/"eta.png")

        hits_fig, hits_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=1, figsize=(12,18))
        residual_rechit_fig, residual_rechit_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        residual_prophit_fig, residual_prophit_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        rechit_prophit_fig, rechit_prophit_axs = plt.subplots(nrows=2, ncols=2, figsize=(24,18))
        ranges = [(-10, 10), (-100, 100)]

        cluster_prophit_fig, cluster_prophit_axs = plt.subplots(nrows=1, ncols=6, figsize=(12*6,9))
        single_hit_mask = ak.count(rechits_cluster_center, axis=1)==1
        cluster_prophit_axs[0].hist2d(
            track_intercept_x[single_hit_mask],
            ak.flatten(rechits_cluster_center[single_hit_mask]),
            bins=100
        )
        cluster_prophit_axs[0].set_xlabel(f"Propagated x (mm)")
        cluster_prophit_axs[0].set_ylabel(f"Cluster center")
        cluster_prophit_axs[1].hist2d(
            track_intercept_x[single_hit_mask],
            ak.mean(digi_strip[single_hit_mask], axis=1),
            bins=100
        )
        cluster_prophit_axs[1].set_xlabel(f"Propagated x (mm)")
        cluster_prophit_axs[1].set_ylabel(f"Average strip")
        cluster_prophit_axs[2].hist2d(
            track_intercept_x[single_hit_mask],
            ak.mean(raw_channel[single_hit_mask], axis=1),
            bins=100
        )
        cluster_prophit_axs[2].set_xlabel(f"Propagated x (mm)")
        cluster_prophit_axs[2].set_ylabel(f"Average VFAT channel")
        cluster_prophit_axs[3].hist2d(
            track_intercept_y[single_hit_mask],
            ak.mean(raw_channel[single_hit_mask], axis=1),
            bins=100
        )
        cluster_prophit_axs[3].set_xlabel(f"Propagated y (mm)")
        cluster_prophit_axs[3].set_ylabel(f"Average VFAT channel")
        print(track_intercept_x[single_hit_mask], ak.count(track_intercept_x[single_hit_mask]))
        print(rechits2d_x_chamber2[single_hit_mask], ak.count(rechits2d_x_chamber2[single_hit_mask], axis=0))
        cluster_prophit_axs[4].hist2d(
            track_intercept_x[single_hit_mask],
            rechits2d_x_chamber2[single_hit_mask],
            bins=100
        )
        cluster_prophit_axs[4].set_xlabel(f"Propagated x (mm)")
        cluster_prophit_axs[4].set_ylabel(f"Rechit in chamber {tracker_test_chamber} (mm)")
        cluster_prophit_axs[5].hist2d(
            track_intercept_y[single_hit_mask],
            ak.mean(digi_strip[single_hit_mask], axis=1),
            bins=100
        )
        cluster_prophit_axs[5].set_xlabel(f"Propagated y (mm)")
        cluster_prophit_axs[5].set_ylabel(f"Average strip")

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

            for eta in range(5):
                hits_axs[idirection][0].hist(
                    ak.flatten(rechits[rechits_eta==eta]),
                    bins=100, range=(-50, 300),
                    histtype="step", label=f"$\eta = {eta}$"
                )
                hits_axs[idirection][1].hist(
                    prophits[prophits_eta==eta],
                    bins=100, range=(-50, 300),
                    histtype="step", label=f"$\eta = {eta}$"
                )
                hits_axs[idirection][0].set_xlabel(f"Rechits {direction} (mm)")
                hits_axs[idirection][1].set_xlabel(f"Prophits {direction} (mm)")
            for ax in hits_axs[idirection]: ax.legend()
            
            single_hit_mask = ak.count(rechits, axis=1)==1
            prophits = prophits[single_hit_mask]
            rechits = ak.flatten(rechits[single_hit_mask])
            rechits_eta_direction = ak.flatten(rechits_eta[single_hit_mask])
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

            rechit_prophit_axs[idirection][0].hist2d(
                prophits[rechits_eta_direction==2],
                rechits[rechits_eta_direction==2],
                bins=100
            )
            rechit_prophit_axs[idirection][0].set_xlabel(f"Propagated {direction} (mm)")
            rechit_prophit_axs[idirection][0].set_ylabel(f"Rechit {direction} (mm)")

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

            rechit_prophit_axs[idirection][1].hist2d(
                [prophits_x,prophits_y][idirection_other][single_hit_mask], rechits, bins=100
            )
            rechit_prophit_axs[idirection][1].set_xlabel(f"Propagated {direction_other} (mm)")
            rechit_prophit_axs[idirection][1].set_ylabel(f"Rechit {direction} (mm)")

        hits_fig.tight_layout()
        hits_fig.savefig(args.odir/"hits.png")

        residual_fig.tight_layout()
        residual_fig.savefig(args.odir/"residuals.png")

        residual_rechit_fig.tight_layout()
        residual_rechit_fig.savefig(args.odir/"residuals_rechits.png")

        rechit_prophit_fig.tight_layout()
        rechit_prophit_fig.savefig(args.odir/"prophits_rechits.png")

        cluster_prophit_fig.tight_layout()
        cluster_prophit_fig.savefig(args.odir/"prophits_cluster.png")

        residual_prophit_fig.tight_layout()
        residual_prophit_fig.savefig(args.odir/"residuals_prophits.png")

if __name__=='__main__': main()
