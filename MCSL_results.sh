#!/bin/bash


for t in mesh torus ; do 
	for s in 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do 
		for a in FFT-1024_complex Fpppp H264-720p_dec Robot RS-32_28_8_dec RS-32_28_8_enc Sparse; do
			for m in mGREEDY mALNS mGRASP ; do
				min=$(cat stat*${t}_${t}_${s}x${s}_${a}*${m}* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
				i=$(cat stat*${t}_${t}_${s}x${s}_${a}*${m}* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
				if [ "$i" != "" ]; then
					iter="Iterations "
				fi
				if [ "$min" != "" ]; then
					echo -e "Found $min \tfor ${t}${s}x${s}_${a} ${m}\t$iter$i" 
				else
					if [ "$1" != "-c" ]; then
						echo -e "Pending... \tfor ${t}${s}x${s} ${m}" 
					fi
				fi
			done
		done
	done
done


stat-f._MCSL_xml_torus_torus_9x9_Sparse_torus_9x9.stp.xml-mALNS-t10800-iGREEDY__2013-02-20_14:25:16__3490909870