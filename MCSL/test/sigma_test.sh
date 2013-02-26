#!/bin/bash
FULL_PROG="../bin/MCSL"
TRAFFIC_DIR="../MCSL_NoC_Benchmark_Suite_v1.5/traffic"
OUTPUT_DIR="./xml_in"
TORUS_APPLICATION="RS-32_28_8_dec"
NETWORK_SIZE=16

mkdir ${OUTPUT_DIR}/torus 2>/dev/null
for si in 1 1.1 1.2 1.3 1.5 2 3 4 5 10 20 30 50 100 200 300 500 ; do
#for si in 1 ; do
	mkdir ${OUTPUT_DIR}/torus/torus_16x16 2>/dev/null
	$FULL_PROG -f "${TRAFFIC_DIR}/torus/torus_${NETWORK_SIZE}x${NETWORK_SIZE}/${TORUS_APPLICATION}_torus_${NETWORK_SIZE}x${NETWORK_SIZE}.stp" -o "${OUTPUT_DIR}/torus/torus_${NETWORK_SIZE}x${NETWORK_SIZE}/${TORUS_APPLICATION}_s_${si}.xml" -s "${si}"
done