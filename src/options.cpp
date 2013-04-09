#include "options.h"

using namespace std;

string get_stat_name(int argc, char *argv[]) 
{
	string ret;
	ret += "stat";

	for (int i = 1; i < argc; i++) {
		string tmp = string(argv[i]);
		for (int i = 0; i < tmp.size(); i++) if (tmp[i] == '/' || tmp[i] =='\\') tmp[i] = '_';
		ret += tmp;
	}
	ret += "__";

	{
		char buff[30];
		time_t now = time(NULL);
		strftime(buff, 30, "%Y-%m-%d_%H:%M:%S", localtime(&now));	
		ret += string(buff);
	}
	ret += "__";
	
	std::random_device get_rand;
	ret += ::lex_cast<string>(get_rand());
	
	return ret;
}

options::options(int argc, char *argv[])
// Option defaults:
:	input_file(""),
	output_dir(""),
	metaheuristic(ERR),
	draw(false),
	meta_inital(ERR),
	save_best(true), // normally, we want to remember the best globally solution
	run_for(0),
	beta_percent(-1.0),
	stat_file(get_stat_name(argc, argv).c_str(), fstream::out)
{
	bool output = false;
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:m:di:qt:b:ho:")) != -1;) {
		switch (c) {
			case 'm':	metaheuristic = parse_meta_t(string(optarg));	break; // m for chosen metaheuristic
			case 'f':	input_file = optarg;							break; // f for xml input file
			case 'd':	draw = true;									break; // d for draw
			case 'i':	meta_inital = parse_meta_t(string(optarg));		break; // i for initial sol
			case 'q':	save_best = false;								break; // q for quick
			case 'b':	beta_percent = ::lex_cast<float>(string(optarg)); break; // b for beta_percent
			case 't':	run_for = ::lex_cast<time_t>(string(optarg));	break; // t for run time, in seconds
			case 'h':   print_help();									break; // h for the help menu
			case 'o':   output_dir = optarg; output = true;				break; // o for specifying the output directory
			default:	ensure(false, "Unknown flag " << c << ".");
		}
	}
	
	if (argc < 2){ // If no comand line options are given, the help menu is printet.
		print_help();
	}

	/* Some input validation */
	ensure(input_file.size() > 0, "Empty file name given.");
	ensure(metaheuristic != ERR, "Metaheuristic must be set to GRASP or ALNS, etc.");
	if (meta_inital != ERR)
		ensure(metaheuristic == ALNS, "ALNS-inital given, but we don't run ALNS");
	if (metaheuristic == ALNS)
		ensure(meta_inital != ERR, "ALNS specified, but no inital solution specified");
	
	const bool both_alns = (metaheuristic == ALNS && meta_inital == ALNS);
	ensure(!both_alns, "Can not use ALNS as initial solution for ALNS");

	if (metaheuristic == GRASP || metaheuristic == ALNS)
		ensure(run_for != 0, "Not specified how long the metaheuristic should run");

	if (metaheuristic == GRASP)
		ensure(0.0 <= beta_percent && beta_percent <= 1.0, "Beta not from 0.0 to 1.0");
	
	if (output)
		ensure(output_dir.size() > 0, "Empty output directory given.");
}

options::meta_t options::parse_meta_t(string str) 
{
	meta_t ret = ERR;
	if		(str=="RANDOM")		ret=RANDOM;
	else if	(str=="BAD_RANDOM")	ret=BAD_RANDOM;
	else if (str=="GREEDY")		ret=GREEDY;
	else if (str=="rGREEDY")	ret=rGREEDY;
	else if (str=="CROSS")		ret=CROSS;
	else if (str=="GRASP")		ret=GRASP;		
	else if (str=="ALNS")		ret=ALNS;
	return ret;
}

void options::print_help()
{
	cout << endl << "Help menu for SNTs" << endl;
	cout << "\tMandatory options:" << endl; 
	print_option('m',"Choose the Metaheuristic to apply schedule [CROSS, GRASP, ALNS].");
	print_option('f',"The file containing the scheduling problem.");
	print_option('i',"Choose the initial solutionused by the Metaheuristics [RANDOM, BAD_RANDOM, GREEDY, rGREEDY].");
	print_option('t',"The time in seconds for which the metaheuristic should run.");
	cout << "\tOptional options:" << endl;
	print_option('d',"If specified the network is drawn in an SVG file.");
	print_option('q',"If specified the scheulder will make a dry run, does not save the result.");
	print_option('b',"Specify the beta value, only applicable when using the GRASP metaheuristic.");
	print_option('o',"Specify the output directory for VHDL routing tables.");
	print_option('h',"Shows the help menu, I guess you know that.");
	cout << endl;
	
	delete this;
	return exit(0);
}

void options::print_option(char opt, string text){
	cout << "\t-" << opt << "\t" << text << endl;
}

options::~options()
{
	this->stat_file.close();
}


namespace global {
	options* opts;
}


