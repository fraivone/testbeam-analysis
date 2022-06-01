#!/bin/sh

# Reconstruct run from raw to tracks

basedir=$1
run=$2
geometry=$3
events=$4

echo "Basedir $basedir, run $run, geometry $geometry, events $events"

echo "##### unpacker #####"
./RawToDigi $basedir/raw/$run-1-0.raw $basedir/digi/$run.root --geometry $geometry #--events $3
echo "##### rechits #####"
#./DigiToRechits $basedir/digi/$run.root $basedir/rechits/$run.root
echo "##### tracks #####"
#./Tracking $basedir/rechits/$run.root $basedir/tracks/$run.root
#--angles .00102455514842 -.00105457956694 -.00002029043069 .00080694014877
echo "##### analysis: residuals #####"
#python3 analysis/residuals.py $basedir/tracks/$run.root $basedir/results/residuals/$run
echo "##### analysis: occupancy #####"
python3 analysis/occupancy.py $basedir/digi/$run.root $basedir/results/$run/occupancy --verbose
