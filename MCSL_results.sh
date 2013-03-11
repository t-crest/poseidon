#!/bin/bash
RESULTS="mGREEDY mrGREEDY mALNS mGRASP"
if [ "$1" = "Final" ]
then
	unset RESULTS
fi

for t in mesh torus ; do 
	for a in FFT-1024_complex Fpppp Robot RS-32_28_8_dec RS-32_28_8_enc Sparse H264-720p_dec ; do #H264-720p_dec
		for s in 3 4 5 6 7 8 9 10 11 12 13 14 15 16 ; do 
			for m in ${RESULTS} ; do
				min=$(cat stat*${t}_${t}_${s}x${s}_${a}*${m}* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
				i=$(cat stat*${t}_${t}_${s}x${s}_${a}*${m}* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
				if [ "$i" != "" ]; then
					iter="Iterations "
				fi
				if [ "$min" != "" ]; then
					echo -e "Found $min \tfor ${t}${s}x${s}_${a} ${m}\t$iter$i" 
				else
					if [ "$1" != "-c" ]; then
						echo -e "Pending... \tfor ${t}${s}x${s}_${a} ${m}" 
					fi
				fi
			done
			
			if [ "$1" = "Final" ]
			then
				min=$(cat stat*${t}_${t}_${s}x${s}_${a}* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
				i=$(cat stat*${t}_${t}_${s}x${s}_${a}* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
				util=$(cat stat*${t}_${t}_${s}x${s}_${a}* 2>/dev/null | grep -ERi Link_util_best | awk '{print $3}' | sort -nu | tail -1)
				if [ "$i" != "" ]; then
					iter="Iterations "
				fi
				if [ "$min" != "" ]; then
					echo -e "Found $min \tfor ${t}${s}x${s}_${a} ${m}\t$iter$i \tLink_util ${util}" 
				else
					if [ "$1" != "-c" ]; then
						echo -e "Pending... \tfor ${t}${s}x${s}_${a} ${m}" 
					fi
				fi
			fi
		done
	done
done