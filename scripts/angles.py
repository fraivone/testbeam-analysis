"""
Plot angular corrections vs iteration
"""

import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import pandas as pd

import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("idirs", type=pathlib.Path, nargs="+", help="Input directory")
    parser.add_argument("odir", type=pathlib.Path, help="Output directory")
    args = parser.parse_args()
    
    os.makedirs(args.odir, exist_ok=True)

    angles_dfs = [
        pd.read_csv(os.path.join(idir, "angles.csv"), sep=" ", index_col=0).T
        for idir in args.idirs    
    ]
    n_steps = len(angles_dfs)
    iterations = np.arange(n_steps).astype(int)
    angles = 1e3*np.array([ list(df["angle"]) for df in angles_dfs ]).T
    corrections = np.diff(angles, axis=1, prepend=0)

    fig, ax = plt.figure(figsize=(12, 10)), plt.axes()
    for chamber in range(angles.shape[0]):
        correction = angles[chamber]-angles[chamber][-1]
        ax.plot(iterations, corrections[chamber], "-o", label=f"BARI-0{chamber+1}")
    ax.set_xticks(iterations)
    ax.set_xlabel("Iteration")
    ax.set_ylabel("Incremental correction (mrad)")
    ax.legend()
    fig.savefig(os.path.join(args.odir, "angles.png"))

if __name__=="__main__": main()
