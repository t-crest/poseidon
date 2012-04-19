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
#include <ctime>
#include <stack>
#include <set>
#include <queue>
#include <random>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;




std::function<void(vector<port_out_t*>&) > get_next_mutator();

class scheduler {
private:
	float percent;
	int best_for_print;
	int curr_for_print;
	int print_cnt;
	int initial;

protected:
	network_t& n;
	void print_status();

	void curr_status(const int curr);
	void best_status(const int best);
	void metaheuristic_done();
	void percent_set(const int init, const string);
	void percent_up(const int curr);

public:
	scheduler(network_t& _n);
	virtual void run() = 0;
	void verify(const bool best);
};

class s_greedy : public scheduler {
	bool random;
public:
	s_greedy(network_t& _n, bool _random);
	void run();
};

/** A cross between greedy and random, chosen by k
 */
class s_cross : public scheduler {
	float beta;
public:
	s_cross(network_t& _n, float _beta);
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


class meta_scheduler : public scheduler {
private:
	std::set<const channel*> find_depend_path(const channel* dom);
	std::set<const channel*> find_depend_rectangle(const channel* c);
	std::set<const channel*> find_dom_paths();
	std::set<const channel*> find_late_paths(timeslot top);

	
protected:
	std::set<std::pair<int, const channel*> > unrouted_channels;
	int best;
	int curr;
	int prev;
	int chosen_adaptive;
	int iterations;

	#define MEM_FUNC_T std::set<const channel*>(meta_scheduler::*)()
	std::vector< std::pair<float, MEM_FUNC_T> > choose_table;

	void punish_or_reward();
	void normalize_choose_table();

	std::set<const channel*> choose_random();
	std::set<const channel*> choose_late_paths();
	std::set<const channel*> choose_dom_paths();
	std::set<const channel*> choose_dom_rectangle();

public:
	meta_scheduler(network_t& _n);
	virtual void run() = 0;
	void destroy();
	void repair();
	void print_stats(time_t t0);
	
};

class s_alns : public meta_scheduler {
public:
	s_alns(network_t& _n);
	void run();
};

class s_grasp : public meta_scheduler {
public:
	s_grasp(network_t& _n);
	void run();
};

scheduler* get_heuristic(options::meta_t meta_id, network_t& n);



#endif	/* SCHEDULERS_HPP */

