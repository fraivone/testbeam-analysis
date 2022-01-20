#!/bin/bash

run=$1
odir=$2
events=$3
steps=$4

angles=(0 0 0 0)
for i in {1..$steps}; do
    echo "############################################################"
    echo "Iteration $i"

    rechit_file=$EOS_TESTBEAM/standalone/rechits/$run.root
    track_dir=$EOS_TESTBEAM/standalone/angle_corrections/$run/iteration_$i
    mkdir -p $track_dir
    mkdir -p $odir/iteration_$i

    for jchamber in {0..3}; do
        echo "Iteration $i, chamber $jchamber"
        ./Tracking $rechit_file $track_dir --events $events --angles ${angles[@]}

        python3 scripts/residuals.py $track_dir/tracks.root $odir/iteration_$i --events 100000
        corrections=($(grep angle $odir/iteration_$i/angles.csv | sed "s/angle //g"))
    
        echo "Applying correction to chamber $jchamber..."
        echo "Old angles: ${angles[@]}"
        echo "Corrections: ${corrections[@]}"
        angles[jchamber]=$( echo "$(printf "%.14f" ${angles[jchamber]}) + $(printf "%.14f" ${corrections[jchamber]})" | bc )
        echo "New angles: ${angles[@]}"
    done

    echo "############################################################"
    echo ""
    echo ""
done