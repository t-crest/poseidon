/* 
 * File:   network_t.hpp
 * Author: T410s
 *
 * Created on 9. april 2013, 10:40
 */

#ifndef NETWORK_T_HPP
#define	NETWORK_T_HPP


#include <vector>
#include "matrix.hpp"
#include "schedule.hpp"
#include "higher_order.h"
#include "options.h"
#include <limits>

namespace snts {

class network_t {
    matrix<router_t*> m_routers;
    std::vector<router_t*> router_ts;
    std::vector<link_t*> link_ts;
    
    friend class parser;
    
public:
	std::vector<channel> specification;
	
public:
    int curr;
    int best;
    int prev;

	// Constructor
    network_t(uint rows, uint cols);
	
	// Get functions
    timeslot p() const;
    timeslot p_best() const;
    uint rows() const;
    uint cols() const;
	const std::vector<link_t*>& links() const;
    const std::vector<router_t*>& routers() const;
    const std::vector<channel>& channels() const;
    bool has(const router_id r) const;
	
	// Topology mutators
    link_t* add(port_out_t& source, port_in_t& sink);
    router_t* add(router_id r);
    router_t* router(router_id r);
	
	// Functions for calculating inital routing information
    void shortest_path_bfs(router_t *dest);
    void shortest_path();
	
	// Print schedule information
    void print_next_table();
    void print_channel_specification();
	timeslot links_in_topology();
	timeslot occupied_links(bool best);
    float link_utilization(bool best);
	
	// Functions for updating the schedule information
    void updatebest();
    void clear(); // clears

	// Routing functions
    bool route_channel_wrapper(channel* c, timeslot t, std::function<void(std::vector<port_out_t*>&)> next_mutator = next_identity);
    void ripup_channel(const channel* c, router_id start);
    void check_channel(const channel* c, const bool best);
private:
    bool route_channel(channel* c, router_id curr, timeslot t, std::function<void(std::vector<port_out_t*>&)> next_mutator = next_identity);
};

}
#endif	/* NETWORK_T_HPP */

