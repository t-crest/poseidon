#include "options.h"

options::options(int argc, char *argv[])
// Option defaults:
:	input_file(""),
	output_file(""),
	var_len_pkt(false)
{
	bool output = false;
	sigma = 1.0;
	mean = false;

	if (argc < 2){ // If to few command line options are given, the help menu is printed.
		print_help();
	}
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:o:s:hmv")) != -1;) {
		switch (c) {
			case 'f':	input_file = optarg;							break; // f for xml input file
			case 'h':   print_help();									break; // h for the help menu
			case 'o':   output_file = optarg; output = true;			break; // o for specifying the output directory
			case 's':	sigma = atof(optarg);							break;
			case 'v':	var_len_pkt = true;								break;
			case 'm':	mean = true;									break; // m for mean traffic allocation, otherwise max rate allocation
			default:	cout << "Unknown flag " << c << "." << endl;
		}
	}

	/* Some input validation */
	if(!input_file.size() > 0) {
		cout << "Empty file name given." << endl;;
	}
	if (output && !(output_file.size() > 0))
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
	print_option('v',"Hardware supports variable length packets");
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


