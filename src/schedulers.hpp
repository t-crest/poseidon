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



std::function<void(vector<port_out_t*>&)> get_next_mutator();




class scheduler {
        float percent;
        int initial;
protected:
	network_t& n;
public:
	scheduler(network_t& _n);
        void percent_set(const int init, const string);
        void percent_up(const int curr);
	virtual void run() = 0;
        void verify();
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

class s_bad_random : public scheduler {
public:
	s_bad_random(network_t& _n);
	void run();
};


class s_lns : public scheduler {
	std::set<std::pair<int, const channel*> > unrouted_channels;
	int best;
	int curr;
	int chosen_adaptive;
	
	#define MEM_FUNC_T std::set<const channel*>(s_lns::*)()
	std::vector< std::pair<float, MEM_FUNC_T> > choose_table;
	
	
public:
	s_lns(network_t& _n);
	void run();
	void destroy();
	void repair();
	
private:
	void punish_or_reward();
	void normalize_choose_table();
	std::set<const channel*> choose_random();
	std::set<const channel*> choose_dom_and_depends();
	std::set<const channel*> depend_path(const channel* dom);
    std::set<const channel*> depend_rectangle(const channel* c);
};



#endif	/* SCHEDULERS_HPP */

