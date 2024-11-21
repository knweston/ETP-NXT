#!/bin/bash

if [ "$#" -lt 5 ]; then
    echo "Illegal number of parameters"
    echo "Usage: ./run_champsim.sh [BINARY] [N_WARM] [N_SIM] [TRACE] [MODE]"
    exit 1
fi

# TRACE_DIR=$PWD/../../dpc3-traces
BINARY=${1}
N_WARM=${2}
N_SIM=${3}
TRACE=${4}
MODE=0

pr="PROFILE"
nr="NORMAL"
ex="EXPLOIT"

if [ ${5} == $pr ] ; then
    MODE=1
elif [ ${5} == $ex ] ; then
    MODE=2
else
    MODE=0
fi

# Sanity check
re='^[0-9]+$'
if ! [[ $N_WARM =~ $re ]] || [ -z $N_WARM ] ; then
    echo "[ERROR]: Number of warmup instructions is NOT a number" >&2;
    exit 1
fi

re='^[0-9]+$'
if ! [[ $N_SIM =~ $re ]] || [ -z $N_SIM ] ; then
    echo "[ERROR]: Number of simulation instructions is NOT a number" >&2;
    exit 1
fi

if [ ! -f "$TRACE_DIR/$TRACE" ] ; then
    echo "[ERROR] Cannot find a trace file: $TRACE_DIR/$TRACE"
    exit 1
fi

IFS='/'
read -ra TRACE_TOKENS <<< "$TRACE"
TRACE_NAME="${TRACE_TOKENS[@]: -1}"
IFS=''

mkdir -p results_${N_SIM}M
# (./bin/${BINARY} -warmup_instructions ${N_WARM}000000 -simulation_instructions ${N_SIM}000000 -execution_mode ${MODE} -traces ${TRACE_DIR}/${TRACE}) &> results_${N_SIM}M/${TRACE:0:25}-${BINARY}-${5}.txt
(${BINARY} -warmup_instructions ${N_WARM}000000 -simulation_instructions ${N_SIM}000000 -execution_mode ${MODE} -traces ${TRACE}) &> results_${N_SIM}M/$TRACE_NAME.txt