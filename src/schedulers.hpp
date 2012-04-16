#ifndef SCHEDULERS_HPP
#define	SCHEDULERS_HPP

#include "output.hpp"
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
#include "options.h"
#include <array>
#include <stack>
#include <set>
#include <queue>
#include <random>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;




class scheduler {
protected:
	network_t& n;
public:
	scheduler(network_t& _n);
	virtual void run() = 0;
};


class s_greedy : public scheduler {
	bool random;
public:
	s_greedy(network_t& _n, bool _random);
	void run();
};


class s_random : public scheduler {
public:
	s_random(network_t& _n);
	void run();
};


class s_lns : public scheduler {
	std::set<std::pair<int, const channel*> > unrouted_channels;
public:
	s_lns(network_t& _n);
	void run();
//	void destroy(router_id r, timeslot t); // destroys all channels going through r at t
//	void destroy(timeslot t); // destroys all channels which are routed at t
	void destroy();
	void repair();
	
	std::set<const channel*> choose_random();
	std::set<const channel*> choose_dom_and_depends();
	std::set<const channel*> depend_path(const channel* dom);
    std::set<const channel*> depend_rectangle(const channel* c);
};



#endif	/* SCHEDULERS_HPP */

