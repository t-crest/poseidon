#ifndef OPTIONS_H
#define	OPTIONS_H

//#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
//#include "file.hpp"
//#include "lex_cast.h"
using namespace std;

class options {
public:
	options(int argc, char *argv[]);
	~options();

	int network_size;
	string output_dir;
	double sigma;
	
private:
    void print_help();
    void print_option(char opt, string text);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
