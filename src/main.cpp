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
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;



bool greedy2(network_t& n, const channel* c, router_id curr, timeslot t) 
{
	if (curr == c->to && n.router(c->to)->local_out_schedule.available(t)) {
		n.router(c->to)->local_out_schedule.add(c, t);
		return true;
	}

	auto next = n.router(curr)->next[c->to];
	for (auto it = next.begin(); it != next.end(); ++it) {
		port_out_t *p = *it;
		link_t& l = p->link();

		if (l.local_schedule.available(t) && greedy2(n, c, l.sink.parent.address, t + 1)) {
			l.local_schedule.add(c, t);
			return true;
		}
	}

	return false;
}

void greedy1(network_t& n) {
	typedef pair<int, const channel*> pq_t;
	priority_queue< pq_t > pq;

	for_each(n.channels(), [&](const channel & c) {
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	debugf(pq.size());

	while (!pq.empty()) {
		pq_t t = pq.top();
		pq.pop();

		for (timeslot i = 0;; i++) {
			if (n.router(t.second->from)->local_in_schedule.available(i)) {
				bool okay = greedy2(n, t.second, t.second->from, i);
				if (okay) {
					n.router(t.second->from)->local_in_schedule.add(t.second, i);
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

	if (global::opts->draw) {
		draw_network(n);
		draw_schedule(n);
	}

	return 0;
}