#include "schedule.hpp"



/** Returns true if nothing has been scheduled at timeslot t */
bool schedule::available(timeslot t) {
	return ! util::contains(this->table, t);
}
/** Get the channel which is scheduled in timeslot t */
channel* schedule::get(timeslot t) {
	return this->table.at(t);
}
/** Returns the last timeslot where we have something scheduled */
timeslot schedule::max_time() {
	if (this->table.empty()) return 0;
	return (this->table.rbegin()->first); // rbegin is iterator to pair having the greatest key (and timeslot is the key)
}
/** Schedule channel c in timeslot t */
void schedule::add(channel *c, timeslot t) {
	this->table[t] = c;
}
/** Removes whatever channel is scheduled at t */
void schedule::remove(timeslot t) 
{
	this->table.erase(t);
}
/** Removes channel c from all timeslots */
void schedule::remove(channel *c) 
{
	auto it = this->table.begin();
	const auto end = this->table.end();

	while (it != end) {
		const bool remove = (it->second == c);
		if (remove)		this->table.erase(it++);
		else			++it;
	}
}

////////////////////////////////////////////////////////////////////////////////

link_t::link_t(port_out_t& _source, port_in_t& _sink) : source(&_source), sink(&_sink) {
	const bool loop = (this->source->parent_router() == this->sink->parent_router());
	debugf(this->source->parent_router());
	debugf(this->sink->parent_router());
	warning(loop, "Self-loop detected on link")
}
port_out_t* link_t::from() {return this->source;}
port_in_t* link_t::to() {return this->sink;}

////////////////////////////////////////////////////////////////////////////////

port_t::port_t(router_t* _parent) : parent(_parent) {}
port_t::~port_t() { }
router_t* port_t::parent_router() const {
	return this->parent;
}
void port_t::add_link(link_t *l) {
	this->l = l;
}
link_t* port_t::link() {
	return *(this->l);
}
bool port_t::has_link() {
	return this->l.is_initialized();
}

port_out_t::port_out_t(router_t* _router) : port_t(_router) {}
port_in_t::port_in_t(router_t* _router) : port_t(_router) {}

////////////////////////////////////////////////////////////////////////////////

router_t::router_t(router_id _address) 
:	address(_address), ports_in(init_inputs(this)), ports_out(init_outputs(this))
{
	debugf(this);
	assert(this->in(N).parent_router() == this);
	assert(this->in(S).parent_router() == this);
	assert(this->in(E).parent_router() == this);
	assert(this->in(W).parent_router() == this);
	assert(this->in(L).parent_router() == this);

	assert(this->out(N).parent_router() == this);
	assert(this->out(S).parent_router() == this);
	assert(this->out(E).parent_router() == this);
	assert(this->out(W).parent_router() == this);
	assert(this->out(L).parent_router() == this);
}

router_id router_t::addr() const {return this->address;}

port_out_t& router_t::out(port_id p) {
	return this->ports_out[p];
}

port_in_t& router_t::in(port_id p) {
	return this->ports_in[p];
}

router_t::port_in_array router_t::init_inputs(router_t *This) {
	port_in_array ret = {port_in_t(This),port_in_t(This),port_in_t(This),port_in_t(This),port_in_t(This)};
	return ret;
}

router_t::port_out_array router_t::init_outputs(router_t *This) {
	port_out_array ret = {port_out_t(This),port_out_t(This),port_out_t(This),port_out_t(This),port_out_t(This)};
	return ret;
}

////////////////////////////////////////////////////////////////////////////////

network_t::network_t(uint rows, uint cols) : routers(rows,cols) {}

uint network_t::rows() const {return this->routers.rows();} 
uint network_t::cols() const {return this->routers.cols();}

bool network_t::has(router_id r) {
	return this->routers(r).is_initialized();
}

void network_t::add(router_id r) {
	debugf(&(this->routers(r)));
	this->routers(r) = router_t(r); //XXX assignment operator
	debugf(&(this->routers(r)));
}

router_t& network_t::router(router_id r) {
	return *(this->routers(r));
}

////////////////////////////////////////////////////////////////////////////////


//schedule::schedule(uint rows, uint cols)
//:	network_topology(rows, cols) 
//{
//	
//}
//
//void
//schedule::add_directed_link(router_id src, port_id src_port, router_id dest) {
//	this->network_topology(src).outgoing_ports[src_port] = dest;
//}
//
//void
//schedule::insert_link(router_id r, port_id output, timeslot t, channel chan) {
//	
////	this->frames[t].routers[r].outputs[output] = chan;
//	
//	
//}
//
//boost::optional<channel>
//schedule::read_link(router_id current, port_id output, timeslot slot) {
//
//}
//
//void
//schedule::free_link(router_id current, port_id output, timeslot slot) {
//
//}
//
//std::vector<port_id>
//schedule::get_next_links(router_id current, timeslot slot, router_id dest) {
//
//}
//
//router_id 
//schedule::get_previous_router(router_id current, port_id input, timeslot slot) {
//
//}
//
