all: compile

compile:
	@-mkdir -p ../../build
	javac *.java -d ../../build
	jar cfe ../../converter.jar converter.Converter -C ../../build .
#	cd ../../build/converter && jar cf converter.jar *.class

clean:
	-rm -r ../../build
