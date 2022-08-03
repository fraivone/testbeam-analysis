import ROOT
from argparse import RawTextHelpFormatter
import argparse
import os
import pathlib
import uproot
import numpy as np
import awkward as ak
import matplotlib.pyplot as plt
import time
import shutil


def generateClopperPeasrsonInterval(num,den):
    confidenceLevel = 0.68
    alpha = 1 - confidenceLevel
    
    lowerLimit = round(ROOT.Math.beta_quantile(alpha/2,num,den-num + 1),4)
    if num==den:
        upperLimit=1
    else:
        upperLimit = round(ROOT.Math.beta_quantile(1-alpha/2,num + 1,den-num),4)
    return lowerLimit,upperLimit


ROOT.gROOT.SetBatch(True)

parser = argparse.ArgumentParser(
    description='''Scripts that: \n\t-Parses the output of https://github.com/antonellopellecchia/testbeam-analysis/blob/feature/may2022/unpacker.cc\n\t-For each VFATs, plots number of hits vs  latency ''',
    epilog="""Typical exectuion\n\t python3 latency_analyzer.py  ./inoutfile.root  """,
    formatter_class=RawTextHelpFormatter
)

parser.add_argument("ifile", type=pathlib.Path, help="Input file")
parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")

args = parser.parse_args()
inputfile=str(args.ifile)
timestamp = inputfile.split(".root")[-2].split("/")[-1] if "run" in inputfile else time.strftime("%-y%m%d_%H%M")


def main():
    
    with uproot.open(args.ifile) as _file:
        tree = _file["outputtree"]
        
        nhits = tree["nhits"].array(entry_stop=args.events)
        latencies = tree["latency"].array(entry_stop=args.events)
        pulse_stretches = tree["pulse_stretch"].array(entry_stop=args.events)
        slots = tree["slot"].array(entry_stop=args.events)
        VFATs =  tree["VFAT"].array(entry_stop=args.events)
        OHs =  tree["OH"].array(entry_stop=args.events)
        CHs = tree["CH"].array(entry_stop=args.events)
        Chambers = tree["digiChamber"].array(entry_stop=args.events)
        etas = tree["digiEta"].array(entry_stop=args.events)
        strips = tree["digiStrip"].array(entry_stop=args.events)

        n_triggers = len(latencies)
        num = 0

        for evt_num,lat in enumerate(latencies):
            if len(VFATs[evt_num])!=0:
                print(f"ETAs in the evt: {etas[evt_num]} VFAT in the evt:{VFATs[evt_num]}\n\tStrips in the evt:{strips[evt_num]}")
                input()
            if 0 in VFATs[evt_num] or 2 in VFATs[evt_num]:
                num += 1

    print(f"{num}/{n_triggers} = {generateClopperPeasrsonInterval(num,n_triggers)}")

        # unique_latencies = np.unique(latencies)
        # unique_slots = np.unique(ak.flatten(slots))
        # unique_Chambers = np.unique(ak.flatten(Chambers))
        # unique_OHs = np.unique(ak.flatten(OHs))
        # unique_etas = np.unique(ak.flatten(etas))
        # unique_VFATs = np.unique(ak.flatten(VFATs))
        # unique_strips = np.unique(ak.flatten(strips))

        

    

            # #Plotting latency with pyplot
            # slot=7
            # for oh in unique_OHs:
            #     nvfats = len(unique_VFATs)
            #     nrows = 3
            #     ncols = int(np.ceil(nvfats/nrows))
            #     latency_fig, latency_axs = plt.subplots(nrows, ncols, figsize=(9*ncols, 9*nrows))
            #     latency_axs = latency_axs.flat
            #     plt.figure(figsize=(20,20))

            #     for idx,vf in enumerate(unique_VFATs):
            #         x_plot = []
            #         y_plot = []

            #         for lat in unique_latencies:
            #             mask = (latencies==lat)&(VFATs==vf)
            #             vfat_hits = len(ak.flatten(strips[mask]))
            #             x_plot.append(lat)
            #             y_plot.append(vfat_hits)
            #         print(f"OH{oh} VFAT{vf} x = {x_plot} y = {y_plot}")
            #         latency_axs[idx].hist(x_plot,bins=len(x_plot),weights=y_plot,color="black",alpha = 0.5)
            #         latency_axs[idx].set_title("OH{} VFAT{}".format(oh,vf),fontweight="bold", size=30)
            #         latency_axs[idx].set_xlabel("latency",fontsize=20)
            #         latency_axs[idx].set_ylabel("hits",fontsize=20)
            #         latency_fig.tight_layout()
            #         latency_fig.savefig(str(args.odir+f"/OH{oh}_latency_"+timestamp+".pdf")

            # print(f"Save output in {args.odir}")

                


if __name__=='__main__': main()
