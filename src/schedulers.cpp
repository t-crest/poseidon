#include <stdlib.h>

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

singleshot_scheduler::singleshot_scheduler(network_t& _n) : n(_n) {
	this->t0 = time(NULL);
	percent = 0.0;
//	best_for_print = 0;
//	curr_for_print = 0;
//	print_cnt = 0;
	initial = 0;
}

void singleshot_scheduler::percent_set(const int init, const string text){
	percent = 0.0;
	initial = init;
	cout << text << endl;
}

void singleshot_scheduler::percent_set(const int init){
	percent = 0.0;
	initial = init;
}

void singleshot_scheduler::percent_up(const int curr){
	
	float curr_percent = (float)curr/initial;
	curr_percent = round(curr_percent*1e4)/1e2; // Rounding to 2 decimal point precision
	if(curr_percent > percent){
		percent = curr_percent;
		printf("\rProgress: %.2f%%",curr_percent);
		fflush(stdout);
		if (int(curr_percent) >= 100)
			cout << " Done." << endl;
	}

}

void singleshot_scheduler::main_run() {
	util::srand();
	this->run();
	this->print_stats();
	this->print_stats_linkutil();
}

void singleshot_scheduler::verify(const bool best){
	if (best)
		ensure(global::opts->save_best, "Can not check best solution, if it is not stored!");
		
	for_each(n.channels(), [&](const channel & c){
		n.check_channel(&c, best);
	});
}

//void scheduler::best_status(const int best){
//	this->best_for_print = best;
//	print_status();
//}

//void scheduler::curr_status(const int curr){
//	this->print_cnt++;
//	this->curr_for_print = curr;
//	if(print_cnt >= 10){
//		print_cnt = 0;
//		print_status();
//	}
//}

//void scheduler::print_status(){
//	(cout << "\r" << "Current solution: p = " << this->curr_for_print << " \tBest solution: p = " << this->best_for_print).flush() << " ";
//}

//void scheduler::metaheuristic_done(){
//	cout << " Metahueristic ran out of time." << endl;
//}

void singleshot_scheduler::print_stats() {
	time_t now = time(NULL);
	global::opts->stat_file 
		<< (now - this->t0) << "\t" 
		<< this->n.curr << endl;
}

void singleshot_scheduler::print_stats_linkutil() {
	global::opts->stat_file << "# Link_util_best\t" << this->n.link_utilization(true) << endl;
//	global::opts->stat_file << "# best\t" << this->best_for_print << endl;
}

////////////////////////////////////////////////////////////////////////////////

s_greedy::s_greedy(network_t& _n, bool _random) : singleshot_scheduler(_n), random(_random) {
}

void s_greedy::run() {
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
		int hops = n.router(c.from)->hops[c.to];
		pq.push(make_pair(hops, &c));
	});
	
	auto next_mutator = this->random ? get_next_mutator() : [](vector<port_out_t*>& arg) {};
	int orig_size = pq.size();
	percent_set(orig_size, "Creating initial solution:");
	
	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second; pq.pop(); 

		for (timeslot t = 0;; t++) {
			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
				break;
			}
		}
		percent_up(orig_size - pq.size());
	}
	n.updatebest();
}

////////////////////////////////////////////////////////////////////////////////

s_cross::s_cross(network_t& _n, float _beta) : singleshot_scheduler(_n), beta(_beta)
{
}

void s_cross::run() 
{
	assert(0.0 <= beta && beta <= 1.0);
	const int k = beta * n.channels().size(); // the number of swaps to perform
	assert(k >= 0);
	
	const int size = n.channels().size();
	
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;
	vector<int> length;
	length.resize(size);
	
	for (int i = 0; i < size; i++) {
		const channel& c = n.channels()[i];
		length[i] = n.router(c.from)->hops[c.to];
	}

	// swap lengths, k times
	for (int i = 0; i < k; i++) {
		const int a = util::rand() % size;
		const int b = util::rand() % size;
		std::swap(length[a], length[b]);
	}
	
	// Add all channels to a priority queue, sorting by their length
	for (int i = 0; i < n.channels().size(); i++) {
		const channel& c = n.channels()[i];
		pq.push(make_pair(length[i], &c));
	}
	
	auto next_mutator = get_next_mutator();
	//percent_set(pq.size(), "Creating initial solution:");
	
	// Routes channels and mutates the network. Long channels routed first.
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second; pq.pop(); 

		for (timeslot t = 0;; t++) {
			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
				break;
			}
		}
		//percent_up(pq.size());
	}
	n.updatebest();
}

////////////////////////////////////////////////////////////////////////////////

s_random::s_random(network_t& _n) : singleshot_scheduler(_n) {
}

