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
    
    def __init__(self, chamber, direction, strip):
        self.chamber, self.direction, self.strip = chamber, direction, strip

    def __repr__(self):
        return f"Digi: {self.chamber},{self.xy},{self.strip}"

    @property
    def xy(self):
        return ["x", "y"][self.direction]

class Cluster:

    def __init__(self, chamber, direction, center, first, size):
        self.chamber, self.direction = chamber, direction
        self.center, self.first, self.size = center, int(first), int(size)

    def __repr__(self):
        return f"Cluster: {self.chamber},{self.xy},{self.center},{self.size}"
    
    @property
    def xy(self):
        return ["x", "y"][self.direction]


    def from_digis(digis):
        # divide digis in list by chamber
        digi_map = dict()
        for digi in digis:
            digi_map.setdefault(digi.chamber, dict())
            digi_map[digi.chamber].setdefault(digi.direction, list())
            digi_map[digi.chamber][digi.direction].append(digi)

        clusters = list()
        for chamber in digi_map:
            for direction in digi_map[chamber]:
                digi_list = sorted(digi_map[chamber][direction], key=lambda digi:digi.strip)
                print(digi_list)
                cluster_first = 0
                while cluster_first < len(digi_list):
                    cluster_size = 0
                    while cluster_first+cluster_size < len(digi_list) and digi_list[cluster_first+cluster_size].strip == digi_list[cluster_first].strip+cluster_size:
                        cluster_size += 1
                    center = digi_list[cluster_first].strip+0.5*(cluster_size-1)
                    clusters.append(Cluster(chamber, direction, center, digi_list[cluster_first].strip, cluster_size))
                    cluster_first += cluster_size
        print(clusters)
        return clusters

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

    h_digi = list()
    """for chamber in mapping_vfats["chamber"].unique():
        h_digi.setdefault(chamber, dict())
        for direction in [1,2]:
            h_digi[chamber][direction] = rt.TH1D(f"h_digis_chamber{chamber}_direction{direction}", ";strip;counts", 358, 0, 358)"""
    
    for chamber in range(4):
        h_digi.append(list())
        for direction in range(2):
            direction_str = ["x", "y"][direction]
            h_digi[chamber].append(rt.TH1D(f"h_digis_chamber{chamber}_direction{direction_str}", ";strip;counts", 358, 0, 358))

    """for eta in mapping["iEta"].unique():
        h_digi[eta] = rt.TH1D(f"h_digis_eta{eta}", ";strip;counts", 358, 0, 358)"""

    rechit_tree = rt.TTree("rechitTree", "tree of rechits")

    n_clusters = np.zeros(1, dtype="int")
    chamber = rt.std.vector("int")()
    direction = rt.std.vector("int")()
    cluster_center = rt.std.vector("float")()
    cluster_first = rt.std.vector("int")()
    cluster_size = rt.std.vector("int")()
    """rechit_x = np.zeros(1, dtype="float")
    rechit_y = np.zeros(1, dtype="float")"""

    rechit_tree.Branch("n_clusters", n_clusters, "n_clusters/I")
    rechit_tree.Branch("chamber", chamber, "chamber/I")
    rechit_tree.Branch("direction", direction, "direction/I")
    rechit_tree.Branch("cluster_center", cluster_center, "cluster_center/D")
    rechit_tree.Branch("cluster_first", cluster_first, "cluster_first/D")
    rechit_tree.Branch("cluster_size", cluster_size, "cluster_size/D")
    """rechit_tree.Branch("rechit_x", rechit_x)
    rechit_tree.Branch("rechit_y", rechit_y)"""

    entries = run_tree.GetEntries()
    skipped_counter = 0
    try:
        for i,event in enumerate(tqdm(run_tree, total=entries)):
            print(f"Event {i}")
            if verbose:
                print("nhits", event.nhits)
                print("OH", event.OH, len(event.OH))
                print("VFAT", event.VFAT, len(event.VFAT))
                print("CH", event.CH, len(event.CH))
            
            digis_in_event = list()
            for i_hit in range(event.nhits):
                if event.OH[i_hit]==0: continue # only trackers for now
                #hit = Hit(event.OH[i_hit], event.VFAT[i_hit], event.CH[i_hit])
                digi = Digi(event.digiChamber[i_hit], event.digiDirection[i_hit], event.digiStrip[i_hit])
                digis_in_event.append(digi)
                """try: digi = hit.to_digi()
                except KeyError:
                    skipped_counter += 1
                    continue"""
                #if digi.direction=="X": direction = 1
                #else: direction = 2
                
                #h_digi[digi.chamber][digi.direction].Fill(float(digi.strip))
                h_digi[digi.chamber][digi.direction].Fill(float(digi.strip))

                """chamber[0] = digi.chamber
                direction[0] = digi.direction"""
                """eta[0] = digi.eta
                strip[0] = digi.strip"""
                #digi_tree.Fill()
            
            clusters = Cluster.from_digis(digis_in_event)
            cluster_center.clear()
            cluster_first.clear()
            cluster_size.clear()
            for cluster in clusters:
                chamber.push_back(int(cluster.chamber))
                chamber.push_back(cluster.direction)
                cluster_center.push_back(cluster.center)
                cluster_first.push_back(cluster.first)
                cluster_size.push_back(cluster.size)
            n_clusters[0] = len(clusters)

            rechit_tree.Fill()

            #if i % 10000 == 0: print(f"{i}, skipped {skipped_counter}")
    except KeyboardInterrupt: pass

    for chamber in range(4):
        for direction in range(2):
            direction_str = ["x", "y"][direction]
            c = rt.TCanvas(f"c_{chamber}_{direction_str}", "", 800, 600)
            h_digi[chamber][direction].Draw()
            c.SaveAs(f"{outdir}/chamber_{chamber}_{direction_str}.eps")
    
    print("Finished plotting.")
    print("Total skipped hits:", skipped_counter)
    
    #digi_tree.Write("", rt.TObject.kOverwrite)
    fin.Close()
    fout.Write()
    fout.Close()

if __name__=='__main__': main()
