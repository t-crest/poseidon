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
 
#include "schedule.hpp"

router_id operator-(const router_id& lhs, const router_id& rhs) {
	router_id ret = {lhs.first - rhs.first, lhs.second - rhs.second};
	return ret;
}

router_id abs(const router_id& arg) {
	router_id ret = {std::abs(arg.first), std::abs(arg.second)};
	return ret;
}

char p2c(const port_id p){
		char c;
		if (p == N) c = 'N';
		if (p == E)	c = 'E';
		if (p == S)	c = 'S';
		if (p == W)	c = 'W';
		if (p == L)	c = 'L';
		if (p == __NUM_PORTS)	c = 'D';

		return c;
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
    stream << "[from=" << rhs.from <<
			", to=" << rhs.to <<
			", t_start=" << rhs.t_start <<
			", t_best_start=" << rhs.t_best_start <<
			", channel_id=" << rhs.channel_id <<
			", phits=" << rhs.phits <<
			"]";
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

std::set<const channel*> schedule::channels() const {
	std::set<const channel*> ret;
	for(auto it = this->table.cbegin(); it != this->table.cend(); ++it)
		ret.insert(it->second);
	
	return ret;
}

/** Returns true if nothing has been scheduled at timeslot t */
bool schedule::available(timeslot t, timeslot phits) {
#ifdef USE_NAIVE_LOOKUP
	bool ret = true;
	for(int i = 0; i < phits; i++) {
		bool contain = util::contains(this->table, t + i);
		if (contain) {
			ret = false;
			break;
		}
	}
	return ret;
#else
	return false;
#endif
}

/** Returns true if something has been scheduled at timeslot t */
bool schedule::has(const timeslot t) {
	return util::contains(this->table, t);
}

bool schedule::is(const channel* c, const timeslot t){
	if(!this->has(t)) return false;
	if(this->get(t) != c) return false;
	return true;
}

/** Schedule channel c in timeslot t */
void schedule::add(const channel *c, timeslot t) {
#ifdef USE_NAIVE_LOOKUP
	for(uint i = 0; i < c->phits; i++){
		this->table[t+i] = c;
	}
#else
	this->table[t] = c;
#endif
	
#ifdef USE_SCHEDULE_HASHMAP
	this->max = util::max(this->max, t);
#endif
}

/** Get the channel which is scheduled in timeslot t */
const channel* schedule::get(const timeslot t) {
#ifdef USE_SCHEDULE_HASHMAP
	assert(t <= this->max);
#endif
	return this->table.at(t);
}

/** 
 * Removes the given channel scheduled at t
 * This function is not safe in the sense that no checking
 * whether the right channel is removed.
 */
void schedule::remove(const channel *c, timeslot t) {
#ifdef USE_NAIVE_LOOKUP
	for(uint i = 0; i < c->phits; i++){
		ensure(this->is(c,t+i), "Remove channel call was unaligned. At time t = " << t << " , channel = " << c);
		this->table.erase(t+i);
	}
#else
	this->table.erase(t);
#endif

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

void schedule::clear() {
#ifdef USE_SCHEDULE_HASHMAP
	this->max = 0;
#endif
	this->table.clear();
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

/** Returns true if channel c is contained in the schedule */
boost::optional<timeslot> schedule::time(const channel *c) {
	boost::optional<timeslot> ret;
	for (auto it = this->table.begin(); it != this->table.end(); ++it) {
		if (it->second == c) return it->first;
	}
	return ret;
}

schedule& schedule::operator == (const schedule& rhs) {
#ifdef USE_SCHEDULE_HASHMAP
	this->max = rhs.max;
#endif
	this->table = rhs.table;
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

void link_t::updatebest() {
	this->best_schedule = this->local_schedule;
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

void router_t::updatebest() {
	this->local_in_best_schedule = this->local_in_schedule;
	this->local_out_best_schedule = this->local_out_schedule;
}
