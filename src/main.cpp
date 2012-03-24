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

int main(int argc, char** argv) {

	matrix<int> m(3, 4);
	m(0, 0) = 2;
	m(1, 1) = 1;
	debugf(m);

//	schedule s(3, 3);
//	s.add_directed_link(make_pair(0, 0), E, make_pair(1, 0));
//
//	
//	network n;
//	n.router(2,2)->port(E).out()->
//	
	
	return 0;
}

