#!/bin/bash

# Calculate angular corrections
# by iterating track reconstruction

run=$1
odir=$2
events=$3
steps=$4

rechit_file=$EOS_TESTBEAM/standalone/rechits/$run.root

angles=(0 0 0 0)
iteration=1
while [ "$iteration" -le "$steps" ]; do
    echo "############################################################"
    track_dir=$EOS_TESTBEAM/standalone/angle_corrections/$run/iteration_$iteration
    mkdir -p $odir/iteration_$iteration
    mkdir -p $track_dir

    for jchamber in {0..3}; do
        echo "Iteration $iteration, chamber $jchamber"
        track_file=$track_dir/chamber_$jchamber.root
        ./Tracking $rechit_file $track_file --events $events --angles ${angles[@]}

        python3 analysis/residuals.py $track_file $odir/iteration_$iteration
        corrections=($(grep angle $odir/iteration_$iteration/angles.txt | sed "s/angle //g"))
    
        echo "Applying correction to chamber $jchamber..."
        echo "Old angles: ${angles[@]}"
        echo "Corrections: ${corrections[@]}"
        angles[jchamber]=$( echo "$(printf "%.14f" ${angles[jchamber]}) + $(printf "%.14f" ${corrections[jchamber]})" | bc )
        echo "New angles: ${angles[@]}"
        echo "------------------------------------------------------------"
    done

    echo "############################################################"
    echo ""
    echo ""

    iteration=$(( $iteration+1 ))
done

echo "Done iterations"
python3 scripts/angles.py $odir/iteration_* $odir
echo "Final angles: ${angles[@]}"
echo ${angles[@]} > $odir/angles.txt