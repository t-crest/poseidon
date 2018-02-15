all: compile

compile:
	@-mkdir -p ../../build
	javac -cp ../../../lib/commons-cli-1.4-src/src/main/java/ *.java -d ../../build
	jar cfe ../../converter.jar converter.Converter -C ../../build .
#	cd ../../build/converter && jar cf converter.jar *.class

clean:
	-rm -r ../../build
