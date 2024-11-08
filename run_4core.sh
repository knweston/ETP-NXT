#!/bin/bash

if [ "$#" -lt 8 ] || [ "$#" -gt 12 ]; then
    echo "Illegal number of parameters"
    echo "Usage: ./run_4core.sh [BINARY] [N_WARM] [N_SIM] [N_MIX] [CONFIG] [TRACE0] [TRACE1] [TRACE2] [TRACE3] [RANDSEED_OFFSET] [RESULT_FOLDER] [OPTION]"
    exit 1
fi

BINARY=${1}
N_WARM=${2}
N_SIM=${3}
N_MIX=${4}
CONFIG=${5}
TRACE0=${6}
TRACE1=${7}
TRACE2=${8}
TRACE3=${9}
RANDSEED_OFFSET=${10}
RESULT_FOLDER=${11}
OPTION=${12}

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

if [ ! -f "$TRACE0" ] ; then
    echo "[ERROR] Cannot find a trace0 file: $TRACE0"
    exit 1
fi

if [ ! -f "$TRACE1" ] ; then
    echo "[ERROR] Cannot find a trace1 file: $TRACE1"
    exit 1
fi

if [ ! -f "$TRACE2" ] ; then
    echo "[ERROR] Cannot find a trace2 file: $TRACE2"
    exit 1
fi

if [ ! -f "$TRACE3" ] ; then
    echo "[ERROR] Cannot find a trace3 file: $TRACE3"
    exit 1
fi

mkdir -p results_4core_${N_SIM}M/${RESULT_FOLDER}
(${BINARY} -warmup_instructions ${N_WARM}000000 -simulation_instructions ${N_SIM}000000 -random_offset ${RANDSEED_OFFSET} ${OPTION} -config ${CONFIG} -traces ${TRACE0} ${TRACE1} ${TRACE2} ${TRACE3}) &>> results_4core_${N_SIM}M/${RESULT_FOLDER}/mix${N_MIX}.txt
