/* 
 * File:   bounds.cpp
 * Author: Rasmus Bo Sorensen
 * 
 * Created on 15. april 2013, 10:35
 */

#include "stats.hpp"

namespace snts {

stats::stats(const snts::network_t& _n) : n(_n) {
	percent = 0.0;
	initial = 0;
}

void stats::print_stats(){
	int high_bound = 0;
	int io_bound = print_io_bound();
	if (io_bound > high_bound) {high_bound = io_bound;}
	int cap_bound = print_cap_bound();
	if (cap_bound > high_bound) {high_bound = cap_bound;}
	//int bisect_bound = print_bisect_bound();
	//if (bisect_bound > high_bound) {high_bound = bisect_bound;}
	
	print_schedule_overlap();
	print_link_util();
}

int stats::print_schedule_overlap(){
	int schedule_overlap = 2*n.get_router_depth();
	std::cout << "The schedule overlap: " << schedule_overlap << std::endl;
	global::opts->stat_file << "# Schedule-overlap\t" << schedule_overlap << std::endl;
	return schedule_overlap;
}

int stats::print_io_bound(){
	int io_bound = n.max_io_activity();
	std::cout << "The lower IO bound: " << io_bound << std::endl;
	global::opts->stat_file << "# IO-bound\t" << io_bound << std::endl;
	return io_bound;
}

int stats::print_cap_bound(){
	int links = n.links_in_topology();
	timeslot used_links = n.occupied_links(true);
	int cap_bound = ceil(used_links/links);
	std::cout << "The lower capacity bound: " << cap_bound << std::endl;
	global::opts->stat_file << "# Capacity-bound\t" << cap_bound << std::endl;
	return cap_bound;
}

int stats::print_bisect_bound(){
	int bisect_bound = 0;
	std::cout << "The lower bisection bound: " << bisect_bound << std::endl;
	global::opts->stat_file << "# Bisection-bound\t" << bisect_bound << std::endl;
	return bisect_bound;
}

int stats::print_link_util(){
	float link_util = link_utilization(true)*100; // We want percent
	std::cout << "The link utilization: " << std::setprecision(4) << link_util << "%" << std::endl;
	return link_util;
}

/**
 * This function calculates the utilization of links in a schedule.
 * @param best If true the function uses the best observed schedule, otherwise the current schedule is used.
 * @return The calcualted link utilization as a float value between 1 and 0.
 */
float stats::link_utilization(bool best) {

	const timeslot length = (best ? n.p_best() : n.p());
	const timeslot max_links_used = n.links_in_topology() * length;
	timeslot links_used = n.occupied_links(best);
	
	assert(links_used <= max_links_used);
	return float(links_used)/max_links_used;
}

void stats::percent_set(const int init, const std::string text){
	percent = 0.0;
	initial = init;
	std::cout << text << std::endl;
}

void stats::percent_set(const int init){
	percent = 0.0;
	initial = init;
}

void stats::percent_up(const int curr){
	
	float curr_percent = (float)curr/initial;
	curr_percent = round(curr_percent*1e4)/1e2; // Rounding to 2 decimal point precision
	if(curr_percent > percent){
		percent = curr_percent;
		printf("\rProgress: %.2f%%",curr_percent);
		fflush(stdout);
		if (int(curr_percent) >= 100)
			std::cout << " Done." << std::endl;
	}

}

void stats::write_stats(time_t start) {
	if (!global::opts->cal_stats) return;
	time_t now = time(NULL);
	global::opts->stat_file 
		<< (now - start) << "\t" 
		<< this->n.curr << std::endl;
}

void stats::write_stats(time_t start, std::string text) {
	if (!global::opts->cal_stats) return;
	static time_t prev = 0;
	time_t now = time(NULL);
	if (prev + 1 <= now) {
		prev = now;

		global::opts->stat_file 
			<< (now - start) << "\t" 
			<< n.curr << "\t" 
			<< text << std::endl; 
	}
}

void stats::write_stats_linkutil() {
	if (!global::opts->cal_stats) return;
	global::opts->stat_file << "# Link_util_best\t" << link_utilization(true) << std::endl;
}

stats::~stats() {
}

}
