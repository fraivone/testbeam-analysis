import os, sys

import numpy as np
import pandas as pd

mapping_file = sys.argv[1]
"""mapping_vfats = pd.read_csv("mapping/tracker_mapping_vfat.csv", sep=";")
mapping_channels = pd.read_csv("mapping/tracker_mapping_channels_V2.csv", sep=";")
mapping = pd.read_csv("mapping/tracker_mapping.csv", sep=",")"""

mapping = pd.read_csv(mapping_file, sep=",")

print("Creating mapping from files...")

vfats_id = mapping.vfatId.unique()
vfat_channels = np.arange(128)

print(vfats_id)
print(vfat_channels)

# init all to 0s
to_chamber = [ [0 for vfat_id in vfats_id] for oh in range(4) ]
to_eta = [ 0 for vfat_id in vfats_id ]
to_strip = [ [0 for vfat_channel in vfat_channels] for vfat_id in vfats_id ]
print(len(to_strip), len(to_strip[0]))

for row in mapping.itertuples():
    vfat_id, vfat_channel, eta, strip = row.vfatId, row.vfatCh, row.iEta, row.strip
    direction = eta%2 # 0 for X, 1 for Y
    to_eta[vfat_id] = eta
    try:
        to_strip[vfat_id][vfat_channel] = strip
    except IndexError as e:
        print(vfat_id, vfat_channel, strip)
        raise e
    for oh in range(4):
        to_chamber[oh][vfat_id] = (oh-2)*2 + 1*(eta>2)

print(f"int to_chamber[{len(to_chamber)}][{len(to_chamber[0])}]")
print(to_chamber)

print(f"int to_strip[{len(to_strip)}][{len(to_strip[0])}]")
print(to_strip)

print(f"int to_eta[{len(to_eta)}]")
print(to_eta)