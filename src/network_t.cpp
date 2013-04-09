
#include "network_t.hpp"

using namespace std;

/**
 * The constructor of the network data structure.
 * @param rows The number of rows in the network topology.
 * @param cols The number of columns in the network topology.
 */
network_t::network_t(uint rows, uint cols) : m_routers(rows, cols) 
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
	return ret;
}

/**
 * 
 * @param r 
 * @return 
 */
bool network_t::has(const router_id r) const {
	if (!(0 <= r.first && r.first < this->rows())) return false;
	if (!(0 <= r.second && r.second < this->cols())) return false;
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

const vector<link_t*>& network_t::links() const {
	return this->link_ts;
}

const vector<router_t*>& network_t::routers() const {
	return this->router_ts;
}

const vector<channel>& network_t::channels() const {
	return this->specification;
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

	while (!Q.empty()) {
		router_t *t = Q.front(); Q.pop();

//		cout << "Router " << t->address << " is " << hops[t] << " hops away from "<< dest->address << endl;

		t->hops[dest->address] = hops[t];
		
		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!t->out((port_id) i).has_link()) continue;
			router_t *c = &t->out((port_id) i).link().sink.parent;
			if (!util::contains(hops, c)) continue;

			if (hops[c] == hops[t] - 1) {
//				assert( ! util::contains(t->next[dest->address], &t->out((port_id) i)));
				t->next[dest->address].push_back( &t->out((port_id) i) );
			}
		}

		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!t->in((port_id) i).has_link()) continue;
			router_t *o = &t->in((port_id) i).link().source.parent; // visit backwards

			if (!marked[o]) {
				marked[o] = true;
				hops[o] = hops[t] + 1;
				Q.push(o);
			}
		}
	}
}

void network_t::shortest_path() {
	for_each(this->routers(), [this](router_t * r) {
		this->shortest_path_bfs(r);
	});
}

void network_t::print_next_table() {
	for_each(this->routers(), [&](router_t * r) {
		for_each(r->next, [&](const pair<router_id, vector<port_out_t*> >& p) {
			for_each(p.second, [&](port_out_t * o) {
				cout << "From router " << r->address << " take " << o->corner << " towards " << p.first << endl;
			});
		});
	});
}

void network_t::print_channel_specification() {
	for_each(this->channels(), [&](const channel& c){
		cout << "Network has channel " << c << endl;
	});
}
 /**
  * 
  * @return The number of link in the topology
  */
timeslot network_t::links_in_topology() {
	return this->link_ts.size();
}

/**
 * Count the number of link timeslots on which a communication channel is routed.
 * @param best
 * @return 
 */
timeslot network_t::occupied_links(bool best) {
	timeslot links_used = 0;
	
	for_each(this->link_ts, [&](link_t *l){
		schedule* sched = (best ? &l->best_schedule : &l->local_schedule);
		links_used += sched->table.size();
	});
	
	return links_used;
}

/**
 * This function calculates the utilization of links in a schedule.
 * @param best If true the function uses the best observed schedule, otherwise the current schedule is used.
 * @return The calcualted link utilization as a float value between 1 and 0.
 */
