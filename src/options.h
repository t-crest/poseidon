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
 
#ifndef OPTIONS_H
#define	OPTIONS_H

#include "util.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <getopt.h>
#include "file.hpp"
#include "lex_cast.h"


class options {
public:
	options(int argc, char *argv[]);
	~options();

	enum meta_t {GREEDY, rGREEDY, CROSS, RANDOM, BAD_RANDOM, GRASP, ALNS, ERR};
	meta_t metaheuristic;
	std::string input_platform;
	std::string input_com;
	std::string output_file;
	bool draw;
	bool cal_stats;
	meta_t meta_inital;
	bool save_best;
	time_t run_for;
	std::ofstream stat_file;
	float beta_percent;
	float argo_version;
	
private:
	meta_t parse_meta_t(std::string str);
	meta_t parse_init_t(std::string str);
	std::string get_stat_name(int argc, char *argv[]);
	void open_stat_file(int argc, char *argv[], char *dir);
    void print_help();
    void print_option(char opt, std::string text);
	void print_option(char opt, std::string long_opt, std::string text);
};

namespace global {
	extern options* opts;
}
#endif	/* OPTIONS_H */
