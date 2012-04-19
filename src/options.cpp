#include "options.h"


string get_stat_name(int argc, char *argv[]) 
{
	string ret;
	for (int i = 1; i < argc; i++) {
		string tmp = string(argv[i]);
		for (int i = 0; i < tmp.size(); i++) if (tmp[i] == '/' || tmp[i] =='\\') tmp[i] = '_';
		ret += tmp;
	}
	ret += " ";

	{
		char buff[30];
		time_t now = time(NULL);
		strftime(buff, 30, "%Y-%m-%d %H:%M:%S", localtime(&now));	
		ret += string(buff);
	}
	
	ret += "_";
	std::random_device get_rand;
	ret += ::lex_cast<string>(get_rand());
	
	ret += ".stat";
	return ret;
}

options::options(int argc, char *argv[])
// Option defaults:
:	input_file(""),
	metaheuristic(ERR),
	draw(false),
	meta_inital(ERR),
	save_best(true), // normally, we want to remember the best globally solution
	run_for(0),
	stat_file(get_stat_name(argc, argv).c_str(), fstream::out)
{
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:m:di:qt:")) != -1;) {
		switch (c) {
			case 'm':	metaheuristic = parse_meta_t(string(optarg));	break; // m for chosen metaheuristic
			case 'f':	input_file = optarg;							break; // f for xml input file
			case 'd':	draw = true;									break; // d for draw
			case 'i':	meta_inital = parse_meta_t(string(optarg));		break; // i for initial sol
			case 'q':	save_best = false;								break; // q for quick
			case 't':	run_for = ::lex_cast<time_t>(string(optarg));	break; // t for run time, in seconds
			default:	ensure(false, "Unknown flag " << c << ".");
		}
	}

	/* Some input validation */
	ensure(input_file.size() > 0, "Empty file name given");
	ensure(metaheuristic != ERR, "Metaheuristic must be set to GRASP or ALNS, etc.");
	if (meta_inital != ERR)
		ensure(metaheuristic == ALNS, "ALNS-inital given, but we don't run ALNS");
	if (metaheuristic == ALNS)
		ensure(meta_inital != ERR, "ALNS specified, but no inital solution specified");
	
	const bool both_alns = (metaheuristic == ALNS && meta_inital == ALNS);
	ensure(!both_alns, "Can not use ALNS as initial solution for ALNS");

	if (metaheuristic == GRASP || metaheuristic == ALNS)
		ensure(run_for != 0, "Not specified how long the metaheuristic should run");
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

options::~options()
{
//	debugf("CLOSED");
//	this->stat_file.close();
}


namespace global {
	options* opts;
}


