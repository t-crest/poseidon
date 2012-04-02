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

    enum {SA, GRASP, ALNS, ERR} meta;
	string	input_spec_file;
};

namespace global {
    extern options* opt;
}
#endif	/* OPTIONS_H */