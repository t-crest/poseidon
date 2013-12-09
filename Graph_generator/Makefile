.PHONY: GRAPH_GEN down_traffic create_xml clean all
SHELL := /bin/bash

all: GRAPH_GEN

GRAPH_GEN:
	@-mkdir -p build 2>&1
	cd build && $(COMPILER_FLAGS) cmake ../src && make && echo "GRAPH_GEN_PATH=$$(pwd)" >> ../../scripts/paths.sh

clean:
	@rm -rf build

