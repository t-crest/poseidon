#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DATA_DIR="${DIR}/../MCSL/test/xml_in"
source ${DIR}/../scripts/define.sh

APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp H264-720p_dec"

for a in ${APPLICATIONS} ; do
	for topo in mesh ; do # turus mesh
		for size in 7 8 ; do
			for s in 1 ; do
					# Single shot solutions
			#		for m in GREEDY rGREEDY; do
				#for m in GREEDY ; do
				#	PoseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m ${m}"
				#done

				for b in 0.02 0.2 ; do
				 	PoseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m GRASP -t $RUNFOR -b ${b}"
				done

				for i in GREEDY ; do #rGREEDY
				 	PoseidonRun "-p ${DATA_DIR}/${a}/${topo}/${topo}_${size}x${size}/${a}_s_${s}.xml -m ALNS -t $RUNFOR -i ${i}"
				done
			done
		done
	done
done
