#!/bin/bash


for m in mGREEDY mALNS mGRASP ; do
	for t in mesh bitorus ; do 
		for s in 3 4 5 6 7 8 9 10 15 ; do 
				min=$(cat stat*${t}${s}x${s}*${m}* | awk '{if (NF==5) print $3}  {if (NF==2) print $2}' | sort -nur | tail -1)
				echo -e "Found $min \tfor ${t}${s}x${s} ${m}" 
		done
	done
done


