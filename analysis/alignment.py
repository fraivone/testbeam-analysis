import os, sys

from tqdm import tqdm
import numpy as np
import pandas as pd
import uproot

import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)


def main():
    ifile, odir = sys.argv[1], sys.argv[2]
    os.makedirs(odir, exist_ok=True)

    if "-v" in sys.argv or "--verbose" in sys.argv:
        verbose = True
    else: verbose = False

    with uproot.open(ifile) as alignment_file:
        chambers = list(range(4))
        directions = ["X", "Y"]
        fig, axs = plt.subplots(2, figsize=(12,18))
        for i_dir,direction in enumerate(directions):
            for chamber in chambers:
                plot_name = f"alignment{direction}_chamber{chamber}"
                print(f"Plotting {plot_name}...")
                alignment_steps = alignment_file[plot_name].member("fX")
                alignment_corrections = alignment_file[plot_name].member("fY")
                axs[i_dir].plot(alignment_steps, alignment_corrections-alignment_corrections[-1], '-o', label=f"BARI-00{chamber+1}")
            axs[i_dir].set_title(f"{direction} direction")
            axs[i_dir].set_xlabel("Number of steps")
            axs[i_dir].set_ylabel("Correction (mm)")
            axs[i_dir].margins(0.1)
            axs[i_dir].legend()
        fig.savefig(f"{odir}/alignment.png")

if __name__=='__main__': main()
