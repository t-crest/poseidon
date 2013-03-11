#!/bin/bash
GRAMME=true
command -v nproc >/dev/null && CORES=$(nproc) || CORES=32
if [ "${GRAMME}" = "true" ]
then
	RUNFOR=7200
	FULL_PROG="dist/Gramme/GNU-Linux-x86/snts"
else
	RUNFOR=10
	FULL_PROG="dist/Release/Cygwin_4.x-Windows/snts"
fi

DATA_DIR="./MCSL/xml"
APPLICATIONS="FFT-1024_complex Fpppp H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse"

function run {
#	echo "Queing $FULL_PROG $@"
#	sleep $(( $RANDOM % 1 )).$(( $RANDOM % 1000 ))

	while [ $(pgrep snts | wc -l) -ge $CORES ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done

	echo "Executing $FULL_PROG $@"	
	$FULL_PROG $@ &
}

for t in torus mesh ; do
	for s in {3..16} ; do
		for a in ${APPLICATIONS}; do
			for r in mean max_rate ; do
				# Single shot solutions
				for m in GREEDY rGREEDY; do
					run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m ${m}"
				done 	

				for b in 0.01 0.02 0.1 0.2 ; do
					run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m GRASP -t $RUNFOR -b ${b}"
				done 

				for i in GREEDY rGREEDY ; do
					run "-f ${DATA_DIR}/${t}/${t}_${s}x${s}/${a}_${t}_${s}x${s}_${r}.stp.xml -m ALNS -t $RUNFOR -i ${i}"
				done 
			done
		done
	done 
done