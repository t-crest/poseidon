#!/bin/bash
RESULTS="mALNS mGRASP" # mGREEDY mrGREEDY
DATA_DIR=./stats
APPLICATIONS="RS-32_28_8_enc RS-32_28_8_dec Robot Fpppp FFT-1024_complex Sparse H264-720p_dec"
if [ "$1" = "Final" ]
then
	unset RESULTS
fi

for t in mesh torus ; do
	for a in ${APPLICATIONS} ; do
		for size in 3 4 5 6 7 8 ; do
			for s in 1 ; do # 1.2 1.5 2 3 5 10 20 30 50 100 200 300 500
				for m in ${RESULTS} ; do
					min=$(cat ${DATA_DIR}/stat*${t}_${t}_${size}x${size}_${a}_s_${s}.xml*${m}* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
					i=$(cat ${DATA_DIR}/stat*${t}_${t}_${size}x${size}_${a}_s_${s}.xml*${m}* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
					if [ "$i" != "" ]; then
						iter="Iterations "
					fi
					if [ "$min" != "" ]; then
						echo -e "Found $min \tfor ${a}_${t}${size}x${size}_${s} ${m}\t$iter$i"
					else
						if [ "$1" != "-c" ]; then
							echo -e "Pending... \tfor ${a}_${t}${size}x${size}_${s} ${m}"
						fi
					fi
				done

				if [ "$1" = "Final" ]
				then
					min=$(cat ${DATA_DIR}/stat*${t}_${t}_${size}x${size}_${a}_s_${s}.xml* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
					i=$(cat ${DATA_DIR}/stat*${t}_${t}_${size}x${size}_${a}_s_${s}.xml* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
					util=$(cat ${DATA_DIR}/stat*${t}_${t}_${size}x${size}_${a}_s_${s}.xml* 2>/dev/null | grep -ERi Link_util_best | awk '{print $3}' | sort -nu | tail -1)
					if [ "$i" != "" ]; then
						iter="Iterations "
					fi
					if [ "$min" != "" ]; then
						echo -e "Found $min \tfor ${a}_${t}${size}x${size}_${s} \t$iter${i} \tLink_util ${util}"
					else
						if [ "$1" != "-c" ]; then
							echo -e "Pending... \tfor ${a}_${t}${size}x${size}_${s}"
						fi
					fi
				fi
			done
		done
	done
done
