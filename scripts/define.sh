#!/bin/bash

source $1/paths.sh
if [[ -z "$SNTS_PATH" ]]; then
	echo "Paths was not set correctly, make project again."
	exit;
fi

PROG=SNTs
RUNFOR=10
FULL_PROG="${SNTS_PATH}/${PROG}"
command -v nproc >/dev/null && CORES=$(nproc) || CORES=32

GRAMME=false
if [ "${GRAMME}" = "true" ]
then
	RUNFOR=36000
	FULL_PROG="./dist/Gramme/GNU-Linux-x86/${PROG}"
fi

function run {
	while [ $(pgrep SNTs | wc -l) -ge $CORES ] ; do 
		sleep $(( $RANDOM % 5 )).$(( $RANDOM % 1000 ))
	done
	echo "Executing $FULL_PROG $@"	
	$FULL_PROG -a $@ &
}