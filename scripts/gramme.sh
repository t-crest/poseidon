#!/bin/bash
cd /home/rasmus/SNTs # Make sure $PWD is what I expect it to be
#cd metanoc &&
set -v
( "/usr/bin/make" --quiet -f nbproject/Makefile-Gramme.mk QMAKE= SUBPROJECTS= .build-conf) || exit
rsync -r * gramme:meta/ || exit
ssh gramme "(cd meta/ && time dist/Gramme/GNU-Linux-x86/snts $@)"

