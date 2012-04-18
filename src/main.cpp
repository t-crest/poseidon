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
	
	{
		scheduler *s = ::get_heuristic(global::opts->metaheuristic, n);
		s->run();
		s->verify( global::opts->save_best );
		cout << "Schedule verified." << endl;
		debugf(n.link_utilization( global::opts->save_best ));
	}
	

	if (global::opts->draw) draw_schedule(n); // finally draw the schedule itself 

	return 0;
}
