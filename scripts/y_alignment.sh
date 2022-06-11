#!/bin/sh

ystep=0.5
y0=-214

for i in {0..16}; do
    python3 analysis/ge21.py /eos/cms/store/group/upgrade/GEM/TestBeams/Oct2021/runs/standalone/standalone/tracks/run_20211103_0304_tracker_leaves_vectors.root /eos/user/a/apellecc/www/gem/testbeam/analysis/ge21/run_20211103_0304_tracker_leaves_vectors --events 100000 --alignment -159.60592801232588 $y0 0
    y0=$( echo "$(printf "%.14f" $y0) + $(printf "%.14f" $ystep)" | bc )
    echo $y0
    #y0=$(( $y0+$ystep ))
    cat misalignment.csv
done
