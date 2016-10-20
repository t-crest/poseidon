/*******************************************************************************
 * Copyright 2012 Mark Ruvald Pedersen <mark@radix63.dk>
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2012 Jaspur Hoejgaard <jaspurh@gmail.com>
 * Copyright 2013 Technical University of Denmark, DTU Compute.
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 ******************************************************************************/
 
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
#include <signal.h>


using namespace std;

void exit_handler(int s){
	global::opts->run_for = 0;
	printf("\nScheduler is halting.\n");
}

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

	// Setup handler to catch ctrl-C
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = exit_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	

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
