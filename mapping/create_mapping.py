import os, sys

import pandas as pd

mapping_vfats = pd.read_csv("mapping/tracker_mapping_vfat.csv", sep=";")
mapping_channels = pd.read_csv("mapping/tracker_mapping_channels_V2.csv", sep=";")
mapping = pd.read_csv("mapping/tracker_mapping.csv", sep=",")
print("Creating mapping from files...")

# init all to 0s
to_chamber = [ [0 for vfat_id in range(12)] for oh in range(4) ]
to_eta = [ 0 for vfat_id in range(12) ]
to_strip = [ [0 for vfat_channel in range(128)] for vfat_id in range(12) ]

for row in mapping.itertuples():
    vfat_id, vfat_channel, eta, strip = row.vfatId, row.vfatCh, row.iEta, row.strip
    direction = eta%2 # 0 for X, 1 for Y
    to_eta[vfat_id] = eta
    to_strip[vfat_id][vfat_channel] = strip
    for oh in range(4):
        to_chamber[oh][vfat_id] = (oh-2)*2 + 1*(eta>2)

print(f"int to_chamber[{len(to_chamber)}][{len(to_chamber[0])}]")
print(to_chamber)

print(f"int to_strip[{len(to_strip)}][{len(to_strip[0])}]")
print(to_strip)

print(f"int to_eta[{len(to_eta)}]")
print(to_eta)