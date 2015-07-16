/*******************************************************************************
 * Copyright 2012 Mark Ruvald Pedersen <mark@radix63.dk>
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2012 Jaspur Hoejgaard <jaspurh@gmail.com>
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
 
#include "options.h"

options::options(int argc, char *argv[])
// Option defaults:
:	output_dir(""), size(-1), min_bw(1), max_bw(1), topology_type(""), chan_prob(0.5)
{
	bool output = false;
	/* Set options as specified by user */
	static struct option long_options[] =
	{
		{"min-bw",	  required_argument, 0, 'i'},
		{"max-bw",	  required_argument, 0, 'a'},
		{"topology",  required_argument, 0, 't'},
		{"size",	  required_argument, 0, 's'},
		{"channel-probability",	  required_argument, 0, 'p'},
		{"help",	  optional_argument, 0, 'h'},
		{0,0,0,0}
	};
	int option_index = 0;

	for (int c; (c = getopt_long(argc, argv, "o:s:hi:a:t:p:", long_options, &option_index)) != -1;) {
		switch (c) {
			case 'o':   output_dir = optarg; output = true;				break;
			case 's':   size = ::lex_cast<int>(string(optarg));			break;
			case 'i':   min_bw = ::lex_cast<int>(string(optarg));		break;
			case 'a':   max_bw = ::lex_cast<int>(string(optarg));		break;
			case 't':   topology_type = string(optarg);					break;
			case 'p':   chan_prob = ::lex_cast<float>(string(optarg));	break;
			case 'h':   print_help();									break;
			default:	ensure(false, "Unknown flag " << c << ".");
		}
	}
	
	if (argc < 2){ // If no comand line options are given, the help menu is printet.
		print_help();
	}

	/* Some input validation */
	if (output) {
		ensure(output_dir.size() > 0, "Empty output directory given.");
	}
	ensure( size > 0, "Size parameter not specified.");
	ensure(min_bw >= 1, "The minimum bandwidth must not be negative or zero.");
	ensure(max_bw >= min_bw, "The maximum bandwidth must be greater or equal to the minimum bandwidth.");
	ensure(topology_type.size() > 0, "Empty topology given.");
	ensure(chan_prob >= 0, "The channel probability must not be less than zero.");
	ensure(chan_prob <= 1, "The channel probability must not be greater than one.");
}


void options::print_help()
{
	cout << endl << "Help menu for graph_gen" << endl;
	cout << "\tMandatory options:" << endl; 
	print_option('o',"Specify the output directory for the use cases.");
	print_option('s',"size","Specify the side length of the platform.");
	print_option('i',"min-bw","The minimum bandwidth of a scheduled communication channel.");
	print_option('a',"max-bw","The maximum bandwidth of a scheduled communication channel.");
	print_option('t',"topology","The topology of the platform.");
	print_option('p',"channel-probability","The probability of adding a each channel to the schedule.");
	print_option('h',"help","Print this help menu.");
	cout << endl;
	
	delete this;
	return exit(0);
}

void options::print_option(char opt, string text){
	cout << "\t-" << opt << "\t" << text << endl;
}

void options::print_option(char opt, string long_opt, string text){
	if(long_opt.length() > 10) {
		cout << "\t-" << opt << " --" << long_opt << endl;
		cout << "\t\t\t" << text << endl;
	} else {
		cout << "\t-" << opt << " --" << long_opt << "\t" << text << endl << endl;
	}
}
options::~options()
{
	
}


namespace global {
	options* opts;
}