void s_random::run() 
{
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
		pq.push(make_pair(util::rand(), &c));
	});
	debugf(pq.size());

	auto next_mutator = get_next_mutator();

	int orig_size = pq.size();
	percent_set(orig_size, "Creating initial solution:");
	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second;
		pq.pop(); // ignore .first

		for (timeslot t = 0;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
				break;
			}
		}
		percent_up(orig_size - pq.size());
	}
	n.updatebest();
}


////////////////////////////////////////////////////////////////////////////////

s_bad_random::s_bad_random(network_t& _n) : singleshot_scheduler(_n) {
}

void s_bad_random::run() 
{
	priority_queue< pair<int/*only for sorting*/, const channel*> > pq;

	// Add all channels to a priority queue, sorting by their length
	for_each(n.channels(), [&](const channel & c) {
		pq.push(make_pair(util::rand(), &c));
	});
	debugf(pq.size());

	auto next_mutator = get_next_mutator();
	
	int orig_size = pq.size();
	percent_set(orig_size, "Creating initial solution:");
	timeslot t_start = 0;

	// Routes channels and mutates the network. 
	while (!pq.empty()) {
		channel *c = (channel*) pq.top().second; pq.pop(); // ignore .first

		for (timeslot t = t_start;; t++) {
			if (n.router(c->from)->local_in_schedule.available(t) == false)
				continue;

			const bool path_routed = n.route_channel_wrapper(c, t, next_mutator);
			if (path_routed) {
				t_start = t + n.router(c->from)->hops.at(c->to) + 1;
				break;
			}
		}
		percent_up(orig_size - pq.size());
	}
	n.updatebest();
}

////////////////////////////////////////////////////////////////////////////////

network_t* meta_scheduler::minor_t::this_n; // UGLY HACK

meta_scheduler::meta_scheduler(network_t& _n) 
:	singleshot_scheduler(_n), 
	iterations(0)
{
	meta_scheduler::minor_t::this_n = &this->n; // UGLY HACK
}
		
void meta_scheduler::main_run() {
	util::srand();
	this->run();
	this->print_stats_linkutil();
}

void meta_scheduler::destroy() {
	chosen_t chosen;
	
	float unit_rand = float(util::rand()) / UTIL_RAND_MAX; // random float in interval [0;1[
	float cumm = 0.0;

	assert(0.0 <= unit_rand && unit_rand <= 1.0);

	/* choose_table is a discrete probillity distrubution (along with function pointers).
	 * Here we find what choose-function to apply, by seeing where unit_rand intersects the cummulative probabillty distribution
	 */
	for (int i = 0; i < this->choose_table.size(); i++) {
		const float a = cumm;
		const float b = a + this->choose_table[i].first;
						
		cumm += this->choose_table[i].first;

		if (a <= unit_rand && unit_rand <= b) {
			chosen = (this->*(choose_table[i].second))(); // run the channel-choosing function
			assert(!chosen.get_set().empty());
			this->chosen_adaptive = i; // remember which choosing function we chose, so we can punish it later
			break;
		}
	}

	/* if no channels selected, try again */
	if (chosen.get_set().empty()) {
		const int i = util::rand() % choose_table.size();
		chosen = (this->*(choose_table[i].second))(); // run the channel-choosing function
	}
	assert(!chosen.get_set().empty());
	assert(this->unrouted_channels.empty());
	
	for_each(chosen.get_set(), [&](const channel_part c) {
		this->n.ripup_channel(c.first, c.second); 
		const int hops = this->n.router(c.second)->hops.at(c.first->to); // hops of the incomplete path
		this->unrouted_channels.insert(make_pair(hops, c));
	});
	
	assert(this->unrouted_channels.size() == chosen.get_set().size());
}

