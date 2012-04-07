#ifndef SCHEDULE2_HPP
#define	SCHEDULE2_HPP

#include "higher_order.h"
#include "util.hpp"
#include "lex_cast.h"
#include "matrix.hpp"
#include <boost/optional/optional.hpp>
#include "parser.hpp"
#include <cassert>
#include <utility>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <queue>
#include <memory>
#include <iostream>

using std::pair;
using std::map;
using std::set;
using std::array;
using std::vector;
using std::cout;
using std::endl;

typedef uint timeslot;
typedef int coord;
typedef std::pair<coord, coord> router_id;

enum port_id {
    N, S, E, W, L, __NUM_PORTS
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
};

////////////////////////////////////////////////////////////////////////////////
#define USE_SCHEDULE_HASHMAP 

class schedule {
private:
#ifdef USE_SCHEDULE_HASHMAP
	std::unordered_map<timeslot, const channel*> table;
	void refresh_max();
	timeslot max;
#else
	std::map<timeslot, const channel*> table;
#endif
public:
	schedule();
    bool available(timeslot t);
    bool has(timeslot t);
    const channel* get(timeslot t);
    timeslot max_time();
    void add(const channel *c, timeslot t);
    void remove(timeslot t);
    void remove(channel *c);
};

////////////////////////////////////////////////////////////////////////////////

class port_out_t; // forward decl
class port_in_t; // forward decl

class link_t {
public:
    port_out_t& source; // A link is always connected to an output put
    port_in_t& sink; // A link is always connected to an input port
    schedule local_schedule; // A link always has a schedule (which may be empty)
    bool wrapped; // true if must be drawn wrapped in SVG

    link_t(port_out_t& _source, port_in_t& _sink);
    link_t(port_out_t& _source, port_in_t& _sink, bool in);
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
    schedule local_out_schedule; // Local link to schedule traffic into the processor
    const router_id address; // Fixed address
    map<router_id, vector<port_out_t*> > next; // shortest path 
    map<router_id, int> hops; // hops from this to router_id
    
    

    router_t(router_id _address);
    port_in_t& in(port_id p);
    port_out_t& out(port_id p);

private:

    template <typename T>
    static std::array<T, __NUM_PORTS> init(router_t *This) {
        std::array<T, __NUM_PORTS> ret = {T(*This, N), T(*This, S), T(*This, E), T(*This, W), T(*This, L)};
        return ret;
    }
};

//typedef boost::optional<router_t> mrouter; // maybe router

////////////////////////////////////////////////////////////////////////////////

class network_t {
    matrix<router_t*> m_routers;
    vector<router_t*> router_ts;
    vector<link_t*> link_ts;
    vector<channel> specification;
    friend class parser;
public:
    network_t(uint rows, uint cols);
    timeslot p() const;
    uint rows() const;
    uint cols() const;
    bool has(router_id r);
    link_t* add(port_out_t& source, port_in_t& sink);
    router_t* add(router_id r);
    router_t* router(router_id r);
    const vector<link_t*>& links() const;
    const vector<router_t*>& routers() const;
    const vector<channel>& channels() const;
    void shortest_path_bfs(router_t *dest);
    void shortest_path();
    void print_next_table();
    void print_channel_specification();
	bool route_channel(const channel* c, router_id curr, timeslot t);
};

#endif	/* SCHEDULE2_HPP */

