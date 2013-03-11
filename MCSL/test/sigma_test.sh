#!/bin/bash
FULL_PROG="../bin/MCSL"
TRAFFIC_DIR="../MCSL_NoC_Benchmark_Suite_v1.5/traffic"
OUTPUT_DIR="./xml_in"
APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp H264-720p_dec"
NETWORK_SIZE=16


for a in ${APPLICATIONS} ; do
	mkdir ${OUTPUT_DIR}/${a} 2>/dev/null
	mkdir ${OUTPUT_DIR}/${a}/torus 2>/dev/null
	for si in 1 1.1 1.2 1.3 1.5 2 3 4 5 10 20 30 50 100 200 300 500 ; do
		mkdir ${OUTPUT_DIR}/${a}/torus/torus_${NETWORK_SIZE}x${NETWORK_SIZE} 2>/dev/null
		$FULL_PROG -f "${TRAFFIC_DIR}/torus/torus_${NETWORK_SIZE}x${NETWORK_SIZE}/${a}_torus_${NETWORK_SIZE}x${NETWORK_SIZE}.stp" \
		-o "${OUTPUT_DIR}/${a}/torus/torus_${NETWORK_SIZE}x${NETWORK_SIZE}/${a}_s_${si}.xml" -s "${si}"
	done
done
