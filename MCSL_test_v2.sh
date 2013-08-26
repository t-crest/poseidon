#!/bin/bash

source ./srcipts/define.sh ./scripts
RUNFOR=7200
DATA_DIR="./MCSL/xml"


#for t in torus mesh ; do
for t in torus ; do
	for s in 4 6 12 16; do 
#		for APPLICATIONS in FFT-1024_complex Fpppp H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse; do
		for APPLICATIONS in RS-32_28_8_dec; do
			# Single shot solutions
#			for m in GREEDY rGREEDY; do
#				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m ${m}" &
#			done 	

#			for b in 0.01 0.02 0.1 0.2 ; do
#				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m GRASP -t $RUNFOR -b ${b}" &
#			done 

			for i in GREEDY rGREEDY ; do
				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m ALNS -t $RUNFOR -i ${i}" &
			done 
		done
	done 
done