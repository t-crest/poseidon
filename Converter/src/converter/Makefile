all: compile

compile:
	@-mkdir -p ../../build
	javac *.java -d ../../build
	jar cf ../../converter.jar -C ../../build .
#	cd ../../build/converter && jar cf converter.jar *.class

clean:
	-rm -r ../../build
