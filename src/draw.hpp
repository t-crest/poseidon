#ifndef DRAW_HPP
#define	DRAW_HPP

#include "svg.h"
#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "file.hpp"
#include <boost/format.hpp>
#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


class draw {
	const int scale;
	const int bezel;
	const int router_size; // width and height of a router

	network_t& n;	
	element link(link_t *l);
	string arrow_head(const double angle);
	element arrow_straight(const float x1, const float y1, const float x2, const float y2);
	element arrow_wrapped(const float x1, const float y1, const float x2, const float y2);
	std::pair<int,int> coords(const port_t& p);

	
public:
	element root;
	draw(network_t& _n);
};

#endif	/* DRAW_HPP */

