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

#include "network_t.hpp"

using namespace std;

namespace snts {
/**
 * The constructor of the network data structure.
 * @param rows The number of rows in the network topology.
 * @param cols The number of columns in the network topology.
 */
network_t::network_t(uint rows, uint cols, uint router_depth, int availavble_timeslots) : m_routers(rows, cols) , router_depth(router_depth), available_timeslots(available_timeslots)
{
	this->curr = this->best = this->prev = ::numeric_limits<int>::max();
}

/**
 *
 * @return Returns the number of rows in the network topology.
 */
uint network_t::rows() const {
	return this->m_routers.rows();
}

/**
 *
 * @return Returns the number of columns in the network topology.
 */
uint network_t::cols() const {
	return this->m_routers.cols();
}

/**
 *
 * @return Returns the period of the current schedule.
 */
timeslot network_t::p() const {
	timeslot ret = 0;
	for_each(this->links(), [&](link_t *l){
		ret = util::max(ret, l->local_schedule.max_time()+1); // +1 because we want the number of time slots not the maximum time
	});
	return ret;
}
 /**
  *
  * @return Returns the period of the best observed schedule.
  */
timeslot network_t::p_best() const {
	timeslot ret = 0;
	for_each(this->links(), [&](link_t *l){
		ret = util::max(ret, l->best_schedule.max_time()+1); // +1 because we want the number of time slots not the maximum time
	});
	//
//	for_each(this->routers(), [&](router_t *t){
//		ret = util::max(ret, t->local_out_best_schedule.max_time()+1);
//	});
	return ret;
}

/**
 *
 * @param r
 * @return
 */
bool network_t::has(const router_id r) const {
	if (!(0 <= r.first && r.first < this->cols())) return false;
	if (!(0 <= r.second && r.second < this->rows())) return false;
	return (this->m_routers(r) != NULL);
}

link_t* network_t::add(port_out_t& source, port_in_t& sink) {
	link_t *l = new link_t(source, sink);
	this->link_ts.push_back(l);
	return l;
}

router_t* network_t::add(router_id r) {
	ensure(0 <= r.first && r.first <= this->cols() - 1, "Tried to add router " << r << " outside network");
	ensure(0 <= r.second && r.second <= this->rows() - 1, "Tried to add router " << r << " outside network");

	if (this->m_routers(r) == NULL) { // create only new router if it didn't exist before
		router_t *rr = new router_t(r);
		this->m_routers(r) = rr;
		this->router_ts.push_back(rr);
		assert(this->router(r) == &this->router(r)->out(N).parent);
		assert(this->router(r) == &this->router(r)->out(L).parent);
	}
	return this->m_routers(r);
}

router_t* network_t::router(router_id r) {
	return this->m_routers(r);
}

router_t* network_t::router(router_id r) const {
	return this->m_routers(r);
}

const vector<link_t*>& network_t::links() const {
	return this->link_ts;
}

const vector<router_t*>& network_t::routers() const {
	return this->router_ts;
}

const vector<channel>& network_t::channels() const {
	return this->specification;
}

const timeslot network_t::get_router_depth() const {
	return this->router_depth;
}

/**
 * Traverse the topology graph backwards from destination via BFS.
 * During BFS the depth of nodes is stored as the the number of hops.
 * This updates the .next map member variable of all nodes connected to dest.
 */
void network_t::shortest_path_bfs(router_t *dest) {
	std::queue<router_t*> Q; // the BFS queue

	// We don't want to pollute our router_t's class with ad-hoc members,
	// instead we use std::maps to associate these properties
	std::map<router_t*, bool> marked;
	std::map<router_t*, int> hops;

	Q.push(dest);
	marked[dest] = true;
	hops[dest] = router_depth;

	while (!Q.empty()) {
		router_t *t = Q.front(); Q.pop();

//		cout << "Router " << t->address << " is " << hops[t] << " hops away from "<< dest->address << endl;

		t->hops[dest->address] = hops[t]; // If hops does not contain t an element with key t is inserted with an empty value.

		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!t->out((port_id) i).has_link()) continue; // If out port has no link continue.
			router_t *c = &t->out((port_id) i).link().sink.parent; // The router at the end of the outgoing link.
			if (!util::contains(hops, c)) continue; // If hops does not contain the router at the end of the outgoing link continue;

			int link_depth = t->out((port_id) i).link().depth;
			if (hops[c] == hops[t] - router_depth - link_depth) {
//				assert( ! util::contains(t->next[dest->address], &t->out((port_id) i)));
				t->next[dest->address].push_back( &t->out((port_id) i) );
			}
		}

		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!t->in((port_id) i).has_link()) continue; // If in port has no link continue
			router_t *o = &t->in((port_id) i).link().source.parent; // visit backwards


			if (!marked[o]) {
				marked[o] = true;
				int link_depth = t->in((port_id) i).link().depth;
				hops[o] = hops[t] + router_depth + link_depth;
				Q.push(o);
			}
		}
