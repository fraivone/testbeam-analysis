#!/bin/sh

# Reconstruct run from raw to tracks

run=$1
geometry=$2
events=$3

echo "##### unpacker #####"
./RawToDigi $EOS_TESTBEAM/raw/$run-*.raw $EOS_TESTBEAM/digi/$run.root --geometry $2 # --events $3
echo "##### rechits #####"
./DigiToRechits $EOS_TESTBEAM/digi/$run.root $EOS_TESTBEAM/rechits/$run.root
echo "##### tracks #####"
./Tracking $EOS_TESTBEAM/rechits/$run.root $EOS_TESTBEAM/tracks/$run.root
#--angles .00102455514842 -.00105457956694 -.00002029043069 .00080694014877
echo "##### analysis: residuals #####"
#python3 analysis/residuals.py $EOS_TESTBEAM/standalone/tracks/$run.root $EOS_WWW/gem/testbeam/analysis/residuals/$run
python3 analysis/ge21.py $EOS_TESTBEAM/tracks/$run.root $EOS_TESTBEAM/analysis/$run/ge21
