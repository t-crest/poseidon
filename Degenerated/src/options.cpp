#include "options.h"

options::options(int argc, char *argv[])
// Option defaults:
:	output_dir("")
{
	bool output = false;
	sigma = 1.0;
	network_size = 0;
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:o:s:h")) != -1;) {
		switch (c) {
			case 'n':	network_size = atoi(optarg);					break; // n for network size
			case 'h':   print_help();									break; // h for the help menu
			case 'o':   output_dir = optarg; output = true;				break; // o for specifying the output directory
			case 's':	sigma = atof(optarg);							break;
			default:	cout << "Unknown flag " << c << "." << endl;
		}
	}
	
	
	if (argc < 2){ // If no comand line options are given, the help menu is printet.
		print_help();
	}

	/* Some input validation */
	
	if (!(network_size > 0)){
		cout << "Network size not specified." << endl;
	}
	if (output || !(output_file.size() > 0))
		cout << "Empty output directory given." << endl;;
}

void options::print_help()
{
	cout << endl << "Help menu for MCSL converter" << endl;
	cout << "\tMandatory options:" << endl; 
	print_option('f',"The file containing the scheduling problem.");
	cout << "\tOptional options:" << endl;
	print_option('o',"Specify the output directory for VHDL routing tables.");
	print_option('s',"Specify the overallocation constant sigma");
	print_option('h',"Shows the help menu, I guess you know that.");
	cout << endl;
	
	return exit(0);
}

void options::print_option(char opt, string text){
	cout << "\t-" << opt << "\t" << text << endl;
}



namespace global {
	options* opts;
}


