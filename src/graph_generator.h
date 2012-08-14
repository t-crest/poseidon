#ifndef GRAPH_GENERATOR_H
#define	GRAPH_GENERATOR_H

#include <cassert>
#include <string>
#include <vector>
#include "IOutput.h"
#include "lex_cast.h"
#include "pugixml.hpp"
using namespace std;


class graph_generator {
public:
	graph_generator(int _n, float channel_factor, int bw_min, int bw_max, string type, string _output_dir);
	~graph_generator();
private:
	const int n;			// Height and width of mesh/bitorus
	const int max_channels;		// Number of channels a NoC maximally can have = n*(n-1)
	
	string output_dir;
	char default_file_name [500];
};

#endif	/* GRAPH_GENERATOR_H */
