#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXE="${DIR}/build/MCSL"
#TRAFFIC_DIR="${DIR}/MCSL_NoC_Benchmark_Suite_v1.5/traffic"
TRAFFIC_DIR="${DIR}/MCSL_NoC_Traffic_Pattern_Suite_v1.6/traffic"
XML_DIR="${DIR}/xml"
APPLICATIONS="Sparse FFT-1024_complex Fpppp Robot RS-32_28_8_dec RS-32_28_8_enc H264-720p_dec H264-1080p_dec"
#APPLICATIONS="Sparse FFT-1024_complex"

for t in torus ; do # torus mesh
	rm -rf ${XML_DIR}/${t} 2>/dev/null
	rm -rf ${XML_DIR}/${t}_v 2>/dev/null
	mkdir -p ${XML_DIR}/${t} 2>/dev/null
	mkdir -p ${XML_DIR}/${t}_v 2>/dev/null
	for i in 4 ; do # 3 4 5 6 7 8 9 10 11 12 13 14 15 16
		echo "------------" ;
		echo ${i}x${i}_${t} ;
		echo "------------" ;
		mkdir -p ${XML_DIR}/${t}/${t}_${i}x${i} ;
		mkdir -p ${XML_DIR}/${t}_v/${t}_${i}x${i} ;
		for a in ${APPLICATIONS} ; do
			${EXE} -f "${TRAFFIC_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}.stp" \
			-o "${XML_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}_max_rate.stp.xml"
			${EXE} -v -f "${TRAFFIC_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}.stp" \
			-o "${XML_DIR}/${t}_v/${t}_${i}x${i}/${a}_${t}_${i}x${i}_max_rate.stp.xml"
			#${EXE} -f "${TRAFFIC_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}.stp" \
			#-m -o "${XML_DIR}/${t}/${t}_${i}x${i}/${a}_${t}_${i}x${i}_mean.stp.xml"
		done
	done
done
