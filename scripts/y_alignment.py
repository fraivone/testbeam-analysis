import os, sys

import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import mplhep as hep
plt.style.use(hep.style.ROOT)

import pandas as pd

misalignment_df = pd.read_csv("misalignment.csv", sep=";")
print(misalignment_df)

ofile = sys.argv[1]
fig, ax = plt.figure(figsize=(12,9)), plt.axes()
ax.plot(misalignment_df["y"], misalignment_df["misaligned"], ".")
ax.set_xlabel("y correction (mm)")
ax.set_ylabel("Events with misaligned eta partitions")
fig.savefig(ofile)
