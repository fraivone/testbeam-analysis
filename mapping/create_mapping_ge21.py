import os, sys

import pandas as pd

mapping_df = pd.read_csv("mapping/ge21_mapping_licciully.csv", sep="\t")

mapping_dict = {
    "vfatId": list(),
    "vfatCh": list(),
    "iEta": list(),
    "strip": list()
}
vfats = list(mapping_df.columns)[1:]
for i_vfat,vfat in enumerate(vfats):
    vfat_column = mapping_df[vfat]
    for i_row,channel_row in enumerate(vfat_column):
        channel = mapping_df.iloc[i_row].Channel.replace("IN", "")
        cell_stripped = vfat_column.iloc[i_row].replace("E", "").split("S")
        mapping_dict["vfatId"].append(vfat)
        mapping_dict["vfatCh"].append(channel)
        mapping_dict["iEta"].append(cell_stripped[0])
        mapping_dict["strip"].append(cell_stripped[1])

mapped_df = pd.DataFrame(mapping_dict)
print(mapped_df)

mapped_df.to_csv("mapping/ge21_mapping.csv", sep=",")