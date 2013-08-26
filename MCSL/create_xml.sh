#!/bin/bash
EXE="./build/MCSL"
TRAFFIC_DIR=./MCSL_NoC_Benchmark_Suite_v1.5/traffic
XML_DIR=./xml
APPLICATIONS="Sparse FFT-1024_complex Fpppp Robot RS-32_28_8_dec RS-32_28_8_enc H264-720p_dec"

for t in torus mesh ; do
	rm -rf ${XML_DIR}/${t} 2>/dev/null
	rm -rf ${XML_DIR}/${t} 2>/dev/null
	mkdir ${XML_DIR}/${t} 2>/dev/null
	mkdir ${XML_DIR}/${t} 2>/dev/null
	for i in 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do
		echo "------------" ;
		echo ${i}x${i}_${t} ;
		echo "------------" ;
		mkdir ${XML_DIR}/${t}/${t}_${i}x${i} ;
		for a in ${APPLICATIONS} ; do
			${EXE} -f "${TRAFFIC_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}.stp" \
			-o "${XML_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}_max_rate.stp.xml"
			${EXE} -f "${TRAFFIC_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}.stp" \
			-m -o "${XML_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}_mean.stp.xml"
		done
	done
done
