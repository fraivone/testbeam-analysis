import os, sys, pathlib
import argparse
from tqdm import tqdm
import numpy as np
import pandas as pd

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("ifile", type=pathlib.Path, help="Input file")
    parser.add_argument("ofile", type=pathlib.Path, help="Output file")
    parser.add_argument("--hrsmapping", type=pathlib.Path, help="HRS to vfat channel mapping")
    args = parser.parse_args()
    
    os.makedirs(os.path.dirname(args.ofile), exist_ok=True)

    hrs_mapping = pd.read_csv(args.hrsmapping)

    vfat_cols = { # maps (vfat) -> (eta, mapping columns, row1)
        0: (8,"A:B","D:E", 1), 8: (8,"H:I","K:L", 1), 16: (8,"O:P","R:S", 1),
        1: (7,"A:B","D:E", 83), 9: (7,"H:I","K:L", 83), 17: (7,"O:P","R:S", 83),
        2: (6,"A:B","D:E", 165), 10: (6,"H:I","K:L", 165), 18: (6,"O:P","R:S", 165),
        3: (5,"A:B","D:E", 247), 11: (5,"H:I","K:L", 247), 19: (5,"O:P","R:S", 247),
        4: (4,"A:B","D:E", 329), 12: (4,"H:I","K:L", 329), 20: (4,"O:P","R:S", 329),
        5: (3,"A:B","D:E", 411), 13: (3,"H:I","K:L", 411), 21: (3,"O:P","R:S", 411),
        6: (2,"A:B","D:E", 493), 14: (2,"H:I","K:L", 493), 22: (2,"O:P","R:S", 493),
        7: (1,"A:B","D:E", 575), 15: (1,"H:I","K:L", 575), 23: (1,"O:P","R:S", 575),
    }
    nrows=76

    print("Parsing xls mapping file...")
    mappings = list()
    #for vfat_id in tqdm(range(vfat_number)):
    for vfat_id,vals in tqdm(vfat_cols.items()):
        eta, cols1, cols2, firstrow = vals
        #print(vfat_id, vals)
        for cols in [cols1, cols2]:
            mappings.append(
                pd.concat([
                    pd.read_excel(
                        args.ifile, engine="openpyxl", names=["strip", "hrsPin"],
                        usecols=cols, skiprows=firstrow, nrows=nrows
                    ).astype("Int64"),
                    pd.read_excel(
                        args.ifile, engine="openpyxl", names=["strip", "hrsPin"],
                        usecols=cols, skiprows=firstrow, nrows=nrows
                    ).astype("Int64")
                ])
            )
            #print(mappings[-1])
            mappings[-1]["iEta"] = eta
            mappings[-1]["vfatId"] = vfat_id
    
    mapping = pd.concat(mappings)
    mapping = mapping[~mapping["strip"].isna()]
    mapping.reset_index()

    print("Mapping hrs pin to vfat channel...")
    vfat_ch = list()
    for row in mapping.itertuples():
        #print("##########", i, "###########")
        # print("ROW:")
        # print(row.hrsPin)
        # print("MAPPING:")
        # print(hrs_mapping[hrs_mapping["hrsPin"]==row.hrsPin]["vfatCh"].iloc[0])
        vfat_ch.append(
            hrs_mapping[hrs_mapping["hrsPin"]==row.hrsPin]["vfatCh"].iloc[0]
        )
        #print(vfat_ch)
    mapping["vfatCh"] = vfat_ch
    
    print("Mapping output:")
    print(mapping)

    mapping.to_csv(args.ofile, sep=",", index=False)
    print("Output file written to", args.ofile)

if __name__=="__main__": main()