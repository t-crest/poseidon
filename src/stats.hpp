/* 
 * File:   bounds.hpp
 * Author: Rasmus Bo Sorensen
 *
 * Created on 15. april 2013, 10:35
 */

#ifndef STATS_HPP
#define	STATS_HPP

#include "network_t.hpp"
#include <iomanip>

namespace snts {
	
class stats {
private:
	float percent;
	int initial;
public:
	stats(const snts::network_t& _n);
	void print_stats();
    float link_utilization(bool best);
	void percent_set(const int init);
	void percent_set(const int init, const std::string);
	void percent_up(const int curr);
	void write_stats_linkutil();
	void write_stats(time_t start);
	void write_stats(time_t start, std::string text);
	virtual ~stats();
private:
	const snts::network_t& n;
	int print_schedule_overlap();
	int print_io_bound();
	int print_cap_bound();
	int print_bisect_bound();
	int print_link_util();
};

}

#endif	/* STATS_HPP */

