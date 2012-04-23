source metanoc/stddev.sh


for s in 3 4 5 6 7 8 9 10 15 ; do 
	echo -e "${s}x${s}: \t"
	for b in 0 0.01 0.02 0.05 0.1 0.2 0.3 0.5 0.9 1.0 ; do 
		cat stat*bitorus${s}x${s}*GRASP*-b${b}__* | awk '{print $2}' | awk '{sum+=$1; sumsq+=$1*$1} END {print sqrt(sumsq/NR - (sum/NR)**2)}'
	done | tr '\n' '\t'
	echo -e '\n'
done

