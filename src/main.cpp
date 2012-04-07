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
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;



// // This is 1.7x slower than the recursive solution. This might be due to more indirections and std::stack is slower than the call-stack
//bool route_channel2(network_t& n, const channel* c, timeslot start) 
//{
//	std::stack<link_t*> ls;
//	
//	std::stack< 
//		std::pair<
//			set<port_out_t*>::iterator, 
//			set<port_out_t*>::iterator /*end*/
//		> 
//	> rs;
//
//	rs.push( make_pair(n.router(c->from)->next[c->to].begin(), n.router(c->from)->next[c->to].end()) );
//	assert(n.router(c->from)->next[c->to].begin() != n.router(c->from)->next[c->to].end());
//	timeslot t = start;
//	
//	while (!rs.empty()) 
//	{
//		set<port_out_t*>::iterator& it = rs.top().first;
//		set<port_out_t*>::iterator& end = rs.top().second;
//		assert(it != end);
//				
//		if ((*it)->link().local_schedule.available(t++)) 
//		{
//			router_t& next_router = (*it)->link().sink.parent;
//			const bool dest_reached = (next_router.address == c->to);
//			if (dest_reached) {
//				if (n.router(c->to)->local_out_schedule.available(t))	break;
//				else													return false;
//			}
//
//			set<port_out_t*>& next_ports = next_router.next[c->to];
//			assert(next_ports.begin() != next_ports.end());
//
//			rs.push(	
//				make_pair(
//					next_ports.begin(), 
//					next_ports.end()
//				)    
//			);
//			
//		} else {
//			while (!rs.empty() && (++rs.top().first == rs.top().second)) {
//				rs.pop(); t--;
//			}
//		}
//	}
//
//	if (rs.empty())
//		return false;
//
//
//	n.router(c->to)->local_out_schedule.add(c, t--);
//	while (!rs.empty()) {
//		set<port_out_t*>::iterator& it = rs.top().first;
//		set<port_out_t*>::iterator& end = rs.top().second;
//		assert(it != end);
//
//		(*it)->link().local_schedule.add(c, t--);
//		rs.pop();
//	}
//	
//	return true;
//}
//

void greedy1(network_t& n) 
{
	typedef pair<int, const channel*> pq_t;
	priority_queue< pq_t > pq;

	for_each(n.channels(), [&](const channel & c) {
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	debugf(pq.size());

	
	while (!pq.empty()) 
	{
		pq_t t = pq.top(); pq.pop();
		const channel *c = t.second;
		
		for (timeslot i = 0;; i++) {
			if (n.router(c->from)->local_in_schedule.available(i)) {
				bool okay = n.route_channel(c, c->from, i);
				if (okay) {
					n.router(c->from)->local_in_schedule.add(c, i);
					break;
				}
			}
		}
	}
}

int main(int argc, char* argv[]) 
{
	global::opts = new options(argc, argv);

	parser p(global::opts->input_file);
	network_t& n = *(p.n);
	greedy1(n);

	debugf(n.p());
	
	if (global::opts->draw) {
		draw_network(n);
		draw_schedule(n);
	}

	return 0;
}