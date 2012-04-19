#ifndef OPTIONS_H
#define	OPTIONS_H

#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "file.hpp"
#include "lex_cast.h"
using namespace std;


string get_stat_name(int argc, char *argv[]);


class options {
public:
	options(int argc, char *argv[]);
	~options();

	enum meta_t {GREEDY, rGREEDY, CROSS, RANDOM, BAD_RANDOM, GRASP, ALNS, ERR};
	meta_t metaheuristic;
	string	input_file;
	bool draw;
	meta_t meta_inital;
	bool save_best;
	time_t run_for;
	ofstream stat_file;
	
private:
	meta_t parse_meta_t(string str);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
