#ifndef SCHEDULERS_HPP
#define	SCHEDULERS_HPP

#include "maxset.hpp"
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



class singleshot_scheduler {
private:
	float percent;
	int initial;

protected:
//	int curr;
	time_t t0;
	network_t& n;

	void percent_set(const int init);
	void percent_set(const int init, const string);
	void percent_up(const int curr);
	void print_stats_linkutil();
	void print_stats();
	
public:
	singleshot_scheduler(network_t& _n);
	virtual void run() = 0;
	virtual void main_run();
	void verify(const bool best);
};

class s_greedy : public singleshot_scheduler {
	bool random;
public:
	s_greedy(network_t& _n, bool _random);
	void run();
};

/** A cross between greedy and random, chosen by k
 */
class s_cross : public singleshot_scheduler {
	float beta;
public:
	s_cross(network_t& _n, float _beta);
	void run();
};

class s_random : public singleshot_scheduler {
public:
	s_random(network_t& _n);
	void run();
};

class s_bad_random : public singleshot_scheduler {
public:
	s_bad_random(network_t& _n);
	void run();
};


class meta_scheduler : public singleshot_scheduler {
private:
	
	typedef std::pair<const channel*, router_id/*start*/> channel_part;
	
	struct minor_t {
		static network_t* this_n;  // UGLY HACK
		
		bool operator()(channel_part a, channel_part b) {
			assert(a.first == b.first);
			const channel *c = a.first; // does not matter if a or b
			const int a_hops = this_n->router(c->from)->hops[a.second]; 
			const int b_hops = this_n->router(c->from)->hops[b.second]; 
			return (a_hops > b_hops); // prefer a if it has more hops (since a and b are the same channel, a will also include b)
		}
	};

	struct major_t {bool operator()(channel_part a, channel_part b) {
		return (a.first < b.first);
	}};	
	
	typedef maxset<channel_part, minor_t, major_t> chosen_t;

	chosen_t find_depend_path(const channel* dom);
	chosen_t find_depend_rectangle(const channel* c);
	chosen_t find_dom_paths();
	chosen_t find_late_paths(timeslot top);
	
protected:
	std::set< std::pair<int, channel_part> > unrouted_channels;
//	int best;
//	int prev;
	int chosen_adaptive;
	int iterations;

//	#define MEM_FUNC_T std::set<const channel*>(meta_scheduler::*)()
	#define MEM_FUNC_T chosen_t(meta_scheduler::*)()
	std::vector< std::pair<float, MEM_FUNC_T> > choose_table;

	void punish_or_reward();
	void normalize_choose_table();

	chosen_t choose_random();
	chosen_t choose_late_paths();
	chosen_t choose_dom_paths();
	chosen_t choose_dom_rectangle();
	chosen_t choose_dom_crater();

public:
	meta_scheduler(network_t& _n);
	void main_run();
	void destroy();
	void repair();
	void print_stats();
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


singleshot_scheduler* get_heuristic(options::meta_t meta_id, network_t& n);


#endif	/* SCHEDULERS_HPP */

