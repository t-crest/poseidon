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

#ifndef XMLOUTPUT_H
#define	XMLOUTPUT_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include "IOutput.h"
#include "lex_cast.h"
#include "pugixml.hpp"

namespace snts {

class xmlOutput: public IOutput {
private:
    std::string output_dir;

	void print_coord(const std::pair<int, int> r, char* co, const int max_dimension);
    void add_latency(const network_t& n, pugi::xml_node* tile, const std::vector<uint>* destinations, router_t* r);
    port_id find_in_port(router_t* r, const channel* out_c, timeslot t);

public:
    xmlOutput(std::string _output_dir);
    ~xmlOutput();

    bool output_schedule(const network_t& n);

};

}

#endif	/* XMLOUTPUT_H */

