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
#include <array>
#include <stack>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


int main(int argc, char* argv[]) 
{
	global::opts = new options(argc, argv);

	parser p(global::opts->input_file);
	network_t& n = *(p.n);

	
	if (global::opts->draw) draw_network(n); // draw network before scheduling anything
	
	
	singleshot_scheduler *s = ::get_heuristic(global::opts->metaheuristic, n);
	s->main_run();
	s->verify( global::opts->save_best );
	cout << "Schedule verified." << endl;
	debugf(n.link_utilization( global::opts->save_best ));
	cout << "Best schedule period: " << n.best << endl;
	

	if (global::opts->draw) draw_schedule(n); // finally draw the schedule itself 
	
	if (!global::opts->output_dir.empty())
	{
		cout << "Printing shedule...";
		xmlOutput* o = new xmlOutput(global::opts->output_dir);
		o->output_schedule(n);
		cout << "Done." << endl;
	}

	delete global::opts;
	return 0;
}
