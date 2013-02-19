#CXX	= g++
CC	= g++
OPT	= -g -std=c++0x
WARN	= -Wall
CXXFLAGS= $(OPT) $(WARN)
SRC_DIR	= ./src
OBJ_DIR	= ./obj
EXE_DIR	= ./bin
OBJECTS	= $(OBJ_DIR)/MCSL.o $(OBJ_DIR)/pugixml.o


all: MCSL
	
MCSL: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXE_DIR)/MCSL 
	
$(OBJ_DIR)/MCSL.o: $(src)/MCSL.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/MCSL.cpp -o $(OBJ_DIR)/MCSL.o
	
$(OBJ_DIR)/pugixml.o: $(src)/pugixml.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/pugixml.cpp -o $(OBJ_DIR)/pugixml.o
	
clean:
	@rm -f *.o 
	
realclean: clean
	@rm -f MCSL

#File dependencies
$(src)/pugiconfig.hpp:
	
$(src)/pugixml.hpp: $(src)/pugiconfig.hpp

$(src)/pugixml.cpp: $(src)/pugixml.hpp $(src)/pugiconfig.hpp

$(src)/MCSL.cpp: $(src)/pugixml.hpp
	

	

	