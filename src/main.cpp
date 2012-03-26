#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include <array>
#include <iostream>
#include <cstdlib>
#include <boost/optional/optional.hpp>
using namespace std;

int main(int argc, char* argv[]) {

	network_t n(3,3);
	router_id r = {1,1};
	n.add(r);
	
	debugf(&(n.router(r)));
	debugf(n.router(r).out(E).parent_router());
	
	debugf(n.router(r).addr());
	debugf(n.router(r).out(E).parent_router()->addr());
	assert(&(n.router(r)) == n.router(r).out(E).parent_router());
	
	
	link_t *l = new link_t(n.router(r).out(E), n.router(r).in(E));
	n.router(r).out(E).add_link(l);

	for (int x = 0; x < n.cols(); x++) 
	for (int y = 0; y < n.rows(); y++)
		if (!n.has({x,y}))
			continue;
		else
		{
			for (int p = 0; p < __NUM_PORTS; p++) {
				auto oport = n.router({x,y}).out((port_id)p);
				if (oport.has_link())
					cout << make_pair(x,y) << p << " connects to " << oport.link()->from()->parent_router();
			}
					
		}
	
	return 0;
}

