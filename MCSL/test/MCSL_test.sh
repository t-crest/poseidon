#!/bin/bash
GRAMME=true
DATA_DIR="./xml_in"
PROG=snts
TOPO=torus
SIZE=16
APPLICATION=RS-32_28_8_dec
if [ "${GRAMME}" = "true" ]
then
	CORES=32
	RUNFOR=10800
	FULL_PROG="../../dist/Gramme/GNU-Linux-x86/snts"
else
	CORES=1
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

#$FULL_PROG -f ${DATA_DIR}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${APPLICATION}_s_1.1.xml -m GREEDY
# 1 1.1 1.2 1.3 1.5 2 3 4 5 10 20 30
for s in 50 100 200 300 500; do
	# Single shot solutions
	for m in GREEDY rGREEDY; do
		run "-f ${DATA_DIR}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${APPLICATION}_s_${s}.xml -m ${m}" &
	done 	

	for b in 0.01 0.02 0.1 0.2 ; do
		run "-f ${DATA_DIR}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${APPLICATION}_s_${s}.xml -m GRASP -t $RUNFOR -b ${b}" &
	done 

	for i in GREEDY rGREEDY ; do
		run "-f ${DATA_DIR}/${TOPO}/${TOPO}_${SIZE}x${SIZE}/${APPLICATION}_s_${s}.xml -m ALNS -t $RUNFOR -i ${i}" &
	done 
done