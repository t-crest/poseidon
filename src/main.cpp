#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


int main(int argc, char* argv[]) 
{
	parser p("test.xml");
	network_t& n = *(p.n);
		
	for (int x = 0; x < n.cols(); x++) 
	for (int y = 0; y < n.rows(); y++) 
		if (!n.has({x,y})) continue;
		else
			for (int p = 0; p < __NUM_PORTS; p++) {
				auto oport = n.router({x,y})->out((port_id)p);
				if (oport.has_link())
					cout << make_pair(x,y) << p << " connects to " << oport.link()->sink.parent.address << endl;
			}

	return 0;
}

