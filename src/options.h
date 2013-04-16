#ifndef OPTIONS_H
#define	OPTIONS_H

#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <getopt.h>
#include "file.hpp"
#include "lex_cast.h"

std::string get_stat_name(int argc, char *argv[]);


class options {
public:
	options(int argc, char *argv[]);
	~options();

	enum meta_t {GREEDY, rGREEDY, CROSS, RANDOM, BAD_RANDOM, GRASP, ALNS, ERR};
	meta_t metaheuristic;
	std::string input_platform;
	std::string input_com;
	std::string output_file;
	bool draw;
	bool cal_stats;
	meta_t meta_inital;
	bool save_best;
	time_t run_for;
	std::ofstream stat_file;
	float beta_percent;
	
private:
	meta_t parse_meta_t(std::string str);
	meta_t parse_init_t(std::string str);
    void print_help();
    void print_option(char opt, std::string text);
	void print_option(char opt, std::string long_opt, std::string text);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
