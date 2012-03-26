#ifndef SCHEDULE2_HPP
#define	SCHEDULE2_HPP

#include "util.hpp"
#include "lex_cast.h"
#include "matrix.hpp"
#include <boost/optional/optional.hpp>
#include <cassert>
#include <utility>
#include <vector>
#include <array>
#include <map>

using std::pair;
using std::map;
using std::array;
using std::vector;

typedef uint timeslot;
typedef uint coord;
typedef std::pair<coord, coord> router_id;
enum port_id {N, S, E, W, L, __NUM_PORTS};

class port_t;

////////////////////////////////////////////////////////////////////////////////

struct channel {
	router_id src;
	router_id dest;
	int bandwidth; // We desire so many packets in the period p.
	// Add Response delay
};

////////////////////////////////////////////////////////////////////////////////

class schedule {
private:
	map<timeslot, channel*> table;
public:
	bool available(timeslot t); 
	channel* get(timeslot t);
	timeslot max_time();
	void add(channel *c, timeslot t); 
	void remove(timeslot t);
	void remove(channel *c);	
};

////////////////////////////////////////////////////////////////////////////////

class port_out_t; // forward decl
class port_in_t;  // forward decl

class link_t {
private:
	port_out_t *source;
	port_in_t *sink;
	schedule local_schedule;
public:
	link_t(port_out_t& _source, port_in_t& _sink);
	port_out_t* from();
	port_in_t* to();
};

//typedef boost::optional<link_t> mlink; // maybe link

////////////////////////////////////////////////////////////////////////////////

class router_t; // forward decl

class port_t {
private:
	router_t* parent;
	boost::optional<link_t*> l;
public:
	port_t(router_t* _router);
	virtual ~port_t() = 0;
	router_t* parent_router() const;
	void add_link(link_t *l);
	link_t* link();
	bool has_link();
};

class port_out_t : public port_t {
public:
	port_out_t(router_t* _router);
};

class port_in_t : public port_t {
public:
	port_in_t(router_t* _router);
};

////////////////////////////////////////////////////////////////////////////////

class router_t {
private:
	typedef std::array<port_in_t, __NUM_PORTS> port_in_array; 
	typedef std::array<port_out_t, __NUM_PORTS> port_out_array; 
	port_in_array ports_in; // always 5 ports (but they might have no links connected)
	port_out_array ports_out; // always 5 ports (but they might have no links connected)
	router_id address;
//	bool dual_ported;
public:
	router_t(router_id _address);
	router_id addr() const;
	port_in_t& in(port_id p);
	port_out_t& out(port_id p);
private:
	static port_in_array init_inputs(router_t *This);
	static port_out_array init_outputs(router_t *This);
};

typedef boost::optional<router_t> mrouter; // maybe router

////////////////////////////////////////////////////////////////////////////////

class network_t {
	matrix<mrouter> routers;
	class specification {
		vector<channel> channels;
	};
public:
	network_t(uint rows, uint cols);
	uint rows() const;
	uint cols() const;
	bool has(router_id r);
	void add(router_id r);
	router_t& router(router_id r);
};

////////////////////////////////////////////////////////////////////////////////

//class schedule {
//private:
//	std::vector<frame> frames;
////	matrix<router> network_topology;
//	network topology;
//
//public:
//	schedule(uint rows, uint cols);
//
//	void add_directed_link(router_id src, port_id src_port, router_id dest);
//	void insert_link(router_id current, port_id output, timeslot t, channel chan);
//	boost::optional<channel> read_link(router_id current, port_id output, timeslot slot);
//	void free_link(router_id current, port_id output, timeslot slot);
//	std::vector<port_id> get_next_links(router_id current, timeslot slot, router_id dest);
//	router_id get_previous_router(router_id current, port_id input, timeslot slot);
//};

#endif	/* SCHEDULE2_HPP */

