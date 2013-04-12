#ifndef SCHEDULE2_HPP
#define	SCHEDULE2_HPP

#include "higher_order.h"
#include "util.hpp"
#include "lex_cast.h"

#include <boost/optional/optional.hpp>
#include <set>
#include <cassert>
#include <utility>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>
#include <iostream>


typedef uint timeslot;
typedef int coord;
typedef std::pair<coord, coord> router_id;

enum port_id {
	N, S, E, W, L, __NUM_PORTS // HACK!!
};
typedef std::pair<router_id, port_id> routerport_id;


router_id operator-(const router_id& lhs, const router_id& rhs);
router_id abs(const router_id& arg);

struct channel; // Forward decl.

std::ostream& operator<<(std::ostream& stream, const port_id& rhs);
std::istream& operator>>(std::istream& stream, port_id& rhs);

std::ostream& operator<<(std::ostream& stream, const routerport_id& rhs);
std::istream& operator>>(std::istream& stream, routerport_id& rhs);

std::ostream& operator<<(std::ostream& stream, const channel& rhs);

////////////////////////////////////////////////////////////////////////////////

struct channel {
	router_id from;
	router_id to;
	int bandwidth; // We desire so many packets in the period p.
	// Add Response delay
	int phits;

	timeslot t_start;
	timeslot t_best_start;
};

////////////////////////////////////////////////////////////////////////////////
//#define USE_SCHEDULE_HASHMAP 

class schedule {
public:
#ifdef USE_SCHEDULE_HASHMAP
    std::unordered_map<timeslot, const channel*> table;
    void refresh_max();
    timeslot max;
#else
    std::map<timeslot, const channel*>  table;
#endif

public:
    schedule();
    bool available(timeslot t);
    bool has(timeslot t);
    bool is(timeslot t, const channel *c);
    boost::optional<timeslot> time(const channel *c);
    const channel* get(timeslot t);
    std::set<const channel*> channels() const;
    timeslot max_time();
    void add(const channel *c, timeslot t);
    void remove(timeslot t);
    void remove(channel *c);
    void clear();
    schedule& operator == (const schedule& rhs);
};

////////////////////////////////////////////////////////////////////////////////

class port_out_t; // forward decl
class port_in_t; // forward decl

class link_t {
public:
    port_out_t& source; // A link is always connected to an output port
    port_in_t& sink; // A link is always connected to an input port
    schedule local_schedule; // A link always has a schedule (which may be empty)
    schedule best_schedule; // The schedule on the link for the best overall solution
    bool wrapped; // true if must be drawn wrapped in SVG
	timeslot depth;

    link_t(port_out_t& _source, port_in_t& _sink);
    link_t(port_out_t& _source, port_in_t& _sink, bool in);
    void updatebest();
};

std::ostream& operator<<(std::ostream& stream, const link_t& rhs);

////////////////////////////////////////////////////////////////////////////////

class router_t; // forward decl

class port_t {
private:
	link_t* l; // A port may not have any link connected
	void add_link(link_t *l);
	friend class link_t; // Allow link_t to call add_link(), despite it being private

public:
	const port_id corner; // World corner
	router_t& parent; // A port always belongs to router, and this can't be changed

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
    std::array<port_in_t, __NUM_PORTS> ports_in; // always 5 ports (but a port might have no link connected)
    std::array<port_out_t, __NUM_PORTS> ports_out; // always 5 ports (but a port might have no link connected)

public:
    schedule local_in_schedule; // Local link to schedule traffic outof the processor
    schedule local_in_best_schedule; 
    schedule local_out_schedule; // Local link to schedule traffic into the processor
    schedule local_out_best_schedule; 
    const router_id address; // Fixed address
    std::map<router_id, std::vector<port_out_t*> > next; // shortest path 
    std::map<router_id, int> hops; // hops from this to router_id

    router_t(router_id _address);
    port_in_t& in(port_id p);
    port_out_t& out(port_id p);
    void updatebest();
private:

    template <typename T>
    static std::array<T, __NUM_PORTS> init(router_t *This) {
            std::array<T, __NUM_PORTS> ret = {T(*This, N), T(*This, S), T(*This, E), T(*This, W), T(*This, L)};
            return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////
typedef std::function<void(std::vector<port_out_t*>&)> mutate_func_t;
static const mutate_func_t next_identity = [](std::vector<port_out_t*>& arg){/*This is the identity function; arg is not modified*/};	

#endif	/* SCHEDULE2_HPP */

