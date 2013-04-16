#include "output.hpp"
#include "draw.hpp"
#include "svg.h"
#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "file.hpp"
#include "draw.hpp"
#include "options.h"
#include "schedulers.hpp"
#include "IOutput.h"
#include "vhdlOutput.h"
#include "xmlOutput.h"
#include "stats.hpp"
#include <array>
#include <stack>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>


using namespace std;

int main(int argc, char* argv[]) 
{
	// Parsing options
	global::opts = new options(argc, argv);
	
	// Parsing input files
	parser p(global::opts->input_platform,global::opts->input_com);
	snts::network_t& n = *(p.n);
	
	// Initializing the stat
	snts::stats* b = new snts::stats(n);
	if (global::opts->draw) snts::draw_network(n); // draw network before scheduling anything
	
	snts::singleshot_scheduler *s = snts::get_heuristic(global::opts->metaheuristic, n, b);
	s->main_run();
	s->verify( global::opts->save_best );
	cout << "Schedule verified." << endl;
	cout << "Best schedule period: " << n.best << endl;
	
	if (global::opts->cal_stats) b->print_stats(); // The statistics of the schedule is printed
	
	if (global::opts->draw) {
		cout << "Drawing shedule...";
		snts::draw_schedule(n); // finally draw the schedule itself 
		cout << "Done." << endl;
	} 
	if (!global::opts->output_file.empty())
	{
		cout << "Printing shedule...";
		snts::xmlOutput* o = new snts::xmlOutput(global::opts->output_file);
		o->output_schedule(n);
		cout << "Done." << endl;
	}

	delete global::opts;
	return 0;
}
