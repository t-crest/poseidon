#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DATA_DIR="${DIR}/xml_in"
source ${DIR}/../../scripts/define.sh

APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp H264-720p_dec"

for a in ${APPLICATIONS} ; do
	for topo in torus ; do # turus mesh
		for size in 8 16 ; do
			for s in 1 1.2 1.5 2 3 5 10 20 30 50 100 200 300 500 ; do
				# Single shot solutions
		#		for m in GREEDY rGREEDY; do
				for m in GREEDY ; do
					poseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m ${m}"
				done 	

				for b in 0.02 0.2 ; do
				 	poseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m GRASP -t $RUNFOR -b ${b}"
				done 

				for i in GREEDY ; do #rGREEDY
				 	poseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m ALNS -t $RUNFOR -i ${i}"
				done 
			done
		done
	done
done
