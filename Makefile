# Copyright 2013 Technical University of Denmark, DTU Compute.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted (subject to the limitations in the
# disclaimer below) provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
# GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
# HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation
# are those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of the copyright holder.
#

ifeq ($(TERM_PROGRAM),Apple_Terminal)
	COMPILER=CXX=clang++ CC=clang
	#FLAGS=CMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++"
	FLAGS=CMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++"
	#FLAGS=CMAKE_CXX_FLAGS="-std=c++0x -stdlib=libstdc++"
else
	# Only for testing clang++ on Ubuntu
	#COMPILER=CXX=clang++ CC=clang
	#FLAGS=CMAKE_CXX_FLAGS="-std=c++0x -stdlib=libstdc++"
	COMPILER=
	FLAGS=
endif
COMPILER_FLAGS=$(COMPILER) $(FLAGS)

.PHONY: clean realclean Converter MCSL poseidon pugixml clean_stats GRAPH_GEN

all: .check_tools pugixml Converter MCSL GRAPH_GEN poseidon

.check_tools:
	@command -v make >/dev/null 2>&1 || { echo >&2 "I require make but it's not installed.  Aborting."; exit 1; }
	@command -v cmake >/dev/null 2>&1 || { echo >&2 "I require cmake but it's not installed.  Aborting."; exit 1; }
	@command -v java >/dev/null 2>&1 || { echo >&2 "I require java but it's not installed.  Aborting."; exit 1; }
	@command -v javac >/dev/null 2>&1 || { echo >&2 "I require javac but it's not installed.  Aborting."; exit 1; }
	@command -v svn >/dev/null 2>&1 || { echo >&2 "I require svn but it's not installed.  Aborting."; exit 1; }
	@echo "Tools checked" > .check_tools

poseidon: lib/pugixml/build/libpugixml.a
	@-mkdir -p build 2>&1
	cd build && $(COMPILER_FLAGS) cmake ../src && $(MAKE) -s && echo "POSEIDON_PATH=$$(pwd)" >> ../scripts/paths.sh

.common-cli:
	@-mkdir -p lib 2>&1
	@cd lib && wget https://archive.apache.org/dist/commons/cli/source/commons-cli-1.4-src.tar.gz 
	@cd lib && tar -xvf commons-cli-1.4-src.tar.gz
	@cd lib && rm commons-cli-1.4-src.tar.gz
	@echo "Commons CLI downloaded" > .common-cli

Converter: .common-cli
	@cd ./Converter/src/converter && $(MAKE) -s

.pugixml:
	#@-mkdir -p lib 2>&1
	#@cd lib && svn checkout http://pugixml.googlecode.com/svn/tags/release-1.2 pugixml
	git submodule init
	git submodule update
	@echo "Pugixml downloaded" > .pugixml

lib/pugixml/build/libpugixml.a: .pugixml
	@-mkdir -p lib/pugixml/build 2>&1
	@cd lib/pugixml/build && $(COMPILER_FLAGS) cmake ../scripts && $(MAKE) -s && echo "PUGI_PATH=$$(pwd)" >> ../../../scripts/paths.sh

MCSL: lib/pugixml/build/libpugixml.a
	@cd MCSL && $(COMPILER_FLAGS) $(MAKE) -s

GRAPH_GEN: lib/pugixml/build/libpugixml.a
	@cd Graph_generator && $(COMPILER_FLAGS) $(MAKE) -s

#	DEGEN is not compiling at the moment
#DEGEN:
#	@cd Degenerated && make -s

help:

	@echo "================================================================================"
	@echo "== This is the help target of the poseidon main Makefile."
	@echo "=="
	@echo "== Targets:"
	@echo "==     all        : Builds third-party library, the scheduler and the schedule"
	@echo "==                  \tconverter."
	@echo "=="
	@echo "==     poseidon       : Builds the C++ source files of the scheduler"
	@echo "=="
	@echo "==     MCSL       : Builds the C++ source files of the MCSL converter"
	@echo "=="
#	@echo "==     DEGEN      : Builds the C++ source files of the Degenerated"
#	@echo "==                    NOT COMPILING!!!!!"
#	@echo "=="
	@echo "==     Converter  : Builds the Java files for the schedule converter."
	@echo "=="
	@echo "==     pugixml    : Updates and builds the third party xml library."
	@echo "=="
	@echo "==     clean      : Cleans the build directories."
	@echo "=="
	@echo "==     clean_stats: Cleans the stats generated by the scheduler."
	@echo "=="
	@echo "==     realclean  : Cleans the build directories and library directory."
	@echo "=="
	@echo "================================================================================"


clean:
	cd Converter/src/converter && $(MAKE) -s clean
	cd MCSL && $(MAKE) -s clean
	cd Graph_generator && $(MAKE) -s clean
	-rm -r build
	-rm ./scripts/paths.sh

clean_stats:
	-rm stat-*

realclean: clean
	-rm -rf lib
	-rm .check_tools
	-rm .pugixml
