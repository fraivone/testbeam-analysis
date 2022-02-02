#!/bin/bash

# Run an efficiency HV scan with multiple runs
# TODO: Support rate scan

#runs=(run_20211102_1056 run_20211102_1210 run_20211102_1349 run_20211102_1609) # tracker hv scan
#runs=(run_20211030_2213 run_20211030_2317 run_20211031_0016 run_20211031_0213 run_20211031_0222 run_20211031_0321 run_20211031_0503 run_20211031_0606 run_20211031_0707) # ge21 hv scan
#runs=(run_20211102_1056 run_20211030_2213) # missing runs
runs=(run_20211030_2054) # runs to be repeated

for run in ${runs[@]}; do
    echo "####################"
    echo "## Analyzing run $run"
    echo "# Unpacker"
    ./RawToDigi $EOS_TESTBEAM/raw/$run-0-0.raw ferol $EOS_TESTBEAM/standalone/digi/$run.root
    echo "# Local reconstruction"
    ./DigiToRechits $EOS_TESTBEAM/standalone/digi/$run.root $EOS_TESTBEAM/standalone/rechits/$run.root
    echo "# Tracking"
    ./Tracking $EOS_TESTBEAM/standalone/rechits/$run.root $EOS_TESTBEAM/standalone/tracks/$run/
    echo "####################"
    echo ""
    echo ""
done