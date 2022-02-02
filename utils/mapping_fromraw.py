import os, sys, pathlib
import argparse
from tqdm import tqdm

import uproot
import numpy as np
import awkward as ak
import scipy
from scipy.optimize import curve_fit

import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import mplhep as hep
plt.style.use(hep.style.ROOT)

# enable multi-threading:
from concurrent.futures import ThreadPoolExecutor
executor = ThreadPoolExecutor()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input file")
    parser.add_argument("ofile", type=pathlib.Path, help="Output file")
    # parser.add_argument("detector", type=str, help="Detector under test")
    # parser.add_argument("bins", type=int, help="Number of bins")
    # parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")
    parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
    args = parser.parse_args()
    
    os.makedirs(os.path.basedir(args.odir), exist_ok=True)

if __name__=="__main__": main()
