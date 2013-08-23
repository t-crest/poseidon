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
	
Converter:

help:
	@echo "=================================="
	@echo "== This is the help function"
	@echo "== of the SNTs main Makefile."
	@echo "== Targets:"
	@echo "==     all  : Builds the scheduler"
	@echo "==             and the converter"
	@echo "==     SNTs : Builds the scheduler"
	@echo "=================================="


clean:
	-rm -rf build

realclean: clean 
	-rm -rf lib
