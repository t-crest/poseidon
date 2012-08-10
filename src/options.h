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
	string input_file;
        string output_dir;
	bool draw;
	meta_t meta_inital;
	bool save_best;
	time_t run_for;
	ofstream stat_file;
	float beta_percent;
	
private:
	meta_t parse_meta_t(string str);
        void print_help();
        void print_option(char opt, string text);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
