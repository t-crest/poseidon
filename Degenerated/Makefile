.PHONY: create_xml clean init all DEGEN

EXE_DIR	= ./build
XML_DIR = ./xml
TOPO=torus
SIZE=16

all: DEGEN

init:
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

	
DEGEN: 
	@-mkdir -p build 2>&1
	cd build &&	cmake ../src && make
	
clean:
	@rm -rf $(BIN_DIR)

	

	

	
