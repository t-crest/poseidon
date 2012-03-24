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

//typedef int channel_t;
typedef uint timeslot;
typedef uint coord;
typedef std::pair<coord, coord> router_id;
enum port_id {N, S, E, W, L, __NUM_PORTS};

class port;

////////////////////////////////////////////////////////////////////////////////

struct channel {
	router_id src;
	router_id dest;
	int bandwidth; // We desire so many packets in the period p.
	// Add Response delay
};

////////////////////////////////////////////////////////////////////////////////

class link_t {
private:
	map<timeslot, channel*> local_schedule;
	port *src;
	port *dest;
public:
	link_t(port* _src, port* _dest);
	
	bool available(timeslot t); // True if nothing has been scheduled onto this at time t
	channel* get(timeslot t);
	timeslot max_time();
	void add(channel *c, timeslot t); // Schedule c onto this link at time t
	void remove(timeslot t);
	void remove(channel *c);
};

typedef boost::optional<link_t> mlink;

////////////////////////////////////////////////////////////////////////////////

class port {
private:
	mlink output;
	mlink input;
public:
	mlink& out();
	mlink& in();
};

////////////////////////////////////////////////////////////////////////////////

class router {
//	bool dual_ported;
	typedef std::array<port, __NUM_PORTS> port_array; // 5 ports
	port_array ports; // A router has output-ports
public:
	port& getport(port_id p);
};

typedef boost::optional<router> mrouter;

////////////////////////////////////////////////////////////////////////////////

class network {
	matrix<mrouter> routers;
	class specification {
		vector<channel> channels;
	};
public:
	network(uint rows, uint cols);
	mrouter& router(router_id r);
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

