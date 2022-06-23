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


ROOT.gROOT.SetBatch(True)

parser = argparse.ArgumentParser(
    description='''Scripts that: \n\t-Parses the output of https://github.com/antonellopellecchia/testbeam-analysis/blob/feature/may2022/unpacker.cc\n\t-For each VFATs, plots number of hits vs  latency ''',
    epilog="""Typical exectuion\n\t python3 latency_analyzer.py  ./inoutfile.root  outdir """,
    formatter_class=RawTextHelpFormatter
)

parser.add_argument("ifile", type=pathlib.Path, help="Input file")
parser.add_argument('odir', type=pathlib.Path, help="Output directory")
parser.add_argument("-v", "--verbose", action="store_true", help="Activate logging")
parser.add_argument("-n", "--events", type=int, default=-1, help="Number of events to analyse")

args = parser.parse_args()



def main():
    os.makedirs(args.odir, exist_ok=True)
    
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
        

        unique_latencies = np.unique(latencies)
        unique_slots = np.unique(ak.flatten(slots))
        unique_Chambers = np.unique(ak.flatten(Chambers))
        unique_OHs = np.unique(ak.flatten(OHs))
        unique_etas = np.unique(ak.flatten(etas))
        unique_VFATs = np.unique(ak.flatten(VFATs))
        unique_strips = np.unique(ak.flatten(strips))

        if(args.verbose):
            print(f"Found {unique_latencies} unique latencies")
            print(f"Found {unique_slots} unique slots")
            print(f"Found {unique_Chambers} unique chambers")
            print(f"Found {unique_OHs} unique OHs")
            print(f"Found {unique_etas} unique etas")
            print(f"Found {unique_VFATs} unique VFATs")
            print(f"Found {unique_strips} unique strips")
            print()


    histo_dict = {}
    infile=ROOT.TFile("/afs/cern.ch/user/f/fivone/Documents/Unpacker_2022/testbeam-analysis/build/bla.root","READ")
    tree = infile.Get("outputtree")
    nvfats = len(unique_VFATs)
    nrows = 3
    ncols = int(np.ceil(nvfats/nrows))
    c1 = ROOT.TCanvas("c1","c1",2000,2000)
    c1.Divide(nrows,ncols)
    timestamp = time.strftime("%-y%m%d_%H%M")

    for oh in unique_OHs:
        histo_dict = {}
        for idx,v in enumerate(unique_VFATs):
            histoname = "hist_VFAT"+str(v)
            histo_dict[v]=ROOT.TH1F(histoname,histoname,120,0,120)
            histo_dict[v].GetXaxis().SetTitle("Latency (BX)")
            histo_dict[v].GetYaxis().SetTitle("hits")
            c1.cd(idx+1)
            tree.Draw(f"latency >> {histoname}","VFAT=="+str(v))
        c1.Modified()
        c1.Update()
        c1.SaveAs(str(args.odir)+f"/OH{oh}_latency_"+timestamp+".png")
        c1.SaveAs(str(args.odir)+f"/OH{oh}_latency_"+timestamp+".pdf")

        ## Plotting latency with pyplot
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
        #         latency_fig.savefig(args.odir/f"{oh}_latency.png")

        #     print(f"Save output in {args.odir}")

                


if __name__=='__main__': main()
