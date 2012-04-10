#include "schedulers.hpp"

scheduler::scheduler(network_t& _n) : n(_n) {
}


////////////////////////////////////////////////////////////////////////////////

s_greedy::s_greedy(network_t& _n, bool _random) : scheduler(_n), random(_random) {
}

void s_greedy::run() {
	util::srand();


	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length

	for_each(n.channels(), [&](const channel & c) {
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	debugf(pq.size());

	auto next_mutator =
		this->random
		?
		[](vector<port_out_t*>& arg){
		if (arg.size() == 1) return;

		for (int i = 0; i < arg.size(); i++) {
			int a = util::rand() % arg.size();
			int b = util::rand() % arg.size();
			std::swap(arg[a], arg[b]);
		}
	}
	:

	[](vector<port_out_t*>& arg) {
		/*This is the identity function; arg is not modified*/
	}
	;

	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) {
		channel *c = (channel*)pq.top().second;
		pq.pop(); // ignore .first

		for (timeslot t = 0;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel(c, c->from, t, next_mutator);
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

s_random::s_random(network_t& _n) : scheduler(_n) {
}

void s_random::run() {
	util::srand();

	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length

	for_each(n.channels(), [&](const channel & c) {
		pq.push(make_pair(util::rand(), &c));
	});
	debugf(pq.size());



	auto next_mutator = [](vector<port_out_t*>& arg){
		if (arg.size() == 1) return;

		//				for (int i = 0; i < arg.size(); i++) {
		int a = util::rand() % arg.size();
		int b = util::rand() % arg.size();
		std::swap(arg[a], arg[b]);
		//				}
	};


	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) {
		channel *c = (channel*)pq.top().second;
		pq.pop(); // ignore .first

		for (timeslot t = 0;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel(c, c->from, t, next_mutator);
			if (path_routed) {
				n.router(c->from)->local_in_schedule.add(c, t);
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

s_lns::s_lns(network_t& _n) : scheduler(n){
	scheduler *s = new s_greedy(n,true);
	s->run();
}

void s_lns::run() {
	// initial 
	int best, curr;
	best = curr = n.p();

	for (;;) {
		this->destroy();
		this->repair();
		curr = n.p();
		if(curr < best){
			best = curr;
			debugf(best);
		}

		// noget := asdasdads
		// choose:
		//	1. random
		//  2. dense routers
		//  3. dominating paths + dependencies
		//  4. functor next_mutator: always route towards least dense router
		//  5. finite lookahead

		// destroy noget
		// repair igen
	}

}

std::set<const channel*> s_lns::choose_random() {
	util::srand();

	std::set<const channel*> ret;
	int cnt = util::rand() % (int) (0.1 * this->n.channels().size());
	cnt = util::max(cnt, 2);

	while (ret.size() < cnt) {
		const int idx = util::rand() % this->n.channels().size();
		ret.insert(&(this->n.channels()[idx]));
	}

	return ret;
}

void s_lns::destroy() {
	auto chosen = this->choose_random();

	for_each(chosen, [&](const channel * c) {
		this->n.ripup_channel(c);
		this->unrouted_channels.insert(c);
	});
}

void s_lns::repair() {

	for_each(this->unrouted_channels, [&](const channel * c) {
		for(int t = 0;; t++){
			if(this->n.route_channel((channel*)c, c->from,t))
				break;
		}
	});
}
