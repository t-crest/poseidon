#include "schedulers.hpp"



std::function<void(vector<port_out_t*>&)>
get_next_mutator() 
{
	std::function<void(vector<port_out_t*>&)> 
	ret = [](vector<port_out_t*>& arg)
	{
		if (arg.size() == 1) return; // singleton -- not much to randomize here

//		for (int i = 0; i < arg.size(); i++) {
			int a = util::rand() % arg.size();
			int b = util::rand() % arg.size();
			std::swap(arg[a], arg[b]);
//		}
	};
	
	return ret;
}


////////////////////////////////////////////////////////////////////////////////

scheduler::scheduler(network_t& _n) : n(_n) {
	percent = 0.0;
	best_for_print = 0;
	curr_for_print = 0;
	print_cnt = 0;
	initial = 0;
}

void scheduler::percent_set(const int init, const string text){
	percent = 0.0;
	initial = init;
	cout << text << endl;
}

void scheduler::percent_up(const int curr){
	
	float curr_percent = 100-(curr*100)/initial;
	curr_percent = round(curr_percent*1e2)/1e2; // Rounding to 2 decimal point precision
	if(curr_percent > percent && curr_percent <= 100){
		percent = curr_percent;
		(cout << "\r" << "Progress: " << curr_percent << "%").flush();
		if (int(curr_percent) == 100)
			cout << " Done." << endl;
	}

}

void scheduler::verify(const bool best){
	if (best)
		ensure(global::opts->save_best, "Can not check best solution, if it is not stored!");
		
	for_each(n.channels(), [&](const channel & c){
		n.check_channel(&c, best);
	});
}

void scheduler::best_status(const int best){
	this->best_for_print = best;
	print_status();
}

void scheduler::curr_status(const int curr){
	this->print_cnt++;
	this->curr_for_print = curr;
	if(print_cnt == 10){
		print_cnt = 0;
		print_status();
	}
}

void scheduler::print_status(){
	(cout << "\r" << "Current solution: p = " << this->curr_for_print << " \tBest solution: p = " << this->best_for_print).flush() << " ";
}

void scheduler::metaheuristic_done(){
	cout << " Metahueristic ran out of time." << endl;
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
//	debugf(pq.size());

	auto next_mutator = this->random ? get_next_mutator() : [](vector<port_out_t*>& arg) {};
	
	percent_set(pq.size(), "Creating initial solution:");
	
	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second;
		pq.pop(); // ignore .first

		for (timeslot t = 0;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
//				n.router(c->from)->local_in_schedule.add(c, t);
				break;
			}
		}
		percent_up(pq.size());
	}
	n.updatebest();
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



	auto next_mutator = get_next_mutator();


	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second;
		pq.pop(); // ignore .first

		for (timeslot t = 0;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
//				n.router(c->from)->local_in_schedule.add(c, t);
				break;
			}
		}
	}
	n.updatebest();
}


////////////////////////////////////////////////////////////////////////////////

s_bad_random::s_bad_random(network_t& _n) : scheduler(_n) {
}

void s_bad_random::run() {
	util::srand();

	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
//		int hops = n.router(c.from)->hops[c.to];
//		pq.push(make_pair(hops, &c));
		pq.push(make_pair(util::rand(), &c));
	});
	debugf(pq.size());



	auto next_mutator = get_next_mutator();

	timeslot t_start = 0;

	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second; pq.pop(); // ignore .first

		for (timeslot t = t_start;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
//				n.router(c->from)->local_in_schedule.add(c, t);
				t_start = t + n.router(c->from)->hops.at(c->to) + 1;
				break;
			}
		}
	}
	n.updatebest();
}


////////////////////////////////////////////////////////////////////////////////

s_lns::s_lns(network_t& _n) : scheduler(_n) {
	assert(&_n == &n);
	scheduler *s = ::get_heuristic(global::opts->alns_inital, this->n);
	
	s->run(); // make initial solution
	s->verify(false);
	
	best = curr = n.p();
	curr_status(best);
	best_status(best);
	
	this->choose_table.push_back({0.5, &s_lns::choose_random});
	this->choose_table.push_back({1.0, &s_lns::choose_dom_paths});
	this->choose_table.push_back({1.0, &s_lns::choose_dom_rectangle});
	this->normalize_choose_table();
}

void s_lns::punish_or_reward() {
	this->choose_table[this->chosen_adaptive].first *= std::sqrt((float(best)/curr));
	this->normalize_choose_table();
//	debugf(this->choose_table);
}


void s_lns::normalize_choose_table() {
	float sum = 0;
	for (int i = 0; i < choose_table.size(); i++) {
		sum += choose_table[i].first;
	}

	for (int i = 0; i < choose_table.size(); i++) {
		choose_table[i].first /= sum;
	}
}

