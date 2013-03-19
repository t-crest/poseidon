#!/bin/bash
FULL_PROG="../bin/MCSL"
TRAFFIC_DIR="../MCSL_NoC_Benchmark_Suite_v1.5/traffic"
OUTPUT_DIR="./xml_in"
APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp H264-720p_dec"
NETWORK_SIZE=16


for a in ${APPLICATIONS} ; do
	for topo in torus ; do # turus mesh
		for size in 8 16 ; do
			for si in 1 1.2 1.5 2 3 5 10 20 30 50 100 200 300 500 ; do
				mkdir -p ${OUTPUT_DIR}/${a}/${topo}/${topo}_${size}x${size} 2>/dev/null
				$FULL_PROG -f "${TRAFFIC_DIR}/${topo}/${topo}_${size}x${size}/${a}_${topo}_${size}x${size}.stp" \
				-o "${OUTPUT_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${si}.xml" -s "${si}"
			done
		done
	done
done
