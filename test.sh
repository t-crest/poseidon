#!/bin/bash

cores=32
runfor=7200
prog=metanoc
full_prog="metanoc/dist/Gramme/GNU-Linux-x86/${prog}"

function run {
#	echo "Queing $full_prog $@"
	sleep $(( $RANDOM % 15 )).$(( $RANDOM % 1000 ))

	while [ $(pgrep metanoc | wc -l) -ge $cores ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $full_prog $@"	
	$full_prog $@
}



for t in bitorus mesh ; do
	for s in 3 4 5 6 7 8 9 10 15 ; do 
		# Single shot solutions
		for m in GREEDY rGREEDY RANDOM BAD_RANDOM ; do
			run "-f ./data/${t}${s}x${s}.xml -m ${m}" &
		done 	

		for b in 0 0.01 0.02 0.05 0.1 0.2 0.3 0.5 0.9 1.0 ; do
			run "-f ./data/${t}${s}x${s}.xml -m GRASP -t $runfor -b ${b}" &
		done 

		for i in GREEDY rGREEDY RANDOM BAD_RANDOM ; do
			run "-f ./data/${t}${s}x${s}.xml -m ALNS -t $runfor -i ${i}" &
		done 
	done 
done


