.PHONY: create_xml clean realclean init all
SHELL := /bin/bash
#CXX	= g++
CC	= g++
OPT	= -g -std=c++0x -O3
WARN	= -Wall
CXXFLAGS= $(OPT) $(WARN)
SRC_DIR	= ./src
OBJ_DIR	= ./obj
EXE_DIR	= ./bin
XML_DIR = ./xml
OBJECTS	= $(OBJ_DIR)/DEGEN.o $(OBJ_DIR)/pugixml.o $(OBJ_DIR)/options.o
TOPO=torus
SIZE=16

all: DEGEN

init:
	-mkdir bin
	-mkdir obj
	-mkdir xml
	
create_xml: DEGEN
	
	
	# -rm -rf $(XML_DIR)/torus
	# -mkdir $(XML_DIR)/torus
	# i=3 ; while [[ $$i -le 16 ]] ; do \
		# echo "------------" ; \
		# echo $${i}x$${i}_torus ; \
		# echo "------------" ; \
		# mkdir $(XML_DIR)/torus/torus_$${i}x$${i} ; \
		# $(foreach TORUS_APPLICATION,$(TORUS_APPLICATIONS), \
			# $(EXE_DIR)/MCSL -f "$(TRAFFIC_DIR)/torus/torus_$${i}x$${i}/$(TORUS_APPLICATION)" \
				# -o "$(XML_DIR)/torus/torus_$${i}x$${i}/$(TORUS_APPLICATION).xml";)\
        # ((i = i + 1)) ; \
    # done

	
DEGEN: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXE_DIR)/DEGEN
	
$(OBJ_DIR)/DEGEN.o: $(src)/DEGEN.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/DEGEN.cpp -o $(OBJ_DIR)/DEGEN.o
	
$(OBJ_DIR)/pugixml.o: $(src)/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/pugixml.cpp -o $(OBJ_DIR)/pugixml.o
	
$(OBJ_DIR)/options.o: $(src)/options.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/options.cpp -o $(OBJ_DIR)/options.o
	
clean:
	@rm -f $(OBJ_DIR)/*.o 
	
realclean: clean
	@rm -f $(BIN_DIR)/MCSL*

#File dependencies
$(src)/pugiconfig.hpp:
	
$(src)/pugixml.hpp: $(src)/pugiconfig.hpp 

$(src)/pugixml.cpp: $(src)/pugixml.hpp $(src)/pugiconfig.hpp

$(src)/options.h: 

$(src)/options.cpp: $(src)/options.h 

$(src)/DEGEN.cpp: $(src)/pugixml.hpp $(src)/options.h
	

	

	
