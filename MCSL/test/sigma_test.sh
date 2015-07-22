#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TRAFFIC_DIR="${DIR}/../MCSL_NoC_Traffic_Pattern_Suite_v1.6/traffic"
OUTPUT_DIR="${DIR}/xml_in"
source ${DIR}/../../scripts/define.sh
MCSL="${MCSL_PATH}/MCSL"

APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp H264-720p_dec H264-1080p_dec"
NETWORK_SIZE=16


for a in ${APPLICATIONS} ; do
	for topo in torus mesh ; do # turus mesh
		for size in 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do
			for si in 1 1.2 1.5 2 3 5 10 20 30 50 100 200 300 500 ; do
				mkdir -p ${OUTPUT_DIR}/${a}/${topo}/${topo}_${size}x${size} 2>/dev/null
				${MCSL} -f "${TRAFFIC_DIR}/${topo}/${topo}_${size}x${size}/${a}_${topo}_${size}x${size}.stp" \
				-o "${OUTPUT_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${si}.xml" -s "${si}"
			done
		done
	done
done
