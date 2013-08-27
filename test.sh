#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DATA_DIR="${DIR}/data"
source ${DIR}/scripts/define.sh

for t in bitorus mesh ; do
	for s in 3 ; do # 3 4 5 6 7 8 9 10 15
		# Single shot solutions
		for m in GREEDY rGREEDY ; do # RANDOM BAD_RANDOM
			SNTsRun "-p ${DATA_DIR}/${t}${s}x${s}.xml -m ${m}"
		done 	

		for b in 0 0.01 0.02 0.05 0.1 0.2 0.3 0.5 0.9 1.0 ; do
			SNTsRun "-p ${DATA_DIR}/${t}${s}x${s}.xml -m GRASP -t $RUNFOR -b ${b}"
		done 

		for i in GREEDY rGREEDY RANDOM ; do # BAD_RANDOM
			SNTsRun "-p ${DATA_DIR}/${t}${s}x${s}.xml -m ALNS -t $RUNFOR -i ${i}"
		done 
	done 
done


