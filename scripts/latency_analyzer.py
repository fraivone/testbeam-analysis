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
inputfile=str(args.ifile)
timestamp = inputfile.split(".root")[-2].split("/")[-1] if "run" in inputfile else time.strftime("%-y%m%d_%H%M")
outdir = args.odir / timestamp 


def main():
    os.makedirs(outdir, exist_ok=True)
    shutil.copyfile("/eos/user/f/fivone/www/index.php", outdir / "index.php")
    
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
        print(f"Found {n_triggers} events")
        print()



    #Plotting latency with ROOT
    latency_dict = {}
    latency2D_dict = {}

    infile=ROOT.TFile(str(args.ifile),"READ")
    tree = infile.Get("outputtree")
    nvfats = len(unique_VFATs)
    nrows = 3
    ncols = int(np.ceil(nvfats/nrows))
    c1 = ROOT.TCanvas("VFAT_latency","VFAT_latency",2000,2000)
    c1_2D = ROOT.TCanvas("strip_latency","strip_latency",2000,2000)
    c2 = ROOT.TCanvas("occupancy","occupancy",2000,2000)
    
    c1.Divide(nrows,ncols)
    c1_2D.Divide(1,len(unique_etas))

    c2.Divide(1,2)
    
    
    all_latency = ROOT.TH1F("lat_distr","lat_distr",max(unique_latencies)+1,0,max(unique_latencies))
    all_latency.GetXaxis().SetTitle("Latency (BX)")
    all_PS = ROOT.TH1F("PulseStr_dist","PulseStr_dist",10,0,10)
    all_PS.GetXaxis().SetTitle("PulseStretch (BX)")

    tree.Draw(f"latency >> lat_distr","","goff")
    tree.Draw(f"pulse_stretch >> PulseStr_dist","","goff")

    c2.cd(1)
    all_latency.Draw("HIST")
    c2.cd(2)
    all_PS.Draw("HIST")
    c2.Modified()
    c2.Update()
    
    c2.SaveAs(str(outdir)+f"/RunParamDistr_"+timestamp+".png")
    c2.SaveAs(str(outdir)+f"/RunParamDistr_"+timestamp+".pdf")



    for oh in unique_OHs:
        for idx,v in enumerate(unique_VFATs):
            hist_lat_name = "latScan_VFAT"+str(v)
            latency_dict[v]=ROOT.TH1F(hist_lat_name,hist_lat_name,max(unique_latencies)+1,0,max(unique_latencies))

            latency_dict[v].GetXaxis().SetTitle("Latency (BX)")
            latency_dict[v].GetYaxis().SetTitle("hits")
                    
            c1.cd(idx+1)
            tree.Draw(f"latency >> {hist_lat_name}","hitspervfat["+str(v)+"]>0","goff")
            latency_dict[v].Divide(all_latency) ## Normalize by number of triggers per latency 
            latency_dict[v].Draw("HIST")
        for idx,e in enumerate(unique_etas):

            hist2D_lat_name = "stripLatency_eta"+str(e)
            latency2D_dict[e]=ROOT.TH2F(hist2D_lat_name,hist2D_lat_name,256,128,384,max(unique_latencies),0,max(unique_latencies))

            c1_2D.cd(idx+1)
            tree.Draw(f"latency:digiStrip>>{hist2D_lat_name}","digiEta=="+str(e),"goff") 
            latency2D_dict[e].GetYaxis().SetTitle("Latency (BX)")
            latency2D_dict[e].GetXaxis().SetTitle("Strip")
            latency2D_dict[e].SetStats(0)
            latency2D_dict[e].Draw("COLZ")
                    

    c1.Modified()
    c1.Update()
    c1_2D.Modified()
    c1_2D.Update()
    c1.SaveAs(str(outdir)+f"/OH{oh}_latency_"+timestamp+".png")
    c1.SaveAs(str(outdir)+f"/OH{oh}_latency_"+timestamp+".pdf")

    c1_2D.SaveAs(str(outdir)+f"/OH{oh}_latency2D_"+timestamp+".png")
    c1_2D.SaveAs(str(outdir)+f"/OH{oh}_latency2D_"+timestamp+".pdf")


    

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
            #         latency_fig.savefig(str(args.odir)+f"/OH{oh}_latency_"+timestamp+".pdf")

            # print(f"Save output in {args.odir}")

                


if __name__=='__main__': main()
