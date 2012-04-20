#!/bin/bash

cores=32
runfor=1800
prog=metanoc
full_prog="metanoc/dist/Gramme/GNU-Linux-x86/${prog}"

function run {
	echo "Queing $full_prog $@"
	sleep $(( $RANDOM % 10 ))

	while [ $(pgrep metanoc | wc -l) -ge $cores ] ; do 
		sleep $(( $RANDOM % 4 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $full_prog $@"	
	$full_prog $@
}


# Single shot solutions
for s in 3 8 15 ; do 
	for m in GREEDY rGREEDY RANDOM BAD_RANDOM ; do
		run "-f ./data/bitorus${s}x${s}.xml -m ${m}" &
	done 	
done 

for s in 3 8 15 ; do 
	for b in 0 0.01 0.02 0.05 0.1 0.2 0.3 0.5 0.9 1.0 ; do
		run "-f ./data/bitorus${s}x${s}.xml -m GRASP -t $runfor -b ${b}" &
	done 

	for i in GREEDY rGREEDY RANDOM BAD_RANDOM ; do
		run "-f ./data/bitorus${s}x${s}.xml -m ALNS -t $runfor -i ${i}" &
	done 
done 


