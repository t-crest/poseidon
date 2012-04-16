#include "schedulers.hpp"


////////////////////////////////////////////////////////////////////////////////

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
		channel *c = (channel*) pq.top().second;
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


	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second;
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

s_bad_random::s_bad_random(network_t& _n) : scheduler(_n) {
}

void s_bad_random::run() {
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

	timeslot t_start = 0;

	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second; pq.pop(); // ignore .first

		for (timeslot t = t_start;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel(c, c->from, t, next_mutator);
			if (path_routed) {
				n.router(c->from)->local_in_schedule.add(c, t);
				t_start = t + n.router(c->from)->hops.at(c->to) + 1;
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

s_lns::s_lns(network_t& _n) : scheduler(_n) {
	assert(&_n == &n);
//	scheduler *s = new s_greedy(this->n, false);
	scheduler *s = new s_bad_random(this->n);
	
	s->run();
}

void s_lns::run() {
	// initial 
	int best, curr;
	best = curr = n.p();

	debugf(curr);

	for (;;) {
		this->destroy();
		this->repair();

		curr = n.p();
		debugf(curr);

		if (curr < best) {
			best = curr;
			debugf(best);
		}

		// noget := asdasdads
		// choose:
		//	1. random - Done
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

std::set<const channel*> s_lns::choose_dom_and_depends() {

	std::set<const channel*> dom;

	timeslot p = this->n.p();

	// Find the dominating paths first

	for_each(this->n.links(), [&](link_t* t) {
		if (t->local_schedule.has(p - 1)) {
			assert(t->local_schedule.max_time() <= p - 1);
			const channel *c = t->local_schedule.get(p - 1);
			assert(c != NULL);
			dom.insert(c);
//			debugf(dom.size());
		}
	});

	std::set<const channel*> ret = dom;
	
	for_each(dom, [&](const channel *dom_c){
		std::set<const channel*> chns = this->depend_path(dom_c);
		ret.insert(chns.begin(), chns.end());
	});

	return ret;
}

std::set<const channel*> s_lns::depend_path(const channel* dom) 
{
	router_id curr = dom->from;
	std::set<const channel*> ret;
	
	for (int i = 0; i < __NUM_PORTS; i++) {
		port_id p = (port_id)i;
		
		if (!n.router(curr)->out(p).has_link()) continue;
		auto time = n.router(curr)->out(p).link().local_schedule.time(dom);
		if (!time) continue;
		
		timeslot max = *time;
		for (timeslot t = 0; t <= max; t++) {
			if (!n.router(curr)->out(p).link().local_schedule.has(t)) continue;
			const channel *c = n.router(curr)->out(p).link().local_schedule.get(t);
			ret.insert(c);
		}
		curr = n.router(curr)->out(p).link().sink.parent.address;
	}
	
	return ret;
}

void s_lns::destroy() {
	//	auto chosen = this->choose_random();
	auto chosen = this->choose_dom_and_depends();

	for_each(chosen, [&](const channel * c) {
		this->n.ripup_channel(c);
		const int hops = this->n.router(c->from)->hops[c->to];
		this->unrouted_channels.insert(make_pair(hops, c));
	});
}

void s_lns::repair() {

	for_each_reverse(this->unrouted_channels, [&](const std::pair<int, const channel*>& p) {

		const channel *c = p.second;

		for (int t = 0;; t++) {
			if (this->n.route_channel((channel*) c, c->from, t))
				break;
			}
	});
	
	this->unrouted_channels.clear();
}
