#include "options.h"


options::options(int argc, char *argv[])
// Option defaults:
:	input_file(""),
	metaheuristic(ERR),
	draw(false)
{
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:m:d")) != -1;) {
		switch (c) {
			case 'm':	if		(string(optarg)=="SA")		metaheuristic=SA;		// -m SA gives simulated annealing
						else if (string(optarg)=="RANDOM")	metaheuristic=RANDOM;
						else if (string(optarg)=="GREEDY")	metaheuristic=GREEDY;
						else if (string(optarg)=="rGREEDY")	metaheuristic=rGREEDY;
						else if (string(optarg)=="GRASP")	metaheuristic=GRASP;	// -m GRASP gives GRASP
						else if (string(optarg)=="ALNS")	metaheuristic=ALNS;		// -m ALNS gives ALNS
						else metaheuristic=ERR;				break;
			case 'f':	input_file = optarg;				break;
			case 'd':	draw = true;						break;
		}
	}

	/* Some input validation */
	ensure(input_file.size() > 0, "Empty file name given");
	ensure(metaheuristic != ERR, "Metaheuristic must be set to sa (Simulated Annealing), GRASP or ALNS");
}

options::~options()
{ }


namespace global {
	options* opts;
}


