all: source


source: doxygen poseidon.pdf

doxygen:
	doxygen poseidon.doxy

latexdoc:
	make -C poseidon/latex

poseidon.pdf: latexdoc
	cp poseidon/latex/refman.pdf poseidon.pdf

clean:
	-rm -r poseidon
	-rm poseidon.pdf

.PHONY: all source doxygen latexdoc clean
