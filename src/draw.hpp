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
	element draw_link(link_t *l);
	element arrow(float x1, float y1, float x2, float y2);
	std::pair<int,int> coords(const port_t& p);
	
public:
	element root;
	draw(network_t& _n);
};

#endif	/* DRAW_HPP */

