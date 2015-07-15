.PHONY: graph_gen down_traffic create_xml clean all
SHELL := /bin/bash

all: graph_gen

graph_gen:
	@-mkdir -p build 2>&1
	cd build && $(COMPILER_FLAGS) cmake ../src && $(MAKE) && echo "GRAPH_GEN_PATH=$$(pwd)" >> ../../scripts/paths.sh

clean:
	@rm -rf build

