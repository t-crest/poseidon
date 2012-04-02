#include "draw.hpp"
#include "svg.h"
#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "file.hpp"
#include "draw.hpp"
#include <array>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


void draw_schedule(network_t& n, timeslot p) {
	for (timeslot t = 0; t < p; t++) {
		draw d(n, t);
		snts::file f("t" + ::lex_cast<string>(t) + ".svg", fstream::out);
		f << d.root.toString() << "\n";
	}
}

int main(int argc, char* argv[]) 
{
	parser p("test.xml");
	network_t& n = *(p.n);
	draw d(n);
	
	snts::file f("network.svg", fstream::out);
	f << d.root.toString() << "\n";
	
	n.print_next_table();
	draw_schedule(n, 2/*TODO*/);	
	
	
	
	
	
	
//	for_each(n.links(), [](link_t *l){
//		debugf(*l);
//	});
	
	
//	for (int x = 0; x < n.cols(); x++) 
//	for (int y = 0; y < n.rows(); y++) 
//		if (!n.has({x,y})) continue;
//		else
//			for (int p = 0; p < __NUM_PORTS; p++) {
//				auto oport = n.router({x,y})->out((port_id)p);
//				if (oport.has_link())
//					cout << oport.link() << endl;
//			}

	return 0;
}

