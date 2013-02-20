.PHONY: down_traffic create_xml clean realclean init all
SHELL := /bin/bash
#CXX	= g++
CC	= g++
OPT	= -g -std=c++0x -O3
WARN	= -Wall
CXXFLAGS= $(OPT) $(WARN)
SRC_DIR	= ./src
OBJ_DIR	= ./obj
EXE_DIR	= ./bin
TRAFFIC_DIR = ./MCSL_NoC_Benchmark_Suite_v1.5/traffic
XML_DIR = ./xml
OBJECTS	= $(OBJ_DIR)/MCSL.o $(OBJ_DIR)/pugixml.o

MESH_APPLICATIONS = Sparse_mesh_$${i}x$${i}.stp FFT-1024_complex_mesh_$${i}x$${i}.stp Fpppp_mesh_$${i}x$${i}.stp Robot_mesh_$${i}x$${i}.stp RS-32_28_8_dec_mesh_$${i}x$${i}.stp RS-32_28_8_enc_mesh_$${i}x$${i}.stp H264-720p_dec_mesh_$${i}x$${i}.stp
TORUS_APPLICATIONS = Sparse_torus_$${i}x$${i}.stp FFT-1024_complex_torus_$${i}x$${i}.stp Fpppp_torus_$${i}x$${i}.stp Robot_torus_$${i}x$${i}.stp RS-32_28_8_dec_torus_$${i}x$${i}.stp RS-32_28_8_enc_torus_$${i}x$${i}.stp H264-720p_dec_torus_$${i}x$${i}.stp

all: MCSL

init:
	-mkdir bin
	-mkdir obj
	-mkdir xml

down_traffic: | init
	-rm -rf ./MCSL_NoC_Benchmark_Suite_v1.5
	wget http://www.ece.ust.hk/~eexu/release/MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	gzip -d MCSL_NoC_Benchmark_Suite_v1.5.tar.gz
	tar -xf MCSL_NoC_Benchmark_Suite_v1.5.tar
	rm -f MCSL_NoC_Benchmark_Suite_v1.5.tar
	@echo "------------------"
	@echo "Traffic downloaded"
	@echo "------------------"
	
create_xml: MCSL #| down_traffic 
	-rm -rf $(XML_DIR)/mesh
	-mkdir $(XML_DIR)/mesh
	i=3 ; while [[ $$i -le 16 ]] ; do \
		echo "------------" ; \
		echo $${i}x$${i}_mesh ; \
		echo "------------" ; \
		mkdir $(XML_DIR)/mesh/mesh_$${i}x$${i} ; \
		$(foreach MESH_APPLICATION,$(MESH_APPLICATIONS), \
			$(EXE_DIR)/MCSL "$(TRAFFIC_DIR)/mesh/mesh_$${i}x$${i}/$(MESH_APPLICATION)" \
				"$(XML_DIR)/mesh/mesh_$${i}x$${i}/$(MESH_APPLICATION).xml";)\
        ((i = i + 1)) ; \
    done
	
	-rm -rf $(XML_DIR)/torus
	-mkdir $(XML_DIR)/torus
	i=3 ; while [[ $$i -le 16 ]] ; do \
		echo "------------" ; \
		echo $${i}x$${i}_torus ; \
		echo "------------" ; \
		mkdir $(XML_DIR)/torus/torus_$${i}x$${i} ; \
		$(foreach TORUS_APPLICATION,$(TORUS_APPLICATIONS), \
			$(EXE_DIR)/MCSL "$(TRAFFIC_DIR)/torus/torus_$${i}x$${i}/$(TORUS_APPLICATION)" \
				"$(XML_DIR)/torus/torus_$${i}x$${i}/$(TORUS_APPLICATION).xml";)\
        ((i = i + 1)) ; \
    done

	
MCSL: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXE_DIR)/MCSL 
	
$(OBJ_DIR)/MCSL.o: $(src)/MCSL.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/MCSL.cpp -o $(OBJ_DIR)/MCSL.o
	
$(OBJ_DIR)/pugixml.o: $(src)/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/pugixml.cpp -o $(OBJ_DIR)/pugixml.o
	
clean:
	@rm -f $(OBJ_DIR)/*.o 
	
realclean: clean
	@rm -f $(BIN_DIR)/MCSL*

#File dependencies
$(src)/pugiconfig.hpp:
	
$(src)/pugixml.hpp: $(src)/pugiconfig.hpp

$(src)/pugixml.cpp: $(src)/pugixml.hpp $(src)/pugiconfig.hpp

$(src)/MCSL.cpp: $(src)/pugixml.hpp
	

	

	
