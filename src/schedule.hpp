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
char p2c(const port_id p);
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
	// Add Response delay
	uint channel_id;
    unsigned char ch_bw;
    unsigned char pkt_id;
	unsigned char phits;

	timeslot t_start;
	timeslot t_best_start;
};

////////////////////////////////////////////////////////////////////////////////
//#define USE_SCHEDULE_HASHMAP 
#define USE_NAIVE_LOOKUP

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
	std::set<const channel*> channels() const;
    bool available(timeslot t, timeslot phits);
    bool has(timeslot t);
    bool is(const channel *c, timeslot t);
    void add(const channel *c, timeslot t);
	const channel* get(timeslot t);
    void remove(const channel *c, timeslot t);
    void remove(channel *c);
    void clear();
	timeslot max_time();
	boost::optional<timeslot> time(const channel *c);
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

