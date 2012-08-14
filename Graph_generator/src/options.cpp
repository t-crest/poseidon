#include "options.h"

options::options(int argc, char *argv[])
// Option defaults:
:	output_dir("")
{
	bool output = false;
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "o:")) != -1;) {
		switch (c) {
			case 'o':   output_dir = optarg; output = true;				break; // o for specifying the output directory
			default:	ensure(false, "Unknown flag " << c << ".");
		}
	}
	
	if (argc < 2){ // If no comand line options are given, the help menu is printet.
		print_help();
	}

	/* Some input validation */
	if (output)
		ensure(output_dir.size() > 0, "Empty output directory given.");
}


void options::print_help()
{
	cout << endl << "Help menu for SNTs" << endl;
	cout << "\tMandatory options:" << endl; 
	print_option('o',"Specify the output directory for the use cases.");
	cout << endl;
	
	delete this;
	return exit(0);
}

void options::print_option(char opt, string text){
	cout << "\t-" << opt << "\t" << text << endl;
}

options::~options()
{
	
}


namespace global {
	options* opts;
}


