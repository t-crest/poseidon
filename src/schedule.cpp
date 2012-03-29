#include "schedule.hpp"


router_id operator-(const router_id& lhs, const router_id& rhs) {
	router_id ret = {lhs.first-rhs.first, lhs.second-rhs.second};
	return ret;
}

router_id abs(const router_id& arg) {
	router_id ret = {abs(arg.first), abs(arg.second)};
	return ret;
}

std::ostream& operator<<(std::ostream& stream, const port_id& rhs) {
	static const std::map<port_id, char> m = {{N,'N'},{S,'S'},{E,'E'},{W,'W'},{L,'L'}};
	stream << m.at(rhs);
	return stream;
}

std::istream& operator>>(std::istream& stream, port_id& rhs) {
	static const std::map<char,port_id> m = {{'N',N},{'S',S},{'E',E},{'W',W},{'L',L}};
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



////////////////////////////////////////////////////////////////////////////////

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

link_t::link_t(port_out_t& _source, port_in_t& _sink) 
:	source(_source), sink(_sink) 
{
	const bool loop = (&this->source.parent == &this->sink.parent);
	warn_if(loop, "Self-loop detected on link");
	
	// Update the ports themselves, that they are infact connect to this link
	this->source.add_link(this);
	this->sink.add_link(this);
}

std::ostream& operator<<(std::ostream& stream, const link_t& rhs) {
	stream << rhs.source << "-->" << rhs.sink;
	return stream;
}

////////////////////////////////////////////////////////////////////////////////

port_t::port_t(router_t& _parent, port_id _corner) : l(NULL), parent(_parent), corner(_corner) {}
port_t::~port_t() {}
void port_t::add_link(link_t *l) {
	ensure(this->l==NULL, "Port " << *this << " already has a link connected: " << *(this->l));
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

port_out_t::port_out_t(router_t& _router, port_id _corner) : port_t(_router, _corner) {}
port_in_t::port_in_t(router_t& _router, port_id _corner) : port_t(_router, _corner) {}

////////////////////////////////////////////////////////////////////////////////

router_t::router_t(router_id _address) 
:	address(_address), ports_in(init<port_in_t>(this)), ports_out(init<port_out_t>(this))
{
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

network_t::network_t(uint rows, uint cols) : routers(rows,cols) {}

uint network_t::rows() const {return this->routers.rows();} 
uint network_t::cols() const {return this->routers.cols();}

bool network_t::has(router_id r) {
	return (this->routers(r) != NULL);
}

link_t* network_t::add(port_out_t& source, port_in_t& sink) {
	link_t *l = new link_t(source, sink);
	this->link_ts.insert(l);
	return l;
}

router_t* network_t::add(router_id r) {
	ensure(0 <= r.first && r.first <= this->cols()-1, "Tried to add router " << r << " outside network");
	ensure(0 <= r.second && r.second <= this->rows()-1, "Tried to add router " << r << " outside network");

	if (this->routers(r) == NULL) { // create only new router if it didn't exist before
		this->routers(r) = new router_t(r);
		assert(this->router(r) == &this->router(r)->out(N).parent);
		assert(this->router(r) == &this->router(r)->out(L).parent);
	}
	return this->routers(r);
}

router_t* network_t::router(router_id r) {
	return this->routers(r);
}

const set<link_t*>& network_t::links() const {
	return this->link_ts;
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
