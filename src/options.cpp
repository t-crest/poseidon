#include "options.h"


options::options(int argc, char *argv[])
// Option defaults:
:	input_file(""),
	metaheuristic(ERR),
	draw(false),
	alns_inital(ERR)
{
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:m:di:")) != -1;) {
		switch (c) {
			case 'm':	metaheuristic = parse_meta_t(string(optarg));	break;
			case 'f':	input_file = optarg;							break;
			case 'd':	draw = true;									break;
			case 'i':	alns_inital = parse_meta_t(string(optarg));		break;
			default:	ensure(false, "Unknown flag " << c << ".");
		}
	}

	/* Some input validation */
	ensure(input_file.size() > 0, "Empty file name given");
	ensure(metaheuristic != ERR, "Metaheuristic must be set to GRASP or ALNS, etc.");
	if (alns_inital != ERR)
		ensure(metaheuristic == ALNS, "ALNS-inital given, but we don't run ALNS");
	if (metaheuristic == ALNS)
		ensure(alns_inital != ERR, "ALNS specified, but no inital solution specified");
}

options::meta_t options::parse_meta_t(string str) 
{
	meta_t ret = ERR;
	if		(str=="RANDOM")	ret=RANDOM;
	else if (str=="GREEDY")	ret=GREEDY;
	else if (str=="rGREEDY")ret=rGREEDY;
	else if (str=="GRASP")	ret=GRASP;		
	else if (str=="ALNS")	ret=ALNS;
	return ret;
}

options::~options()
{ }


namespace global {
	options* opts;
}


