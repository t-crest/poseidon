#!/bin/bash
CORES=32
RUNFOR=10800
PROG=snts
FULL_PROG="dist/Gramme/GNU-Linux-x86/${prog}"
DATA_DIR="./MCSL/xml"


function run {
#	echo "Queing $FULL_PROG $@"
	sleep $(( $RANDOM % 15 )).$(( $RANDOM % 1000 ))

	while [ $(pgrep snts | wc -l) -ge $CORES ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $FULL_PROG $@"	
	$FULL_PROG $@
}



for t in torus mesh ; do
	for s in 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do 
		for APPLICATIONS in FTT-1024_complex Fpppp H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse; do
			# Single shot solutions
			for m in GREEDY rGREEDY; do
				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m ${m}" &
			done 	

			for b in 0.01 0.02 0.1 0.2 ; do
				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m GRASP -t $runfor -b ${b}" &
			done 

			for i in GREEDY rGREEDY ; do
				run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${APPLICATIONS}_${t}_${s}x${s}.stp.xml -m ALNS -t $runfor -i ${i}" &
			done 
		done
	done 
done