#!/bin/bash

# mkdir -p $PWD/../../../dpc3-traces
# mkdir -p $PWD/../../../google-traces
while read LINE
do
    # wget -P $PWD/../../../dpc3-traces -c http://hpca23.cse.tamu.edu/champsim-traces/speccpu/$LINE
    # wget -P $PWD/../../../dpc3-traces -c https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu/$LINE
    # wget -P $PWD/../../../dpc3-traces -c http://nuez.cse.tamu.edu/champsim-traces/GAPS-and-XSBench-traces/$LINE
    # wget -P $PWD/../../../google-traces -c $LINE
    wget -P $PWD/../../spec-traces -c https://dpc3.compas.cs.stonybrook.edu/champsim-traces/speccpu/$LINE
done < dpc3-more-traces.txt
# done < dpc3_max_simpoint.txt
# done < gap_and_xsbench_traces.txt
# done < google_traces.txt
