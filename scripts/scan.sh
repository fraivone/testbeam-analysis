#!/bin/bash

# Run an efficiency HV scan with multiple runs
# TODO: Support rate scan

RUN_DIR=$1 # $EOS_TESTBEAM
STANDALONE_DIR=$2 # $EOS_TESTBEAM/standalone
ANALYSIS_DIR=$3 # $EOS_WWW/gem/testbeam/analysis

#runs=(run_20211102_1056 run_20211102_1210 run_20211102_1349 run_20211102_1609) # tracker hv scan
runs=(run_20211030_2213 run_20211030_2317 run_20211031_0016 run_20211031_0213 run_20211031_0222 run_20211031_0321 run_20211031_0503 run_20211031_0606 run_20211031_0707) # ge21 hv scan
#runs=(run_20211102_1056 run_20211030_2213) # missing runs
#runs=(run_20211030_2054) # runs to be repeated

for run in ${runs[@]}; do
    echo "####################"
    echo "## Analyzing run $run"
    echo "# Unpacker"
    ./RawToDigi $RUN_DIR/raw/$run-0-0.raw $STANDALONE_DIR/digi/$run.root
    echo "# Local reconstruction"
    ./DigiToRechits $STANDALONE_DIR/digi/$run.root $STANDALONE_DIR/rechits/$run.root
    echo "# Tracking"
    ./Tracking $STANDALONE_DIR/rechits/$run.root $STANDALONE_DIR/tracks/$run.root
    echo "# Analysis"
    python3 analysis/efficiency.py $STANDALONE_DIR/tracks/$run.root $ANALYSIS_DIR/$run/ ge21 100
    echo "####################"
    echo ""
    echo ""
done