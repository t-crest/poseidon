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
#include <array>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


void draw_schedule(network_t& n, timeslot p) {
	for (timeslot t = 0; t < p; t++) {
		draw d(n, t);
		snts::file f("./cartoon/t" + ::lex_cast<string>(t) + ".svg", fstream::out);
		f << d.root.toString() << "\n";
	}
}

bool greedy2(network_t& n, const channel* c, router_id curr, timeslot t) 
{
	if (curr == c->to) return true;
	
	for_each(n.router(curr)->next[c->to], [&](port_out_t *p) {
		link_t& l = p->link();
		
		debugf(curr);
		
		if (l.local_schedule.available(t) && greedy2(n, c, l.sink.parent.address, t+1)) {
			l.local_schedule.add(c, t);
			return true;
		} 
	});
	
	return false;
}

void greedy1(network_t& n) 
{
	typedef pair<int, const channel*> pq_t; 
	priority_queue< pq_t > pq;
	
	for_each(n.channels(), [&](const channel& c){
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	
	while (!pq.empty()) {
		pq_t t = pq.top(); pq.pop();
		debugf(t);
		
		for (timeslot i = 0;; i++) {
			debugf(i);
			bool okay = greedy2(n, t.second, t.second->from, i);
			if (okay) break;
		}
	}
}

int main(int argc, char* argv[]) 
{
	parser p("../data/bitorus3x3.xml");
	network_t& n = *(p.n);
	draw d(n);
	greedy1(n);
	
	snts::file f("network.svg", fstream::out);
	f << d.root.toString() << "\n";
	
//	n.print_next_table();
	debugf(n.p());
	draw_schedule(n, n.p());	
	
	
	
	
	
	
	
	
//	for_each(n.links(), [](link_t *l){
//		debugf(*l);
//	});
	
	
//	for (int x = 0; x < n.cols(); x++) 
//	for (int y = 0; y < n.rows(); y++) 
//		if (!n.has({x,y})) continue;
//		else
//			for (int p = 0; p < __NUM_PORTS; p++) {
//				auto oport = n.router({x,y})->out((port_id)p);
//				if (oport.has_link())
//					cout << oport.link() << endl;
//			}

	return 0;
}

