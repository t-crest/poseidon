#!/bin/bash
GRAMME=true
DATA_DIR="./xml_in"
PROG=snts
TOPO=torus
SIZE=16
command -v nproc >/dev/null && CORES=$(nproc) || CORES=32
TORUS_APPLICATIONS="RS-32_28_8_dec RS-32_28_8_enc FFT-1024_complex Sparse Robot Fpppp"
if [ "${GRAMME}" = "true" ]
then
	RUNFOR=10800
	FULL_PROG="../../dist/Gramme/GNU-Linux-x86/snts"
else
	RUNFOR=10
	FULL_PROG="../../dist/Release/Cygwin_4.x-Windows/${PROG}"
fi



function run {
#	echo "Queing $FULL_PROG $@"
	sleep $(( $RANDOM % 15 )).$(( $RANDOM % 1000 ))

	while [ $(pgrep snts | wc -l) -ge $CORES ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $FULL_PROG $@"	
	$FULL_PROG $@
}

for a in ${TORUS_APPLICATIONS} ; do
	
	for s in 1 1.1 1.2 1.3 1.5 2 3 4 5 10 20 30 50 100 200 300 500 ; do
		# Single shot solutions
#		for m in GREEDY rGREEDY; do
		for m in GREEDY ; do
			run "-f ${DATA_DIR}/${a}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${a}_s_${s}.xml -m ${m}" &
		done 	

		# for b in 0.01 0.02 0.1 0.2 ; do
		# 	run "-f ${DATA_DIR}/${a}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${a}_s_${s}.xml -m GRASP -t $RUNFOR -b ${b}" &
		# done 

		# for i in GREEDY rGREEDY ; do
		# 	run "-f ${DATA_DIR}/${a}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${a}_s_${s}.xml -m ALNS -t $RUNFOR -i ${i}" &
		# done 
	done
done
