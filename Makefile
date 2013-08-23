.PHONY: clean realclean Converter

all: pugixml Converter SNTs

SNTs: 
	@-mkdir -p build 2>&1
	cd build &&	cmake ../src && make

Converter:
	cd Converter/src/converter && make

pugixml:
	@-mkdir -p lib 2>&1
	cd lib && svn checkout http://pugixml.googlecode.com/svn/tags/release-1.2 pugixml
	@-mkdir -p lib/pugixml/build 2>&1
	cd lib/pugixml/build && cmake ../scripts && make

help:

	@echo "================================================================================"
	@echo "== This is the help function"
	@echo "== of the SNTs main Makefile."
	@echo "== Targets:"
	@echo "==     all       : Builds third-party library, the scheduler and the schedule"
	@echo "==                 \tconverter."
	@echo "=="
	@echo "==     SNTs      : Builds the C++ source files of the scheduler"
	@echo "=="
	@echo "==     Converter : Builds the Java files for the schedule converter."
	@echo "=="
	@echo "==     pugixml   : Updates and builds the third party xml library."
	@echo "=="
	@echo "==     clean     : Cleans the build directories."
	@echo "=="
	@echo "==     realclean : Cleans the build directories and library directory."
	@echo "=="
	@echo "================================================================================"


clean:
	cd Converter/src/converter && make clean
	-rm -rf build

realclean: clean
	-rm -rf lib
