#ifndef OPTIONS_H
#define	OPTIONS_H

#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include "lex_cast.h"
using namespace std;

class options {
public:
	options(int argc, char *argv[]);
	~options();

	enum meta_t {GREEDY, rGREEDY, RANDOM, SA, GRASP, ALNS, ERR} metaheuristic;
	string	input_file;
	bool draw;
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