void meta_scheduler::repair() {

	auto next_mutator = get_next_mutator();
	
	assert(!this->unrouted_channels.empty());	
	int cnt = 0;

	for_each_reverse(this->unrouted_channels, [&](const std::pair<int, channel_part>& p) 
	{
		const channel *c = p.second.first;
		
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

void meta_scheduler::punish_or_reward() {
	if (this->iterations >= 1) {
		this->choose_table[this->chosen_adaptive].first *= std::sqrt((float(n.prev)/n.curr));
		this->normalize_choose_table();
	} 
	n.prev = n.curr;
}

void meta_scheduler::normalize_choose_table() {
	float sum = 0;
	for (int i = 0; i < choose_table.size(); i++) {
		sum += choose_table[i].first;
	}

	for (int i = 0; i < choose_table.size(); i++) {
		choose_table[i].first /= sum;
	}
}

meta_scheduler::chosen_t meta_scheduler::choose_random() {
	chosen_t ret;
	int cnt = util::rand() % (int) (0.1 * this->n.channels().size());
	cnt = util::max(cnt, 2);

	while (ret.get_set().size() < cnt) {
		const int idx = util::rand() % this->n.channels().size();
		const channel* c = &(this->n.channels()[idx]);
		ret.insert({c, c->from});
	}

	return ret;
}

meta_scheduler::chosen_t meta_scheduler::find_dom_paths()
{
	return this->find_late_paths(this->n.p()-1);
}

meta_scheduler::chosen_t meta_scheduler::find_late_paths(timeslot top)
{
	chosen_t late; // set of late-ending paths
	timeslot p = this->n.p();

	for_each(this->n.links(), [&](link_t* l) {
		for (timeslot t = top; t < p; t++) { // this loop is iterated maybe once or twice only, since top should be close to p
			if (l->local_schedule.has(t)) {
				assert(l->local_schedule.max_time() <= p - 1);
				const channel *c = l->local_schedule.get(t);
				assert(c != NULL);
				late.insert({c, c->from});
			}
		}
	});

	return late;
}

meta_scheduler::chosen_t meta_scheduler::choose_late_paths() 
{
	chosen_t late = this->find_late_paths(this->n.p()-2);
	chosen_t ret = late; 
	
	for_each(late.get_set(), [&](const channel_part dom_c){
		chosen_t chns = this->find_depend_path(dom_c.first);
		ret.insert(chns.get_set().begin(), chns.get_set().end());
	});

	return ret;
}

meta_scheduler::chosen_t meta_scheduler::choose_dom_paths() 
{
	chosen_t dom = this->find_dom_paths(); // the dominating path + its "dependencies"
	chosen_t ret = dom; // the dominating path + its "dependencies"
	
	for_each(dom.get_set(), [&](const channel_part dom_c){
		chosen_t chns = this->find_depend_path(dom_c.first);
		ret.insert(chns.get_set().begin(), chns.get_set().end());
	});

	return ret;
}

meta_scheduler::chosen_t meta_scheduler::choose_dom_rectangle() 
{
	chosen_t dom = this->find_dom_paths(); // the dominating path + its "dependencies"
	chosen_t ret = dom;
	
	for_each(dom.get_set(), [&](const channel_part dom_c){
		chosen_t chns = this->find_depend_rectangle(dom_c.first);
		ret.insert(chns.get_set().begin(), chns.get_set().end());
	});

	return ret;
}


void meta_scheduler::crater_process(const router_id r0, const timeslot t0, meta_scheduler::chosen_t& ret) 
{
	const int radius = 2; // space-time radius :)

	for (int x = -radius; x <= radius; x++) {
	for (int y = -radius; y <= radius; y++) {
		router_id r = r0;
		r.first += x;
		r.second += y;
		if (!this->n.has(r)) continue; // stay inside of matrix
		
		for (int i = 0; i < __NUM_PORTS; i++) {
			port_out_t* p = NULL;
			p = &this->n.router(r)->out((port_id)i);
			if (!p->has_link()) continue;
			

			for (int t = std::max(int(t0)-radius, 0); t <= t0+radius; t++) { // avoid negative time
				if (!p->link().local_schedule.has(t)) continue;

				const channel *c = p->link().local_schedule.get(t);
				ret.insert({c, r}); // rip up channel c, starting at r (note that the same channel may be encountered several times, but due to maxset, the channel will ripped up at its earliest encoutered)
			}			
		}
	}
	}		
	
	
}

meta_scheduler::chosen_t meta_scheduler::choose_dom_crater() 
{
	chosen_t ret;
	
	chosen_t doms = this->find_dom_paths();
	for_each(doms.get_set(), [&](const channel_part dom){

		const channel* c_dom = dom.first;
		const router_id r_epi = c_dom->from;
		const timeslot t_epi = c_dom->t_start;
		assert(dom.second == r_epi);
		
		int len = n.router(c_dom->from)->hops[c_dom->to];
		
//		debugf(dom);
//		debugf(c_dom->t_start);
//		debugf(*c_dom);
//		debugf(t_epi);
//		debugf(len);
		
		timeslot t_curr = t_epi;
		router_id r_curr = r_epi; // start in epi center
		
		// walk along the dominating path
		while (r_curr != c_dom->to) 
		{
			// Process r_curr
			this->crater_process(r_curr, t_curr, ret);
			
			// Find next r_curr
			port_out_t *p = NULL;
			for (int i = 0; i < __NUM_PORTS; i++) {
				if (!n.router(r_curr)->out((port_id)i).has_link())
					continue;

				if (n.router(r_curr)->out((port_id)i).link().local_schedule.is(t_curr,c_dom)) {
					p = &n.router(r_curr)->out((port_id)i);
					break;
				}
			}
			assert(p != NULL);			
			t_curr++;
			r_curr = p->link().sink.parent.address;
		}

		assert(r_curr == c_dom->to);
		
		// Process c_dom->to
		this->crater_process(r_curr, t_curr, ret);
	});

	
	return ret;
}

meta_scheduler::chosen_t meta_scheduler::find_depend_path(const channel* dom) 
{
	router_id curr = dom->from;
	chosen_t ret;
	
	for (int i = 0; i < __NUM_PORTS; i++) {
		port_id p = (port_id)i;
		
		if (!n.router(curr)->out(p).has_link()) continue;
		auto time = n.router(curr)->out(p).link().local_schedule.time(dom);
		if (!time) continue;
		
		timeslot max = *time;
		for (timeslot t = 0; t <= max; t++) {
			if (!n.router(curr)->out(p).link().local_schedule.has(t)) continue;
			const channel *c = n.router(curr)->out(p).link().local_schedule.get(t);
			ret.insert({c, c->from});
		}
		curr = n.router(curr)->out(p).link().sink.parent.address;
	}
	
	return ret;
}

meta_scheduler::chosen_t meta_scheduler::find_depend_rectangle(const channel* c) {
	chosen_t ret;
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
			ret.insert({c, c->from});
		}
	});
	
	return ret;
}

