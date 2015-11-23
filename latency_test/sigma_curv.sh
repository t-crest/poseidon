#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TRAFFIC_DIR="${DIR}/../MCSL/MCSL_NoC_Traffic_Pattern_Suite_v1.6/traffic"
OUTPUT_DIR="${DIR}/xml_in"
SCHED_DIR="${DIR}/sched"
source ${DIR}/../scripts/define.sh
MCSL="${MCSL_PATH}/MCSL"

APPLICATIONS="FFT-1024_complex Fpppp H264-1080p_dec H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse"
SIGMAS="1 1.5 2 3 5 10 20 25 30 40 50 75 100 200 250 300 500"
#APPLICATIONS="Sparse"
#SIGMAS="1"

topo="torus"
size=4

CORES=4

mkdir -p ${OUTPUT_DIR}/${topo}_${size}x${size} 2>/dev/null
mkdir -p ${SCHED_DIR}/${topo}_${size}x${size} 2>/dev/null

function loadBalance {
	while [ $(pgrep poseidon | wc -l) -ge $CORES ] ; do 
		printf "\n[Script: ${BASH_SOURCE[0]}] $(pgrep poseidon | wc -l) poseidon instances running\n"
		sleep 5.000
	done
}

for si in ${SIGMAS} ; do	
	for a in ${APPLICATIONS} ; do
		if [ ! -f "${OUTPUT_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml" ]; then
			${MCSL} -f "${TRAFFIC_DIR}/${topo}/${topo}_${size}x${size}/${a}_${topo}_${size}x${size}.stp" \
			-o "${OUTPUT_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml" -s "${si}"
		fi
		if [ ! -f "${SCHED_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml" ]; then
			loadBalance
			poseidon -p "${OUTPUT_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml" \
			-s "${SCHED_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml" -m GRASP -b 0.02 -t 200 -v 2 &
			#-m GRASP -b 0.02 -t 200 -a -v 2
		fi
	done
done

CORES=1
loadBalance

printf "\n[Script: ${BASH_SOURCE[0]}] All schedules generated\n"

for a in ${APPLICATIONS} ; do
	printf "Application ${a}\n"
	printf "sigma\tlength\tflows\tmin_WCTT\taverage_WCTT\tmax_WCTT\tmin_rate\taverage_rate\tmax_rate\n"
	for si in ${SIGMAS} ; do
		printf "${si}\t"
		./extract_latency.py "${SCHED_DIR}/${topo}_${size}x${size}/${a}_s_${si}.xml"
	done
done