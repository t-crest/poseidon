#include "schedule.hpp"




link_t::link_t(port* _src, port* _dest) : src(_src), dest(_dest) {}

bool link_t::available(timeslot t) {
	return util::contains(this->local_schedule, t);
}
channel* link_t::get(timeslot t) {
	return this->local_schedule.at(t);
}
timeslot link_t::max_time() {
	if (this->local_schedule.empty()) return 0;
	return (this->local_schedule.rbegin()->first); // rbegin is iterator to pair having the greatest key (and timeslot is the key)
}
void link_t::add(channel *c, timeslot t) {
	this->local_schedule[t] = c;
}
void link_t::remove(timeslot t) {
	this->local_schedule.erase(t);
}
void link_t::remove(channel *c) {
	auto it = this->local_schedule.begin();
	const auto end = this->local_schedule.end();
	
	while (it != end) {
		const bool remove = (it->second == c);
		if (remove)		this->local_schedule.erase(it++);
		else			++it;
	}
}

////////////////////////////////////////////////////////////////////////////////

mlink& port::out() {
	return this->output;
}
mlink& port::in() {
	return this->input;	
}

////////////////////////////////////////////////////////////////////////////////

port& router::getport(port_id p) {
	return this->ports[p];
}

////////////////////////////////////////////////////////////////////////////////

network::network(uint rows, uint cols) : routers(rows,cols) {}

mrouter& network::router(router_id r) {
	return this->routers(r);
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