//		debugf(t->address);
//		for(auto it = t->next.begin(); it != t->next.end(); it++){
//			for(int i = 0; i < (*it).second.size(); i++){
//				std::cout << *((*it).second.at(i)) << std::endl;
//			}
//			std::cout << std::endl;
//		}
////		for_each(t->next, [&](std::vector<port_out_t*> v){
////			std::cout << v << std::endl;
////		});
//		debugf(t->next);
	}
}

void network_t::shortest_path() {
	for_each(this->routers(), [this](router_t * r) {
		this->shortest_path_bfs(r);
	});
}

void network_t::print_next_table() const {
	for_each(this->routers(), [&](router_t * r) {
		for_each(r->next, [&](const pair<router_id, vector<port_out_t*> >& p) {
			for_each(p.second, [&](port_out_t * o) {
				cout << "From router " << r->address << " take " << o->corner << " towards " << p.first << endl;
			});
		});
	});
}

void network_t::print_channel_specification() const {
	for_each(this->channels(), [&](const channel& c){
		cout << "Network has channel " << c << endl;
	});
}
 /**
  *
  * @return The number of link in the topology
  */
int network_t::links_in_topology() const {
	return this->link_ts.size();
}

/**
 * Count the number of link timeslots on which a communication channel is routed.
 * @param best
 * @return
 */
timeslot network_t::occupied_links(bool best) const {
	timeslot links_used = 0;

	for_each(this->link_ts, [&](link_t *l){
		schedule* sched = (best ? &l->best_schedule : &l->local_schedule);
		links_used += sched->table.size();
	});

	return links_used;
}


int network_t::io_activity(router_t* r) const {
	int in_activity = r->local_in_best_schedule.table.size();
	int out_activity = r->local_out_best_schedule.table.size();
	return (in_activity > out_activity) ? in_activity : out_activity;
}

int network_t::max_io_activity() const {
	int max_io = 0;
	auto r = this->routers();
	for(int i = 0; i < r.size(); i++){
		int new_io = io_activity(r.at(i));
		if ( new_io > max_io ){
			max_io = new_io;
		}
	}
	return max_io;
}

void network_t::updatebest() {
	if (global::opts->save_best == false) return;

	this->curr = this->p();

	if (this->curr < this->best) {
		this->best = this->curr;

		for_each(this->links(), [&](link_t* l){
			l->updatebest();
		});

		for_each(this->routers(), [&](router_t* r) {
			r->updatebest();
		});

		for_each(this->specification, [&](channel& c){
			c.t_best_start = c.t_start;
		});

	}

}

void network_t::clear() {
	for_each(this->router_ts, [&](router_t *r){
		r->local_in_schedule.clear();
		r->local_out_schedule.clear();
	});

	for_each(this->link_ts, [&](link_t *l){
		l->local_schedule.clear();
	});
}