float network_t::link_utilization(bool best) {

	const timeslot length = (best ? this->p_best() : this->p());
	const timeslot max_links_used = links_in_topology() * length;
	timeslot links_used = occupied_links(best);
	
	assert(links_used <= max_links_used);
	return float(links_used)/max_links_used;
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
	const bool already = this->router(c->from)->local_in_schedule.is(t, c);
	const bool available = this->router(c->from)->local_in_schedule.available(t);
	
	if (!already && !available)
		return false;

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
		const bool already = this->router(c->to)->local_out_schedule.is(t, c);
		const bool available = this->router(c->to)->local_out_schedule.available(t);
		
		if (available || already) {
			this->router(c->to)->local_out_schedule.add(c, t);
			return true;
		} else {
			return false;
		}
	}

	auto& next = this->router(curr)->next[c->to];
	next_mutator(next);
	assert(1 <= next.size());
	assert(next.size() <= 4);

	for (auto it = next.begin(); it != next.end(); ++it) {
		port_out_t *p = *it;
				
		assert(p->has_link());
		link_t& l = p->link();

		const bool already = l.local_schedule.is(t, c);
		const bool available = l.local_schedule.available(t);
		if (!already && !available)
			continue;
				
		if (this->route_channel(c, l.sink.parent.address, t+1, next_mutator)) {
			l.local_schedule.add(c, t);
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
	if(!this->router(c->from)->local_in_schedule.is(c->t_start,c)){
		ensure(false,"Ripup failed: Channel: " << *c << " is not routed on local in schedule at time slot: " << c->t_start << ".");
	}
	this->router(c->from)->local_in_schedule.remove(c->t_start);

	bool modify = true;	// flag indicating if we delete links from now on, or just follow them until we have found start
	router_id curr = c->from;
	const router_id dest = c->to;
	timeslot t = c->t_start;
	
	assert(c->from != c->to);
	
	while (curr != dest) {
		if (curr == start) modify = true; // we found the start, let's begin to delete links
		
		port_out_t *p = NULL;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!this->router(curr)->out((port_id)i).has_link())
				continue;

			if (this->router(curr)->out((port_id)i).link().local_schedule.is(t,c)) {
				p = &this->router(curr)->out((port_id)i);
				break;
			}
		}
		assert(p != NULL);

		ensure(p->link().local_schedule.is(t,c),"Ripup failed: Channel: " << *c << " is not routed on local schedule of link " << p->link() << ".")
		if (modify)
			p->link().local_schedule.remove(t);
		t++;
		curr = p->link().sink.parent.address;
	}

	assert(curr == c->to);
	
	if(!this->router(c->to)->local_out_schedule.is(t,c)){
		ensure(false,"Ripup failed: Channel: " << *c << " is not routed on local out schedule at time slot " << t << ".");
	}
	this->router(c->to)->local_out_schedule.remove(t);
	return;
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
	
	if (from_in_schedule->is(t_curr, c) == false) {	
		debugf(best);
//		debugf(t_curr);
//		debugf(*from_in_schedule->get(t_curr));
//		debugf(from_in_schedule->get(t_curr)->t_start);
//		if (from_in_schedule->has(6)) debugf(*from_in_schedule->get(6));
//		if (from_in_schedule->has(7)) debugf(*from_in_schedule->get(7));
//		if (from_in_schedule->has(8)) debugf(*from_in_schedule->get(8));
//		debugf(*c);
//		
//		debugf(*c);
//		debugf(c);
//		debugf(t_curr);
//		debugf(from_in_schedule->table);
//
//		
//		if (from_in_schedule->has(t_curr)){
//			assert(from_in_schedule->get(t_curr)->t_start == t_curr);
//		}
		ensure(false, "EPIC faliure: Channel " << *c << " is not routed to the local in port of " << curr << ".");
	}
	
	while (curr != dest)
	{
		int count = 0;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if(!this->router(curr)->out((port_id)i).has_link()) continue;
			
			link_t& l = this->router(curr)->out((port_id)i).link();
			if(best){
				link_schedule = &l.best_schedule;
			} else {
				link_schedule = &l.local_schedule;
			}
			
			if (link_schedule->is(t_curr,c)) {
				bool is_shortest = false;
				for_each(this->router(curr)->next[dest], [&](const port_out_t* p){
					if(p == &this->router(curr)->out((port_id)i))
						is_shortest = true;
				});
				ensure(is_shortest,"EPIC failure: Channel " << *c << " not routed on shortest path.");
				next_curr = l.sink.parent.address;
				count++;
			}	
		}
		curr = next_curr;
		t_curr++;
		ensure(count == 1,"EPIC failure: Count: " << count << " Current: " << curr << " has non or multiple output ports for channel " << *c << ".");
	}
	
	if(!to_out_schedule->is(t_curr,c))
		ensure(false,"EPIC faliure: Channel " << *c << " is not routed to the local out port of " << curr << ".");
	
	return;
}

