import os, sys

from tqdm import tqdm
import numpy as np
import pandas as pd
import ROOT as rt

mapping_vfats = pd.read_csv("mapping/tracker_mapping_vfat.csv", sep=";")
mapping_channels = pd.read_csv("mapping/tracker_mapping_channels_V2.csv", sep=";")
mapping = pd.read_csv("mapping/tracker_mapping.csv", sep=",")
print("Creating mapping from files...")

map_to_strip = dict()
map_to_chamber = dict()
for row in mapping.itertuples():
    vfat_id, vfat_channel, eta, strip = row.vfatId, row.vfatCh, row.iEta, row.strip
    map_to_strip.setdefault(vfat_id, dict())
    direction = eta%2 # 0 for X, 1 for Y
    map_to_strip[vfat_id][vfat_channel] = (eta, strip)
    for oh in [2, 3]:
        map_to_chamber.setdefault(oh, dict())
        map_to_chamber[oh][vfat_id] = (oh-2)*2 + 1*(eta>2)

"""to_chamber = {} # 1-4
#to_direction = {} # x or y
to_strip = {} # 0 to 358
for row in mapping_vfats.itertuples():
    oh, vfat, chamber, direction, sector = row.oh,row.vfat,row.chamber,row.direction,row.sector
    if direction=="X": direction = 1
    else: direction = 2
    to_chamber.setdefault(oh,{})
    to_chamber[oh][vfat]=(chamber,direction,sector)

for row in mapping_channels.itertuples():
    sector,channel,strip = row.sector,row.channel,row.strip
    to_strip.setdefault(sector,{})
    to_strip[sector][channel]=strip"""

"""
to_chamber = dict()
to_direction = dict()
for oh in mapping_vfats["oh"].unique():
    for vfat in mapping_vfats["vfat"].unique():
        series = mapping_vfats[(mapping_vfats["oh"]==oh)&(mapping_vfats["vfat"]==vfat)]
        to_chamber[(oh,vfat)] = series.iloc[0]["chamber"]
        to_direction[(oh,vfat)] = series.iloc[0]["direction"]

to_strip = dict()
for sector in mapping_channels["sector"].unique():
    for channel in mapping_channels["channel"].unique():
        series = mapping_channels[(mapping_channels["sector"]==sector)&(mapping_channels["channel"]==channel)]
        #print(sector, channel, series)
        try: to_strip[(sector,channel)] = series.iloc[0]["strip"]
        except IndexError: pass
print(to_chamber)
print(to_direction)
print(to_strip)
"""


class Hit:
    def __init__(self, oh, vfat_id, vfat_channel):
        self.oh, self.vfat_id, self.vfat_channel = oh, vfat_id, vfat_channel

    def to_digi(self):
        eta, strip = map_to_strip[self.vfat_id][self.vfat_channel]
        #strip = to_strip[sector][self.channel]
        return Digi(eta, strip)

class Digi:
    """def __init__(self, chamber, direction, strip):
        self.chamber, self.direction, self.strip = chamber, direction, strip"""
    
    def __init__(self, eta, strip):
        self.eta, self.strip = eta, strip

    def __repr__(self):
        return f"{self.eta}, {self.strip}"

def main():
    infile, outdir = sys.argv[1], sys.argv[2]
    os.makedirs(outdir, exist_ok=True)

    if "-v" in sys.argv or "--verbose" in sys.argv:
        verbose = True
    else: verbose = False

    fin = rt.TFile(infile)
    run_tree = fin.outputtree
    run_tree.Print()

    fout = rt.TFile(f"{outdir}/digi.root", "RECREATE")

    h_digi = dict()
    """for chamber in mapping_vfats["chamber"].unique():
        h_digi.setdefault(chamber, dict())
        for direction in [1,2]:
            h_digi[chamber][direction] = rt.TH1D(f"h_digis_chamber{chamber}_direction{direction}", ";strip;counts", 358, 0, 358)"""
    
    for eta in mapping["iEta"].unique():
        h_digi[eta] = rt.TH1D(f"h_digis_eta{eta}", ";strip;counts", 358, 0, 358)

    h_latency = dict()
    for oh in range(4):
        h_latency[oh] = dict()
        for vfat in mapping["vfatId"].unique():
            h_latency[oh][vfat] = rt.TH1I(f"latency_oh{oh}_vfat{vfat}", ";latency;", 7, 40, 60)

    digi_tree = rt.TTree("digiTree", "tree of digis")

    chamber = np.zeros(1, dtype="float")
    strip = np.zeros(1, dtype="float")
    direction = np.zeros(1, dtype="float")
    eta = np.zeros(1, dtype="float")

    """digi_tree.Branch("chamber", chamber, 'chamber/D')
    digi_tree.Branch("direction", direction, 'direction/D')"""
    digi_tree.Branch("eta", eta, 'eta/D')
    digi_tree.Branch("strip", strip, 'strip/D')

    entries = run_tree.GetEntries()
    skipped_counter = 0
    try:
        for i,event in enumerate(tqdm(run_tree, total=entries)):
            if verbose:
                print("nhits", event.nhits)
                print("latency", event.latency)
                print("pulse_stretch", event.pulse_stretch)
                print("OH", event.OH, len(event.OH))
                print("VFAT", event.VFAT, len(event.VFAT))
                print("CH", event.CH, len(event.CH))
            
            for i_hit in range(event.nhits):
                hit = Hit(event.OH[i_hit], event.VFAT[i_hit], event.CH[i_hit])

                """try: digi = hit.to_digi()
                except KeyError:
                    skipped_counter += 1
                    continue"""
                #if digi.direction=="X": direction = 1
                #else: direction = 2

                h_latency[hit.oh][hit.vfat_id].Fill(event.latency)

                #h_digi[digi.chamber][digi.direction].Fill(float(digi.strip))
                #h_digi[digi.eta].Fill(float(digi.strip))

                """chamber[0] = digi.chamber
                direction[0] = digi.direction"""
                #eta[0] = digi.eta
                #strip[0] = digi.strip
                #digi_tree.Fill()

            #if i % 10000 == 0: print(f"{i}, skipped {skipped_counter}")
    except KeyboardInterrupt: pass

    for oh in range(4):
        for vfat in mapping["vfatId"].unique():
            c = rt.TCanvas(f"canvas_oh{oh}_vfat{vfat}", "", 800, 600)
            h_latency[oh][vfat].Draw()
            c.SaveAs(f"{outdir}/latency_oh{oh}_vfat{vfat}.eps")
        
    #digi_tree.Write("", rt.TObject.kOverwrite)
    fin.Close()
    fout.Write()
    fout.Close()

if __name__=='__main__': main()
