import os, sys

import pandas as pd

mapping_me0 = pd.read_csv("mapping/me0_mapping_hrs.csv", sep=",")
mapping_hrs = pd.read_csv("mapping/hrs_connector.csv", sep=",")

mapping_me0.vfatId -= 1

print(mapping_me0, "\n", mapping_hrs)

mapped_channels = list()
for row in mapping_me0.itertuples():
    try:
        hrs_row = mapping_hrs[mapping_hrs.hrsPin==row.hrsPin].iloc[0]
        mapped_channels.append(int(hrs_row.vfatCh))
    except IndexError as e:
        print(f"Missing mapping for {row.hrsPin}:\n", mapping_hrs[mapping_hrs.hrsPin==row.hrsPin])
        mapped_channels.append(0)

mapping_me0["vfatCh"] = mapped_channels
mapping_me0 = mapping_me0.astype(int)
print(mapping_me0)

mapping_me0.to_csv("mapping/me0_mapping.csv", sep=",")