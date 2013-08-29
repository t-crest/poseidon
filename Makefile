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

.PHONY: clean realclean Converter MCSL SNTs pugixml clean_stats GRAPH_GEN

all: .check_tools pugixml Converter MCSL GRAPH_GEN SNTs

.check_tools:
	@command -v make >/dev/null 2>&1 || { echo >&2 "I require make but it's not installed.  Aborting."; exit 1; }
	@command -v cmake >/dev/null 2>&1 || { echo >&2 "I require cmake but it's not installed.  Aborting."; exit 1; }
	@command -v javac >/dev/null 2>&1 || { echo >&2 "I require cmake but it's not installed.  Aborting."; exit 1; }
	@command -v svn >/dev/null 2>&1 || { echo >&2 "I require svn but it's not installed.  Aborting."; exit 1; }
	@command -v cmake >/dev/null 2>&1 || { echo >&2 "I require cmake but it's not installed.  Aborting."; exit 1; }
	@echo "Tools checked" > .check_tools

SNTs: 
	@-mkdir -p build 2>&1
	@cd build && cmake ../src && make -s && echo "SNTS_PATH=$$(pwd)" >> ../scripts/paths.sh

Converter:
	@cd Converter/src/converter && make -s

.pugixml:
	@-mkdir -p lib 2>&1
	@cd lib && svn checkout http://pugixml.googlecode.com/svn/tags/release-1.2 pugixml
	@echo "Pugixml downloaded" > .pugixml

pugixml: .pugixml
	@-mkdir -p lib/pugixml/build 2>&1
	@cd lib/pugixml/build && cmake ../scripts && make -s && echo "PUGI_PATH=$$(pwd)" >> ../../../scripts/paths.sh

MCSL:
	@cd MCSL && make -s

GRAPH_GEN:
	@cd Graph_generator && make -s

#	DEGEN is not compiling at the moment
#DEGEN:
#	@cd Degenerated && make -s

help:

	@echo "================================================================================"
	@echo "== This is the help target of the SNTs main Makefile."
	@echo "=="
	@echo "== Targets:"
	@echo "==     all        : Builds third-party library, the scheduler and the schedule"
	@echo "==                  \tconverter."
	@echo "=="
	@echo "==     SNTs       : Builds the C++ source files of the scheduler"
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
	cd Converter/src/converter && make -s clean
	cd MCSL && make -s clean
	cd Graph_generator && make -s clean
	-rm -r build
	-rm ./scripts/paths.sh

clean_stats:
	-rm stat-*

realclean: clean
	-rm -r lib
	-rm .check_tools
	-rm .pugixml
