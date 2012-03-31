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
#include <memory>

using std::pair;
using std::map;
using std::set;
using std::array;
using std::vector;

typedef uint timeslot;
typedef int coord;
typedef std::pair<coord, coord> router_id;
enum port_id {N, S, E, W, L, __NUM_PORTS};
typedef std::pair<router_id, port_id> routerport_id;


router_id operator-(const router_id& lhs, const router_id& rhs);
router_id abs(const router_id& arg);

std::ostream& operator<<(std::ostream& stream, const port_id& rhs);
std::istream& operator>>(std::istream& stream, port_id& rhs);

std::ostream& operator<<(std::ostream& stream, const routerport_id& rhs);
std::istream& operator>>(std::istream& stream, routerport_id& rhs);




////////////////////////////////////////////////////////////////////////////////

struct channel {
	router_id from;
	router_id to;
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
public:
	port_out_t& source;			// A link is always connected to an output put
	port_in_t& sink;			// A link is always connected to an input port
	schedule local_schedule;	// A link always has a schedule (which may be empty)
	bool wrapped;	// true if must be drawn wrapped in SVG
	
	link_t(port_out_t& _source, port_in_t& _sink);
};

std::ostream& operator<<(std::ostream& stream, const link_t& rhs);

////////////////////////////////////////////////////////////////////////////////

class router_t; // forward decl

class port_t {
private:
	link_t* l;				// A port may not have any link connected
	void add_link(link_t *l);
	friend class link_t;	// Allow link_t to call add_link(), despite it being private
	
public:
	const port_id corner;	// World corner
	router_t& parent;		// A port always belongs to router, and this can't be changed

	port_t(router_t& _router, port_id _corner);
	virtual ~port_t() = 0; // Disallow instances of port_t
	link_t& link();
	bool has_link();
};

class port_out_t : public port_t {
public:
	port_out_t(router_t& _router, port_id _corner);
};

class port_in_t : public port_t {
public:
	port_in_t(router_t& _router, port_id _corner);
};

std::ostream& operator<<(std::ostream& stream, const port_t& rhs);

////////////////////////////////////////////////////////////////////////////////

class router_t {
private:
	std::array<port_in_t, __NUM_PORTS>  ports_in;	// always 5 ports (but a port might have no link connected)
	std::array<port_out_t, __NUM_PORTS> ports_out;	// always 5 ports (but a port might have no link connected)

public:
	const router_id address;	// Fixed address

	router_t(router_id _address);
	port_in_t& in(port_id p);
	port_out_t& out(port_id p);

private:
	template <typename T> 
	static std::array<T, __NUM_PORTS> init(router_t *This) {
		std::array<T, __NUM_PORTS> ret = {T(*This,N),T(*This,S),T(*This,E),T(*This,W),T(*This,L)};
		return ret;
	}
};

//typedef boost::optional<router_t> mrouter; // maybe router

////////////////////////////////////////////////////////////////////////////////

class network_t {
	matrix<router_t*> routers;
	set<link_t*> link_ts;
	class specification {
		vector<channel> channels;
	};
public:
	network_t(uint rows, uint cols);
	uint rows() const;
	uint cols() const;
	bool has(router_id r);
	link_t* add(port_out_t& source, port_in_t& sink);
	router_t* add(router_id r);
	router_t* router(router_id r);
	const set<link_t*>& links() const;
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

