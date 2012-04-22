#!/bin/bash


for m in mGREEDY mALNS mGRASP ; do
	for t in mesh bitorus ; do 
		for s in 3 4 5 6 7 8 9 10 15 ; do 
			min=$(cat stat*${t}${s}x${s}*${m}* 2>/dev/null | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
			i=$(cat stat*${t}${s}x${s}*${m}* 2>/dev/null | awk '{if (NF==5) print $4}' | sort -nu | tail -1)
			if [ "$i" != "" ]; then
				iter="Iterations "
			fi
			if [ "$min" != "" ]; then
				echo -e "Found $min \tfor ${t}${s}x${s} ${m}\t$iter$i" 
			else
				if [ "$1" != "-c" ]; then
					echo -e "Pending... \tfor ${t}${s}x${s} ${m}" 
				fi
			fi
		done
	done
done


