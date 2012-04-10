#include "schedule.hpp"

router_id operator-(const router_id& lhs, const router_id& rhs) {
	router_id ret = {lhs.first - rhs.first, lhs.second - rhs.second};
	return ret;
}

router_id abs(const router_id& arg) {
	router_id ret = {abs(arg.first), abs(arg.second)};
	return ret;
}

std::ostream& operator<<(std::ostream& stream, const port_id& rhs) {
	static const std::map<port_id, char> m = {
		{N, 'N'},
		{S, 'S'},
		{E, 'E'},
		{W, 'W'},
		{L, 'L'}};
	stream << m.at(rhs);
	return stream;
}

std::istream& operator>>(std::istream& stream, port_id& rhs) {
	static const std::map<char, port_id> m = {
		{'N', N},
		{'S', S},
		{'E', E},
		{'W', W},
		{'L', L}};
	char c;
	stream >> c;
	rhs = m.at(c);
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const routerport_id& rhs) {
	stream << rhs.first << rhs.second;
	return stream;
}

std::istream& operator>>(std::istream& stream, routerport_id& rhs) {
	stream >> rhs.first >> rhs.second;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const channel& rhs) {
    stream << "[from=" << rhs.from << ", to=" << rhs.to << ", bandwith=" << rhs.bandwidth << "]";
    return stream;
}

////////////////////////////////////////////////////////////////////////////////

schedule::schedule() 
#ifdef USE_SCHEDULE_HASHMAP
:	max(0) 
#endif
{}

#ifdef USE_SCHEDULE_HASHMAP
void schedule::refresh_max() {
	timeslot t = 0;
	for (auto it = this->table.cbegin(); it != this->table.cend(); ++it)
		t = util::max(t, it->first);
	this->max = t;
}
#endif

/** Returns true if nothing has been scheduled at timeslot t */
bool schedule::available(timeslot t) {
	return ! util::contains(this->table, t);
}

/** Returns true if something has been scheduled at timeslot t */
bool schedule::has(timeslot t) {
	return util::contains(this->table, t);
}

/** Get the channel which is scheduled in timeslot t */
const channel* schedule::get(timeslot t) {
#ifdef USE_SCHEDULE_HASHMAP
	assert(t <= this->max);
#endif
	return this->table.at(t);
}

/** Returns the last timeslot where we have something scheduled */
timeslot schedule::max_time() {
	if (this->table.empty()) return 0;

#ifdef USE_SCHEDULE_HASHMAP
	return this->max;
#else
	return (this->table.rbegin()->first); // rbegin is iterator to pair having the greatest key (and timeslot is the key)
#endif
}

/** Schedule channel c in timeslot t */
void schedule::add(const channel *c, timeslot t) {
	this->table[t] = c;

#ifdef USE_SCHEDULE_HASHMAP
	this->max = util::max(this->max, t);
#endif
}

/** Removes whatever channel is scheduled at t */
void schedule::remove(timeslot t) {
	this->table.erase(t);

#ifdef USE_SCHEDULE_HASHMAP
	this->refresh_max();
#endif
}

/** Removes channel c from all timeslots */
void schedule::remove(channel *c) {
	auto it = this->table.begin();
	const auto end = this->table.end();

	while (it != end) {
		const bool remove = (it->second == c);
		if (remove) this->table.erase(it++);
		else ++it;
	}

#ifdef USE_SCHEDULE_HASHMAP
	this->refresh_max();
#endif
}

////////////////////////////////////////////////////////////////////////////////

link_t::link_t(port_out_t& _source, port_in_t& _sink)
: source(_source), sink(_sink), wrapped(false) {
	const bool loop = (&this->source.parent == &this->sink.parent);
	warn_if(loop, "Self-loop detected on link");

	// Update the ports themselves, that they are infact connect to this link
	this->source.add_link(this);
	this->sink.add_link(this);
}

link_t::link_t(port_out_t& _source, port_in_t& _sink, bool in)
: source(_source), sink(_sink), wrapped(false) {
	if(in){
		this->sink.add_link(this);
	} else {
		this->source.add_link(this);
	}
	
}

