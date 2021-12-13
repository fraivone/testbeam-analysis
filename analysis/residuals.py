import os, sys

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

        prophits_x, prophits_y = track_tree["prophitX"].array(library="np"), track_tree["prophitY"].array(library="np")
        rechits_x, rechits_y = track_tree["rechitX"].array(library="np"), track_tree["rechitY"].array(library="np")
        residuals_x, residuals_y = prophits_x-rechits_x, prophits_y-rechits_y

        print("Saving residual map...")
        residual_fig, residual_axs = plt.subplots(nrows=2, ncols=1, figsize=(10,14))
        prophits = [prophits_x, prophits_y]
        residuals = [residuals_x, residuals_y]
        directions = ["x", "y"]

        space_resolutions = dict()
        spres_fig, spres_ax = plt.subplots(figsize=(10,7))
        residuals2d_fig, residuals2d_ax = plt.subplots(nrows=2, ncols=1, figsize=(10,12))
        cluster_sizes = list(range(1,10))
        for iplot in range(2):
            direction = directions[iplot]
            space_resolutions[direction] = list()
            if direction == "x": cluster_size = track_tree["rechitXClusterSize"].array(library="np")
            elif direction == "y": cluster_size = track_tree["rechitYClusterSize"].array(library="np")
            for cls in cluster_sizes:
            #for parity in [0, 1]:
                #even_or_odd = "even"*(parity==0) + "odd"*(parity==1)
                #data = residuals[iplot][cluster_size%2==parity]
                data = residuals[iplot][cluster_size==cls]

                points, bins = np.histogram(data, bins=30, range=(-1.5, 1.5))
                # points, bins, _ = plt.hist(data, bins=20, range=[-2, 2])
                bins = bins[:-1]+ 0.5*(bins[1:] - bins[:-1])
                
                # gaussian fit
                p0 = [len(data), data.mean(), data.std()]
                p0 += [len(data)*0.1, data.mean(), 10*data.std()]
                coeff, var_matrix = curve_fit(gauss2, bins, points, p0=p0)
                print(f"Before fit: {p0}. Fit results: {coeff}")
                space_resolution = 1e3*coeff[2]
                space_resolutions[direction].append(space_resolution)
                
                # plot data and fit
                residual_axs[iplot].scatter(bins, points, marker="o", label=f"size {cls} - {space_resolution:1.0f} µm")
                xvalues = np.linspace(bins[0], bins[-1], 1000)
                residual_axs[iplot].plot(xvalues, gauss2(xvalues, *coeff))
                residual_axs[iplot].set_xlabel(f"residuals {directions[iplot]} (mm)")
                # residual_axs[iplot].text(
                #     2, (1-0.1*parity)*1e6,
                #     f"Space resolution {space_resolution:1.0f} µm",
                #     horizontalalignment="right",
                #     fontsize=20
                # )
                residual_axs[iplot].legend()

            spres_ax.plot(cluster_sizes, space_resolutions[direction], marker="o", label=direction)
            
            residuals2d_ax[iplot].hist2d(prophits[iplot], residuals[iplot], bins=100, range=[[-40, 40],[-1, 1]])
            residuals2d_ax[iplot].set_title(f"Direction {direction}")
            residuals2d_ax[iplot].set_xlabel("Propagated position (mm)")
            residuals2d_ax[iplot].set_ylabel("Residual (mm)")

        # bins_x = (matched_bins_x + 0.5*(matched_bins_x[1]-matched_bins_x[0]))[:-1]
        # bins_y = (matched_bins_y + 0.5*(matched_bins_y[1]-matched_bins_y[0]))[:-1]
        # #plt.contourf(bins_x, bins_y, efficiency)
        # plt.imshow(efficiency, extent=eff_range[0]+eff_range[1], origin="lower")
        # plt.xlabel("x (mm)")
        # plt.ylabel("y (mm)")
        # plt.colorbar(label="Efficiency")
        # plt.tight_layout()
        # plt.text(eff_range[0][-1]-.5, eff_range[1][-1]+2, "GEM-10x10-380XY-BARI-04", horizontalalignment="right")
        residual_fig.tight_layout()
        residual_fig.savefig(f"{odir}/residuals.png")

        spres_ax.set_xlabel("Cluster size")            
        spres_ax.set_ylabel(f"Space resolution (µm)")
        spres_ax.legend()
        spres_fig.tight_layout()
        spres_fig.savefig(f"{odir}/space_resolution.png")

        residuals2d_fig.tight_layout()
        residuals2d_fig.savefig(f"{odir}/residuals2d.png")

if __name__=='__main__': main()
