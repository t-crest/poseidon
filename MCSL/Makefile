.PHONY: MCSL down_traffic create_xml clean all
SHELL := /bin/bash

all: MCSL

#############################################################
#	Download the MCSL benchmark suite
#	Close to half a GB
#############################################################
down_traffic:
	-rm -rf ./MCSL_NoC_Benchmark_Suite_v1.5
	wget http://www.ece.ust.hk/~eexu/release/MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	gzip -d MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	tar -xf MCSL_NoC_Benchmark_Suite_v1.5.tar
	rm -f MCSL_NoC_Benchmark_Suite_v1.5.tar
	@echo "----------------------"
	@echo "-- Traffic downloaded"
	@echo "----------------------"
	
#############################################################
#	Create the XML files for the different topologies
#	and applications.
#############################################################
create_xml: MCSL
	@-mkdir -p xml 2>&1
	sh create_xml.sh
	
MCSL: 
	@-mkdir -p build 2>&1
	cd build &&	cmake ../src && make
	
clean:
	@rm -rf build 
	
