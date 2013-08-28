/*******************************************************************************
 * Copyright 2012 Mark Ruvald Pedersen <mark@radix63.dk>
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2012 Jaspur Hoejgaard <jaspurh@gmail.com>
 * Copyright 2013 Technical University of Denmark, DTU Compute.
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 ******************************************************************************/
 
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
#include "network_t.hpp"
#include "stats.hpp"
#include <array>
#include <ctime>
#include <stack>
#include <set>
#include <queue>
#include <random>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <algorithm>

namespace snts {

std::function<void(std::vector<port_out_t*>&) > get_next_mutator();


class singleshot_scheduler {

protected:
	time_t t0;
	network_t& n;
	stats* b;
	
public:
	singleshot_scheduler(network_t& _n, stats* _b);
	virtual void run() = 0;
	virtual void main_run();
	void verify(const bool best);
};

class s_greedy : public singleshot_scheduler {
	bool random;
public:
	s_greedy(network_t& _n, stats* _b, bool _random);
	void run();
};

/** A cross between greedy and random, chosen by k
 */
class s_cross : public singleshot_scheduler {
	float beta;
public:
	s_cross(network_t& _n, stats* _b, float _beta);
	void run();
};

class s_random : public singleshot_scheduler {
public:
	s_random(network_t& _n, stats* _b);
	void run();
};

class s_bad_random : public singleshot_scheduler {
public:
	s_bad_random(network_t& _n, stats* _b);
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
		return (a.first < b.first); // compare channels
	}};	
	
	typedef maxset<channel_part, minor_t, major_t> chosen_t;

	chosen_t find_depend_path(const channel* dom);
	chosen_t find_depend_rectangle(const channel* c);
	chosen_t find_dom_paths();
	chosen_t find_late_paths(timeslot top);
	void crater_process(const router_id r0, const timeslot t0, chosen_t& ret);
	
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
	meta_scheduler(network_t& _n, stats* _b);
	void main_run();
	void destroy();
	void repair();
};

class s_alns : public meta_scheduler {
public:
	s_alns(network_t& _n, stats* _b);
	void run();
};

class s_grasp : public meta_scheduler {
public:
	s_grasp(network_t& _n, stats* _b);
	void run();
};


singleshot_scheduler* get_heuristic(options::meta_t meta_id, network_t& n, stats* b);

}

#endif	/* SCHEDULERS_HPP */