void s_lns::run() 
{
	// noget := asdasdads
	// choose:
	//	1. random - Done
	//  2. dense routers
	//  3. dominating paths + dependencies
	//  4. functor next_mutator: always route towards least dense router
	//  5. finite lookahead

	// destroy noget
	// repair igen
	
	for (time_t t0 = time(NULL);  time(NULL) <= t0 + global::opts->run_for;  ) 
	{
		this->destroy();
		this->repair();
		this->verify(false);

		curr = n.p();
		this->punish_or_reward();
		curr_status(curr);

		if (curr < best) {
			best = curr;
			this->n.updatebest();
			best_status(best);
		}
	}
	metaheuristic_done();
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

std::set<const channel*> s_lns::find_dom_paths()
{
	std::set<const channel*> dom;
	timeslot p = this->n.p();

	// Find the dominating paths first
	for_each(this->n.links(), [&](link_t* t) {
		if (t->local_schedule.has(p - 1)) {
			assert(t->local_schedule.max_time() <= p - 1);
			const channel *c = t->local_schedule.get(p - 1);
			assert(c != NULL);
			dom.insert(c);
		}
	});

	return dom;
}

std::set<const channel*> s_lns::choose_dom_paths() 
{
	std::set<const channel*> dom = this->find_dom_paths(); // the dominating path + its "dependencies"
	std::set<const channel*> ret = dom; // the dominating path + its "dependencies"
	
	for_each(dom, [&](const channel *dom_c){
		std::set<const channel*> chns = this->find_depend_path(dom_c);
		ret.insert(chns.begin(), chns.end());
	});

	return ret;
}

std::set<const channel*> s_lns::choose_dom_rectangle() 
{
	std::set<const channel*> dom = this->find_dom_paths(); // the dominating path + its "dependencies"
	std::set<const channel*> ret = dom;
	
	for_each(dom, [&](const channel *dom_c){
		std::set<const channel*> chns = this->find_depend_rectangle(dom_c);
		ret.insert(chns.begin(), chns.end());
	});

	return ret;
}

std::set<const channel*> s_lns::find_depend_path(const channel* dom) 
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

std::set<const channel*> s_lns::find_depend_rectangle(const channel* c) {
	std::set<const channel*> ret;
	std::set<const link_t*> links;
	
	std::queue<router_t*> Q;
	std::map<router_t*, bool> marked;
	
	Q.push(this->n.router(c->from));
	marked[this->n.router(c->from)] = true;
	
	while(!Q.empty()) {
		router_t *t = Q.front(); Q.pop();
		if (t->address == c->to) continue; // we've reached the destination
		auto &next = t->next.at(c->to);
		
		for_each(next, [&](port_out_t* p){
			if(p->has_link()) {
				Q.push(&(p->link().sink.parent));
				links.insert(&(p->link()));
			}
		});
	}
	
	for_each(links, [&](const link_t* l) {
		std::set<const channel*> channels = l->local_schedule.channels();
		for (auto it = channels.begin(); it != channels.end(); ++it ) {
			ret.insert(c);
		}
	});
	
	return ret;
}


void s_lns::destroy() {
	std::set<const channel*> chosen;
	
	float unit_rand = float(util::rand()) / UTIL_RAND_MAX; // random float in interval [0;1[
	float cumm = 0.0;

	assert(0.0 <= unit_rand && unit_rand <= 1.0);
	
	for (int i = 0; i < this->choose_table.size(); i++) {
		const float a = cumm;
		const float b = a + this->choose_table[i].first;
						
		cumm += this->choose_table[i].first;

		if (a <= unit_rand && unit_rand <= b) {
			chosen = (this->*(choose_table[i].second))(); // run the channel-choosing function
			assert(!chosen.empty());
			this->chosen_adaptive = i;
			break;
		}
	}
	
	if (chosen.empty()) {
		const int i = util::rand() % choose_table.size();
		chosen = (this->*(choose_table[i].second))(); // run the channel-choosing function
	}
	assert(!chosen.empty());
	assert(this->unrouted_channels.empty());
	
	
	for_each(chosen, [&](const channel * c) {
		this->n.ripup_channel(c);
		const int hops = this->n.router(c->from)->hops[c->to]; //+ util::rand() % 2;
		this->unrouted_channels.insert(make_pair(hops, c));
	});
	
	assert(this->unrouted_channels.size() == chosen.size());
}

void s_lns::repair() {

	auto next_mutator = get_next_mutator();
	
	assert(!this->unrouted_channels.empty());	
	int cnt = 0;

	for_each_reverse(this->unrouted_channels, [&](const std::pair<int, const channel*>& p) 
	{
		const channel *c = p.second;
		
		for (int t = 0;; t++) {
			const bool path_routed = this->n.route_channel_wrapper((channel*) c, t, next_mutator);
			if (path_routed) {
				assert(n.router(c->from)->local_in_schedule.get(t) == c);
				assert(c->t_start == t);
				
				cnt++;
				break;
			}		
		}
	});
	
	assert(cnt == this->unrouted_channels.size());
	this->unrouted_channels.clear();
}


////////////////////////////////////////////////////////////////////////////////


scheduler* get_heuristic(options::meta_t meta_id, network_t& n) 
{
	scheduler *s;
	switch (meta_id) 
	{
		case options::GREEDY	: s = new s_greedy(n, false);	break;
		case options::rGREEDY	: s = new s_greedy(n, true);	break;
		case options::RANDOM	: s = new s_random(n);			break;
		case options::BAD_RANDOM: s = new s_bad_random(n);		break;
		case options::ALNS		: s = new s_lns(n);				break;
		default:		ensure(false, "Uknown metaheuristic, or not implemented yet");
	}	
	assert(s != NULL);
	return s;
}

