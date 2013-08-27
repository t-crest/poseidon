#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ${DIR}/scripts/define.sh

DATA_DIR="${DIR}/MCSL/xml"
APPLICATIONS="FFT-1024_complex Fpppp H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse"


for t in torus mesh ; do
	for s in 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do
		for a in ${APPLICATIONS}; do
			for r in mean max_rate ; do
				# Single shot solutions
				for m in GREEDY rGREEDY; do
					SNTsRun "-p ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m ${m}"
				done 	

				for b in 0.01 0.02 0.1 0.2 ; do
					SNTsRun "-p ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m GRASP -t $RUNFOR -b ${b}"
				done 

				for i in GREEDY rGREEDY ; do
					SNTsRun "-p ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m ALNS -t $RUNFOR -i ${i}"
				done 
			done
		done
	done 
done