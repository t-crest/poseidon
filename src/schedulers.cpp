#include "schedulers.hpp"


scheduler::scheduler(network_t& _n) : n(_n) {}


////////////////////////////////////////////////////////////////////////////////


s_greedy::s_greedy(network_t& _n) : scheduler(_n) {}
void s_greedy::run() 
{
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	
	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	debugf(pq.size());

	
	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) 
	{
		const channel *c = pq.top().second; pq.pop(); // ignore .first
		
		for (timeslot t = 0;; t++) 
		{
			if (n.router(c->from)->local_in_schedule.available(t) == false) 
				continue;
			
			const bool path_routed = n.route_channel(c, c->from, t);
			if (path_routed) {
				n.router(c->from)->local_in_schedule.add(c, t);
				break;
			}
		}
	}
}


// // This is 1.7x slower than the recursive solution. This might be due to more indirections and std::stack is slower than the call-stack
//bool route_channel2(network_t& n, const channel* c, timeslot start) 
//{
//	std::stack<link_t*> ls;
//	
//	std::stack< 
//		std::pair<
//			set<port_out_t*>::iterator, 
//			set<port_out_t*>::iterator /*end*/
//		> 
//	> rs;
//
//	rs.push( make_pair(n.router(c->from)->next[c->to].begin(), n.router(c->from)->next[c->to].end()) );
//	assert(n.router(c->from)->next[c->to].begin() != n.router(c->from)->next[c->to].end());
//	timeslot t = start;
//	
//	while (!rs.empty()) 
//	{
//		set<port_out_t*>::iterator& it = rs.top().first;
//		set<port_out_t*>::iterator& end = rs.top().second;
//		assert(it != end);
//				
//		if ((*it)->link().local_schedule.available(t++)) 
//		{
//			router_t& next_router = (*it)->link().sink.parent;
//			const bool dest_reached = (next_router.address == c->to);
//			if (dest_reached) {
//				if (n.router(c->to)->local_out_schedule.available(t))	break;
//				else													return false;
//			}
//
//			set<port_out_t*>& next_ports = next_router.next[c->to];
//			assert(next_ports.begin() != next_ports.end());
//
//			rs.push(	
//				make_pair(
//					next_ports.begin(), 
//					next_ports.end()
//				)    
//			);
//			
//		} else {
//			while (!rs.empty() && (++rs.top().first == rs.top().second)) {
//				rs.pop(); t--;
//			}
//		}
//	}
//
//	if (rs.empty())
//		return false;
//
//
//	n.router(c->to)->local_out_schedule.add(c, t--);
//	while (!rs.empty()) {
//		set<port_out_t*>::iterator& it = rs.top().first;
//		set<port_out_t*>::iterator& end = rs.top().second;
//		assert(it != end);
//
//		(*it)->link().local_schedule.add(c, t--);
//		rs.pop();
//	}
//	
//	return true;
//}
//

////////////////////////////////////////////////////////////////////////////////

s_random::s_random(network_t& _n) : scheduler(_n) {}
void s_random::run() 
{
	std::random_device seed;
	std::mt19937 rng(seed());
	std::uniform_int_distribution<uint> unif;
	
	
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;
	
	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
		pq.push( make_pair(unif(rng), &c) );
	});
	debugf(pq.size());

	
	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) 
	{
		const channel *c = pq.top().second; pq.pop(); // ignore .first
		
		for (timeslot t = 0;; t++) 
		{
			if (n.router(c->from)->local_in_schedule.available(t) == false) 
				continue;
			
			const bool path_routed = n.route_channel(c, c->from, t);
			if (path_routed) {
				n.router(c->from)->local_in_schedule.add(c, t);
				break;
			}
		}
	}	
	
	
}
	