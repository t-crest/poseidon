.PHONY: down_traffic create_xml clean realclean init all
SHELL := /bin/bash
CXX	= g++
CC	= g++
OPT	= -g -std=c++0x -O3
WARN	= -Wall
CXXFLAGS= $(OPT) $(WARN)
SRC_DIR	= ./src
OBJ_DIR	= ./obj
EXE_DIR	= ./bin
OBJECTS	= $(OBJ_DIR)/MCSL.o $(OBJ_DIR)/pugixml.o $(OBJ_DIR)/options.o

all: MCSL

init:
	-mkdir bin
	-mkdir obj
	-mkdir xml

#############################################################
#	Download the MCSL benchmark suite
#	Close to half a GB
#############################################################
down_traffic: | init
	-rm -rf ./MCSL_NoC_Benchmark_Suite_v1.5
	wget http://www.ece.ust.hk/~eexu/release/MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	gzip -d MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	tar -xf MCSL_NoC_Benchmark_Suite_v1.5.tar
	rm -f MCSL_NoC_Benchmark_Suite_v1.5.tar
	@echo "------------------"
	@echo "Traffic downloaded"
	@echo "------------------"
	
#############################################################
#	Create the XML files for the different topologies
#	and applications.
#############################################################
create_xml: MCSL #| down_traffic 
	sh create_xml.sh
	
MCSL: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXE_DIR)/MCSL 
	
$(OBJ_DIR)/MCSL.o: $(SRC_DIR)/MCSL.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/MCSL.cpp -o $(OBJ_DIR)/MCSL.o
	
$(OBJ_DIR)/pugixml.o: $(SRC_DIR)/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/pugixml.cpp -o $(OBJ_DIR)/pugixml.o
	
$(OBJ_DIR)/options.o: $(SRC_DIR)/options.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/options.cpp -o $(OBJ_DIR)/options.o
	
clean:
	@rm -f $(OBJ_DIR)/*.o 
	
realclean: clean
	@rm -f $(BIN_DIR)/MCSL*

#File dependencies
$(SRC_DIR)/pugiconfig.hpp:
	
$(SRC_DIR)/pugixml.hpp: $(SRC_DIR)/pugiconfig.hpp 

$(SRC_DIR)/pugixml.cpp: $(SRC_DIR)/pugixml.hpp $(SRC_DIR)/pugiconfig.hpp

$(SRC_DIR)/options.h: 

$(SRC_DIR)/options.cpp: $(SRC_DIR)/options.h 

$(SRC_DIR)/MCSL.cpp: $(SRC_DIR)/pugixml.hpp $(SRC_DIR)/options.h
	

	

	