bool network_t::route_channel_wrapper(
	channel* c, timeslot t,
	std::function<void(vector<port_out_t*>&)> next_mutator
)
{
	const bool already = this->router(c->from)->local_in_schedule.is(c, t);
	const bool available = this->router(c->from)->local_in_schedule.available(t,c->phits);

	if (!available || already){
	//if (!already && !available){
		return false;
	}

	const bool path_routed = this->route_channel(c, c->from, t, next_mutator);
	if (path_routed) {
		this->router(c->from)->local_in_schedule.add(c, t);
		c->t_start = t;
		return true;
	}
	return false;
}

bool network_t::route_channel(
	channel* c, router_id curr, timeslot t,
	std::function<void(vector<port_out_t*>&)> next_mutator
)
{
	const bool dest_reached = (curr == c->to);

	if (dest_reached) {
		const bool already = this->router(c->to)->local_out_schedule.is(c, t + this->router_depth);
		const bool available = this->router(c->to)->local_out_schedule.available(t + this->router_depth, c->phits);

		if (available || already) {
			this->router(c->to)->local_out_schedule.add(c, t + this->router_depth);
			return true;
		} else {
			return false;
		}
	}

	auto& next = this->router(curr)->next[c->to];
	ensure(1 <= next.size(),"Nowhere to route packet. Next.size() = " << next.size());
	next_mutator(next);
	ensure(1 <= next.size(),"Nowhere to route packet. Next.size() = " << next.size());
	assert(1 <= next.size());
	assert(next.size() <= 4);

	for (auto it = next.begin(); it != next.end(); ++it) {
		port_out_t *p = *it;

		assert(p->has_link());
		link_t& l = p->link();
		const timeslot outtime = t + this->router_depth + l.depth;

		const bool already = l.local_schedule.is(c, outtime);
		const bool available = l.local_schedule.available(outtime,c->phits);
		if (!already && !available)
			continue;

		if (this->route_channel(c, l.sink.parent.address, outtime, next_mutator)) {
			l.local_schedule.add(c, outtime);
			return true;
		}
	}

	return false;
}

/* Rips up channel c, starting from start (inclusive).
 * This means links (including local ones) on channel c from start to c->to are removed
 */
void network_t::ripup_channel(const channel* c, router_id start)
{
//	if (c->from != start) {
//		debugf(c->from);
//		debugf(c->to);
//		debugf(start);
//	}
	if(!this->router(c->from)->local_in_schedule.is(c, c->t_start)){
		ensure(false,"Ripup failed: Channel: " << *c << " is not routed on local in schedule at time slot: " << c->t_start << ".");
	}
	this->router(c->from)->local_in_schedule.remove(c, c->t_start);

	router_id curr = c->from;
	const router_id dest = c->to;
	timeslot t = c->t_start;

	assert(c->from != c->to);

	while (curr != dest) {
		port_out_t *p = NULL;
		link_t *l = NULL;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!this->router(curr)->out((port_id)i).has_link())
				continue;

			l = &this->router(curr)->out((port_id)i).link();
			if (l->local_schedule.is(c, t + this->router_depth + l->depth)) {
				p = &this->router(curr)->out((port_id)i);
				break;
			}
		}
		assert(p != NULL);

		ensure(p->link().local_schedule.is(c, t + this->router_depth + l->depth),"Ripup failed: Channel: " << *c << " is not routed on local schedule of link " << p->link() << ".")
		p->link().local_schedule.remove(c, t + this->router_depth + l->depth);
		t = t + this->router_depth + l->depth;
		curr = p->link().sink.parent.address;
	}

	assert(curr == c->to);

	if(!this->router(c->to)->local_out_schedule.is(c, t + this->router_depth)){
		ensure(false,"Ripup failed: Channel: " << *c << " is not routed on local out schedule at time slot " << t << ".");
	}
	this->router(c->to)->local_out_schedule.remove(c, t + this->router_depth);
	return;
}


