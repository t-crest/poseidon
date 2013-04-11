#ifndef DRAW_HPP
#define	DRAW_HPP

#include "svg.h"
#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "network_t.hpp"
#include "file.hpp"
#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

namespace snts {

class draw {
	const int scale;
	const int bezel;
	const int router_size; // width and height of a router
	boost::optional<timeslot> t;
	const snts::network_t& n;	
	
	void init();
	element link(link_t *l);
	std::string arrow_head(const double angle);
	element arrow_straight(const float x1, const float y1, const float x2, const float y2, const std::string color);
	element arrow_wrapped(const float x1, const float y1, const float x2, const float y2, const std::string color);
	std::pair<int,int> coords(const port_t& p);
	uint32_t hsv(float h, float s, float v);
	
public:
	element root;
	draw(const snts::network_t& _n);
	draw(const snts::network_t& _n, timeslot _t);
};

}

#endif	/* DRAW_HPP */

