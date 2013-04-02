#!/bin/bash
GRAMME=true
PROG=snts
command -v nproc >/dev/null && CORES=$(nproc) || CORES=32
if [ "${GRAMME}" = "true" ]
then
	RUNFOR=36000
	FULL_PROG="./dist/Gramme/GNU-Linux-x86/${PROG}"
else
	RUNFOR=10
	FULL_PROG="./dist/Release/Cygwin_4.x-Windows/${PROG}"
fi

function run {
#	echo "Queing $FULL_PROG $@"
#	sleep $(( $RANDOM % 1 )).$(( $RANDOM % 1000 ))

	while [ $(pgrep snts | wc -l) -ge $CORES ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $FULL_PROG $@"	
	$FULL_PROG $@ &
}

for t in bitorus mesh ; do
	for s in 15 ; do # 3 4 5 6 7 8 9 10 15
		# Single shot solutions
		for m in GREEDY rGREEDY ; do # RANDOM BAD_RANDOM
			run "-f ./data/${t}${s}x${s}.xml -m ${m}"
		done 	

		for b in 0 0.01 0.02 0.05 0.1 0.2 0.3 0.5 0.9 1.0 ; do
			run "-f ./data/${t}${s}x${s}.xml -m GRASP -t $RUNFOR -b ${b}"
		done 

		for i in GREEDY rGREEDY RANDOM ; do # BAD_RANDOM
			run "-f ./data/${t}${s}x${s}.xml -m ALNS -t $RUNFOR -i ${i}"
		done 
	done 
done


