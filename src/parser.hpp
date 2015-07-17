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
 
#ifndef PARSER_HPP
#define	PARSER_HPP

#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "network_t.hpp"
#include "pugixml.hpp"

#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

#define EACH_TAG(ITR, TAG, GRAPH)	pugi::xml_node ITR = GRAPH.child(TAG); ITR; ITR = ITR.next_sibling(TAG)
#define ALL_ATTR(ITR, NODE)			pugi::xml_attribute ITR = NODE.first_attribute(); ITR; ITR = ITR.next_attribute()


//class snts::network_t;
class channel;

class parser {
public:
    snts::network_t *n;
    parser(std::string platform_file, std::string com_file);

private:
    static unsigned int channel_count;

    template<typename T>
    T get_attr(pugi::xml_node& xn, const std::string& attr_name) {
        auto attr = xn.attribute(attr_name.c_str());
        ensure(!attr.empty(), "XML attribute '" << attr_name << "' not found under '" << xn.name() << "' tag");
        return ::lex_cast<T > (attr.value());
    }
	
	template<typename T>
    T get_opt_attr(pugi::xml_node& xn, const std::string& attr_name, T default_val) {
		if (xn.empty()) return default_val;
		auto attr = xn.attribute(attr_name.c_str());
		if (attr.empty()) return default_val;
        return ::lex_cast<T > (attr.value());
    }

    void parse_custom(pugi::xml_node& graph, const int link_depth);
    void create_mesh(const int link_depth);
    void create_bitorus(const int link_depth);
    channel parse_channel(pugi::xml_node& chan);
    void create_all2all(int phits);
};

#endif	/* PARSER_HPP */

