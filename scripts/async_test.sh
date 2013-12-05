#!/bin/bash

../build/Poseidon -p ../data/async.xml -m GREEDY -s test.xml

#java -cp Converter/build/ converter.Converter test.xml test.txt Source-text
java -cp ../Converter/build/ converter.Converter test.xml test.h Aegean-async-c


../build/Poseidon -p ../data/bitorus2x2.xml -m GREEDY -s test.xml

#java -cp Converter/build/ converter.Converter test.xml test.txt Source-text
java -cp ../Converter/build/ converter.Converter test.xml test2.h Aegean-c
