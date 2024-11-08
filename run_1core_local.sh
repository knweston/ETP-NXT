#!/bin/bash
if [ "$#" -lt 4 ]; then
    echo "Illegal number of parameters"
    echo "Usage: ./run_champsim.sh [BINARY] [N_WARM] [N_SIM] [CONFIG] [TRACE] [OUTPUT_FOLDER] [OUTPUT_FILE]"
    exit 1
fi

BINARY=${1}
N_WARM=${2}
N_SIM=${3}
CONFIG=${4}
TRACE=${5}
OUTPUT_FOLDER=${6}
OUTPUT_FILE=${7}

# Sanity check
if [ ! -f "$BINARY" ] ; then
    echo "[ERROR] Cannot find a ChampSim binary: $BINARY"
    exit 1
fi

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

if [ ! -f "$TRACE" ] ; then
    echo "[ERROR] Cannot find a trace file: $TRACE"
    exit 1
fi

mkdir -p results_${N_SIM}M_${N_WARM}M/${OUTPUT_FOLDER}
(${BINARY} -warmup_instructions ${N_WARM}000000 -simulation_instructions ${N_SIM}000000 -config ${CONFIG} -traces ${TRACE}) >> results_${N_SIM}M_${N_WARM}M/${OUTPUT_FOLDER}/${OUTPUT_FILE}.txt