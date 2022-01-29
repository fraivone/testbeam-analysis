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
    coeff = [len(residuals), ak.mean(residuals), ak.std(residuals)]
    coeff += [len(residuals)*0.1, ak.mean(residuals), 10*ak.std(residuals)]
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
        tracks_slope_x = track_tree["tracks_X_slope"].array(entry_stop=args.events)
        tracks_slope_y = track_tree["tracks_Y_slope"].array(entry_stop=args.events)
        tracks_intercept_x = track_tree["tracks_X_intercept"].array(entry_stop=args.events)
        tracks_intercept_y = track_tree["tracks_Y_intercept"].array(entry_stop=args.events)
        
        impact_radius = np.sqrt(tracks_intercept_x**2+tracks_intercept_y**2)
        tracks_phi_x, tracks_phi_y = np.arctan(tracks_slope_x), np.arctan(tracks_slope_y)
        print("phi x rms", ak.std(tracks_phi_x)*1e3)
        print("phi y rms", ak.std(tracks_phi_y)*1e3)
        
        print("Plotting histogram...")
        angles_fig, angles_axs = plt.subplots(nrows=2, ncols=2, figsize=(25,20))
        img1 = angles_axs[0][0].hist2d(
            ak.flatten(impact_radius), abs(ak.flatten(tracks_phi_x))*1e3,
            bins=100, range=[[0.1, 80], [-5, 10]]
        )
        img2 = angles_axs[0][1].hist2d(
            ak.flatten(impact_radius), abs(ak.flatten(tracks_phi_y))*1e3,
            bins=100, range=[[0.1, 80], [-5, 10]]
        )
        angles_axs[1][0].hist((ak.flatten(tracks_phi_x))*1e3, range=(-10, 10), bins=100)
        angles_axs[1][1].hist((ak.flatten(tracks_phi_y))*1e3, range=(-10, 10), bins=100)

        angles_axs[1][0].text(
            0.6, 0.8,
            f"mean = {ak.mean(tracks_phi_x)*1e3:1.2f} mrad\n"+
            f"std = {ak.std(tracks_phi_x)*1e3:1.2f} mrad",
            transform=angles_axs[1][0].transAxes
        )
        angles_axs[1][1].text(
            0.6, 0.8,
            f"mean = {ak.mean(tracks_phi_y)*1e3:1.2f} mrad\n"+
            f"std = {ak.std(tracks_phi_y)*1e3:1.2f} mrad",
            transform=angles_axs[1][1].transAxes
        )

        angles_axs[0][1].set_xlabel("Impact point on GE2/1 from beam axis (mm)")
        angles_axs[0][0].set_xlabel("Impact point on GE2/1 from beam axis (mm)")
        angles_axs[0][0].set_ylabel("$\phi_x$ (mrad)")
        angles_axs[0][1].set_ylabel("$\phi_y$ (mrad)")
        angles_axs[1][0].set_xlabel("$\phi_x$ (mrad)")
        angles_axs[1][1].set_xlabel("$\phi_y$ (mrad)")
        angles_fig.savefig(os.path.join(args.odir, "track_angles.png"))

if __name__=='__main__': main()