void meta_scheduler::print_stats() {
	static time_t prev = 0;
	time_t now = time(NULL);
	if (prev + 1 <= now) {
		prev = now;

		global::opts->stat_file 
			<< (now - this->t0) << "\t" 
			<< this->n.curr << "\t" 
			<< this->n.best << "\t" 
			<< this->iterations << "\t" 
			<< this->choose_table << endl; 
	}
}

////////////////////////////////////////////////////////////////////////////////

s_alns::s_alns(network_t& _n) : meta_scheduler(_n) {
	assert(&_n == &n);
	singleshot_scheduler *s = ::get_heuristic(global::opts->meta_inital, this->n);
	s->run(); // make initial solution
	n.updatebest();
	
	this->choose_table.push_back({1.0, &s_alns::choose_random});
	this->choose_table.push_back({1.0, &s_alns::choose_late_paths});
	this->choose_table.push_back({1.0, &s_alns::choose_dom_paths});
	this->choose_table.push_back({1.0, &s_alns::choose_dom_rectangle});
	this->choose_table.push_back({1.0, &s_alns::choose_dom_crater});
	this->normalize_choose_table();
}


void s_alns::run() 
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
	percent_set(global::opts->run_for, "Applying ALNS");
	for (; time(NULL) <= this->t0 + global::opts->run_for;  ) 
	{
//		debugf(time(NULL)- this->t0);
		
		
		percent_up(time(NULL)- this->t0 );
		
		this->destroy();
		this->repair();

		this->n.updatebest();
		this->punish_or_reward();
		
		this->print_stats();
		this->iterations++;
	}
}

////////////////////////////////////////////////////////////////////////////////

s_grasp::s_grasp(network_t& _n) : meta_scheduler(_n) {
	assert(&_n == &n);
	
	this->choose_table.push_back({1.0, &s_grasp::choose_late_paths});
	this->choose_table.push_back({1.0, &s_grasp::choose_dom_paths});
	this->choose_table.push_back({1.0, &s_grasp::choose_dom_rectangle});
	this->choose_table.push_back({1.0, &s_grasp::choose_dom_crater});
	this->normalize_choose_table();
}


void s_grasp::run() 
{
	for (; time(NULL) <= this->t0 + global::opts->run_for; ) 
	{
		this->n.clear(); // make sure nothing has been scheduled

		s_cross s(this->n, global::opts->beta_percent); 
		s.run(); // make initial solution

		// Also check initial sol
		this->n.updatebest();
		
		
		// Local search
		this->destroy();
		this->repair();
		this->punish_or_reward();

		this->n.updatebest();

		
		this->print_stats();
		this->iterations++;
	}
	debugf(n.p_best());
}

////////////////////////////////////////////////////////////////////////////////


singleshot_scheduler* get_heuristic(options::meta_t meta_id, network_t& n) 
{
	singleshot_scheduler *s;
	switch (meta_id) 
	{
		case options::GREEDY	: s = new s_greedy(n, false);	break;
		case options::rGREEDY	: s = new s_greedy(n, true);	break;
		case options::RANDOM	: s = new s_random(n);			break;
		case options::BAD_RANDOM: s = new s_bad_random(n);		break;
		case options::ALNS		: s = new s_alns(n);			break;
		case options::GRASP		: s = new s_grasp(n);			break;
		default:		ensure(false, "Uknown metaheuristic, or not implemented yet");
	}	
	assert(s != NULL);
	return s;
}

