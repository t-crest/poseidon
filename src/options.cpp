#include "options.h"


options::options(int argc, char *argv[])
:	input_spec_file("../data/bitorus3x3.xml"),		// Options defaults
	meta(ALNS)
{
	/* Set options as specified by user */
	for (int c; (c = getopt(argc, argv, "f:M")) != -1;) {
		switch (c) {
			case 'M':	if		(string(optarg)=="sa") meta=SA; // -M sa gives simulated annealing
						else if (string(optarg)=="grasp") meta=GRASP; // -M grasp gives GRASP
						else if (string(optarg)=="alns") meta=ALNS; // -M alns gives ALNS
						else meta=ERR;							break;
			case 'f':	input_spec_file = optarg;				break;
		}
	}

	/* Some input validation */
	ensure(input_spec_file.size() > 0, "Empty file name given");
	ensure(meta != ERR, "Metaheuristic must be set to sa (Simulated Annealing), GRASP or ALNS");
}

options::~options()
{ }
