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
timestamp = time.strftime("%-y%m%d_%H%M")



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

    eta_to_vfat = {}
    for oh in unique_OHs:
        eta_to_vfat.setdefault(oh,{})
        for eta in unique_etas:
            mask = (OHs==oh)&(etas==eta)
            vfat_in_eta = VFATs[mask]
            eta_to_vfat[oh][eta] = list(np.unique(ak.flatten(vfat_in_eta)))



    #Plotting latency with ROOT
    TStack_eta_dict = {}
    occupancy_dict = {}
    color = [ROOT.TColor.GetColorPalette(50*i) for i in range(4)]

    infile=ROOT.TFile(str(args.ifile),"READ")
    tree = infile.Get("outputtree")
    nvfats = len(unique_VFATs)
    nrows = 3
    ncols = int(np.ceil(nvfats/nrows))
    c1 = ROOT.TCanvas("c1","c1",2000,2000)
    c2 = ROOT.TCanvas("occupancy","occupancy",2000,2000)
    c1.Divide(nrows,ncols)
    c2.Divide(1,len(unique_etas))
    
    for oh in unique_OHs:                    
        for idx,eta in enumerate(unique_etas):
            TStack_eta_dict[eta] = ROOT.THStack("Occu eta"+str(eta),"Occu eta"+str(eta))
            for j,vfat in enumerate(eta_to_vfat[oh][eta]):
                hist_occ_name = "occupancy_OH"+str(oh)+"_vfat"+str(vfat)
                occupancy_dict[vfat]= ROOT.TH1F(hist_occ_name, hist_occ_name,400,0,400)
                occupancy_dict[vfat].SetLineColor(color[j])
                occupancy_dict[vfat].SetFillColor(color[j])

                tree.Draw(f"digiStrip >> {hist_occ_name}","VFAT=="+str(vfat),"goff") 
                TStack_eta_dict[eta].Add(occupancy_dict[vfat])


            c2.cd(idx+1)
            TStack_eta_dict[eta].Draw("nostack")
            c2.cd(idx+1).BuildLegend(0.3,0.21,0.3,0.21,"","F")

    c2.Modified()
    c2.Update()
    c2.SaveAs(str(args.odir)+f"/OH{oh}_occ_"+timestamp+".png")
    c2.SaveAs(str(args.odir)+f"/OH{oh}_occ_"+timestamp+".pdf")

if __name__=='__main__': main()
