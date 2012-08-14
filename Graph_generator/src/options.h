#ifndef OPTIONS_H
#define	OPTIONS_H

#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "lex_cast.h"
using namespace std;

class options {
public:
	options(int argc, char *argv[]);
	~options();
    
    string output_dir;
private:
        void print_help();
        void print_option(char opt, string text);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
