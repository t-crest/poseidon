#!/bin/sh


# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c {
	pkill $(basename $0) # NB: Also kills other instances of this script!
	exit
}

function do_nothing {
    while [ 1 ] ; do
        sleep 1
        echo -n .
    done
}


function run() {
	while [ 1 ] ; do 
		metanoc/dist/Gramme/GNU-Linux-x86/metanoc "$@"
	done
}


for i in {1..32} ; do
	run "$@" &
done | awk 'BEGIN {min = 99999999} /n.p/ {i=strtonum(substr($4,2)); if (i<min) {min = i; print i} } '


do_nothing