std::ostream& operator<<(std::ostream& stream, const link_t& rhs) {
	stream << rhs.source << "-->" << rhs.sink;
	return stream;
}

////////////////////////////////////////////////////////////////////////////////

port_t::port_t(router_t& _parent, port_id _corner) : l(NULL), parent(_parent), corner(_corner) {
}

port_t::~port_t() {
}

void port_t::add_link(link_t *l) {
	ensure(this->l == NULL, "Port " << *this << " already has a link connected: " << *(this->l));
	this->l = l; // now it's connected, but it shouldn't have been before
}

link_t& port_t::link() {
	assert(this->l);
	return *(this->l);
}

bool port_t::has_link() {
	return (this->l != NULL);
}

std::ostream& operator<<(std::ostream& stream, const port_t& rhs) {
	routerport_id rp_id = {rhs.parent.address, rhs.corner};
	stream << rp_id;
	return stream;
}

port_out_t::port_out_t(router_t& _router, port_id _corner) : port_t(_router, _corner) {
}

port_in_t::port_in_t(router_t& _router, port_id _corner) : port_t(_router, _corner) {
}

////////////////////////////////////////////////////////////////////////////////

router_t::router_t(router_id _address)
: address(_address), ports_in(init<port_in_t>(this)), ports_out(init<port_out_t>(this)){	
	// , local_in({this,L},{this,L},true), local_out({this,L},{this,L},false)
	//local_out = new link_t({this,L},{this,L},false);
	//local_in = new link_t({this,L},{this,L},true);
	assert(&this->in(N).parent == this);
	assert(&this->in(S).parent == this);
	assert(&this->in(E).parent == this);
	assert(&this->in(W).parent == this);
	assert(&this->in(L).parent == this);

	assert(&this->out(N).parent == this);
	assert(&this->out(S).parent == this);
	assert(&this->out(E).parent == this);
	assert(&this->out(W).parent == this);
	assert(&this->out(L).parent == this);
}

port_out_t& router_t::out(port_id p) {
	return this->ports_out[p];
}

port_in_t& router_t::in(port_id p) {
	return this->ports_in[p];
}

////////////////////////////////////////////////////////////////////////////////

network_t::network_t(uint rows, uint cols) : m_routers(rows, cols) {
}

uint network_t::rows() const {
	return this->m_routers.rows();
}

uint network_t::cols() const {
	return this->m_routers.cols();
}

timeslot network_t::p() const {
	timeslot ret = 0;
	for_each(this->links(), [&](link_t *l){
		ret = util::max(ret, l->local_schedule.max_time()+1); // +1 because we want the number of time slots not the maximum time
	});
	return ret;
}

bool network_t::has(router_id r) {
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

bool network_t::route_channel(
	channel* c, router_id curr, timeslot t, 
	std::function<void(vector<port_out_t*>&)> next_mutator  
) 
{
	const bool dest_reached = (curr == c->to);
	
	if (dest_reached) {
		if (this->router(c->to)->local_out_schedule.available(t)) {
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
		link_t& l = p->link();

		if (l.local_schedule.available(t) == false)
			continue;
		
		if (this->route_channel(c, l.sink.parent.address, t+1, next_mutator)) {
			l.local_schedule.add(c, t);
			c->t_start = t;
			return true;
		}
	}

	return false;
}


bool network_t::ripup_channel(const channel* c) 
{
	this->router(c->from)->local_in_schedule.remove(c->t_start);
	router_id curr = c->from;
	timeslot t = c->t_start;

	while (curr != c->to) {
		port_out_t *p = NULL;
		for (int i = 0; i < __NUM_PORTS; i++) {
			if (!this->router(curr)->out((port_id)i).has_link())
				continue;
			
			if (this->router(curr)->out((port_id)i).link().local_schedule.get(t) == c) {
				p = &this->router(curr)->out((port_id)i);
				break;
			}
		}
		assert(p != NULL);

		p->link().local_schedule.remove(t++);
		curr = p->link().sink.parent.address;
	}

	assert(curr == c->to);
	
	this->router(c->to)->local_out_schedule.remove(t);
}
