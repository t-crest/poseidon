#!/bin/bash
cd /home/eisbaw/NetBeansProjects/Metaheuristics/SNTs # Make sure $PWD is what I expect it to be

set -v
(cd metanoc && "/usr/bin/make" --quiet -f nbproject/Makefile-Gramme.mk QMAKE= SUBPROJECTS= .build-conf) || exit
rsync -r * gramme:meta/ || exit
ssh gramme "(cd meta/ && time metanoc/dist/Gramme/GNU-Linux-x86/metanoc -f data/bitorus15x15.xml)"