string network_t::get_route(const channel* c) const
{
	if (c == NULL) {return "";}
	string route = "";
	router_id curr = c->from;
	router_id next_curr = c->from;
	const router_id dest = c->to;
	timeslot t_curr = c->t_best_start;
	schedule* from_in_schedule = &this->router(c->from)->local_in_best_schedule;;
	schedule* to_out_schedule = &this->router(c->to)->local_out_best_schedule;
	schedule* link_schedule;

	if (!from_in_schedule->is(c, t_curr)) {
		ensure(false, "EPIC faliure: Channel " << *c << " is not routed to the local in port of " << curr << ".");
	}

	while (curr != dest)
	{
		int count = 0;
		timeslot next_t_curr = t_curr;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if(!this->router(curr)->out((port_id)i).has_link()) continue;

			link_t& l = this->router(curr)->out((port_id)i).link();
			link_schedule = &l.best_schedule;

			if (link_schedule->is(c, t_curr + this->router_depth + l.depth)) {
				route += p2c((const port_id)i);
				next_curr = l.sink.parent.address;
				count++;
				next_t_curr = t_curr + this->router_depth + l.depth;
			}
		}
		curr = next_curr;
		t_curr = next_t_curr;
		ensure(count == 1,"EPIC failure: Count: " << count << " Current: " <<
				curr << " has non or multiple output ports for channel " << *c << ".");
	}

	if(!to_out_schedule->is(c, t_curr + this->router_depth))
		ensure(false,"EPIC faliure: Channel " << *c << " is not routed to the local out port of " << curr << ".");

	route += 'L';
	return route;
}

void network_t::check_channel(const channel* c, const bool best)
{
	router_id curr = c->from;
	router_id next_curr = c->from;
	const router_id dest = c->to;
	timeslot t_curr;
	schedule* from_in_schedule;
	schedule* to_out_schedule;
	schedule* link_schedule;

	if (best) {
		t_curr = c->t_best_start;
		from_in_schedule = &this->router(c->from)->local_in_best_schedule;
		to_out_schedule = &this->router(c->to)->local_out_best_schedule;
	} else {
		t_curr = c->t_start;
		from_in_schedule = &this->router(c->from)->local_in_schedule;
		to_out_schedule = &this->router(c->to)->local_out_schedule;
	}

	if (!from_in_schedule->is(c, t_curr)) {
		debugf(best);
//		if (from_in_schedule->has(t_curr)){
//			assert(from_in_schedule->get(t_curr)->t_start == t_curr);
//		}
		ensure(false, "EPIC faliure: Channel " << *c << " is not routed to the local in port of " << curr << ".");
	}

	while (curr != dest)
	{
		int count = 0;
		timeslot next_t_curr = t_curr;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if(!this->router(curr)->out((port_id)i).has_link()) continue;

			link_t& l = this->router(curr)->out((port_id)i).link();
			if(best){
				link_schedule = &l.best_schedule;
			} else {
				link_schedule = &l.local_schedule;
			}

			if (link_schedule->is(c, t_curr + this->router_depth + l.depth)) {
				bool is_shortest = false;
				for_each(this->router(curr)->next[dest], [&](const port_out_t* p){
					if(p == &this->router(curr)->out((port_id)i))
						is_shortest = true;
				});
				ensure(is_shortest,"EPIC failure: Channel " << *c << " not routed on shortest path.");
				next_curr = l.sink.parent.address;
				count++;
				next_t_curr = t_curr + this->router_depth + l.depth;
			}
		}
		curr = next_curr;
		t_curr = next_t_curr;
		ensure(count == 1,"EPIC failure: Count: " << count << " Current: " <<
				curr << " has non or multiple output ports for channel " << *c << ".");
	}

	if(!to_out_schedule->is(c, t_curr + this->router_depth))
		ensure(false,"EPIC faliure: Channel " << *c << " is not routed to the local out port of " << curr << ".");

	return;
}

}
