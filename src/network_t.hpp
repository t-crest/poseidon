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

#ifndef NETWORK_T_HPP
#define	NETWORK_T_HPP


#include <vector>
#include "matrix.hpp"
#include "schedule.hpp"
#include "higher_order.h"
#include "options.h"
#include <limits>
#include <string>

namespace snts {

class network_t {
    matrix<router_t*> m_routers;
    std::vector<router_t*> router_ts;
    std::vector<link_t*> link_ts;
    const timeslot router_depth;
	const int available_timeslots;
	
    friend class parser;
    
public:
	std::vector<channel> specification;
	
public:
    int curr;
    int best;
    int prev;

	// Constructor
    network_t(uint rows, uint cols, uint router_depth, int available_timeslots);
	
	// Get functions
    timeslot p() const;
    timeslot p_best() const;
    uint rows() const;
    uint cols() const;
	const std::vector<link_t*>& links() const;
    const std::vector<router_t*>& routers() const;
    const std::vector<channel>& channels() const;
    bool has(const router_id r) const;
	const timeslot get_router_depth() const;
    const timeslot get_schedule_overlap() const;
	
	// Topology mutators
    link_t* add(port_out_t& source, port_in_t& sink);
    router_t* add(router_id r);
    router_t* router(router_id r);
	router_t* router(router_id r) const;
	
	// Functions for calculating inital routing information
    void shortest_path_bfs(router_t *dest);
    void shortest_path();
	
	// Print schedule information
    void print_next_table() const;
    void print_channel_specification() const;
	int links_in_topology() const;
	timeslot occupied_links(bool best) const;
	int io_activity(router_t* r) const;
	int max_io_activity() const;
	
	// Functions for updating the schedule information
    void updatebest();
    void clear(); // clears

	// Routing functions
    bool route_channel_wrapper(channel* c, timeslot t, std::function<void(std::vector<port_out_t*>&)> next_mutator = next_identity);
    void ripup_channel(const channel* c, router_id start);
    void check_channel(const channel* c, const bool best);
	std::string get_route(const channel* c) const;
private:
    bool route_channel(channel* c, router_id curr, timeslot t, std::function<void(std::vector<port_out_t*>&)> next_mutator = next_identity);
};

}
#endif	/* NETWORK_T_HPP */

