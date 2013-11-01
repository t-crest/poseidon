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

#include "parser.hpp"

using namespace std;
using namespace pugi;
using namespace snts;

parser::parser(string platform_file, string com_file) {
	// First the platform specification is parsed, later we parse the communication specification.
	xml_document platform_doc;
	xml_parse_result foo = platform_doc.load_file(platform_file.c_str());
	ensure(foo.status != status_file_not_found, "File " << platform_file << " could not be found");

	xml_node platform = platform_doc.child("platform");
	xml_node topology = platform.child("topology");
	ensure(!platform.empty(), "File " << platform_file << " has no platform");
	ensure(!topology.empty(), "File " << platform_file << " has no topology-graph");

	const int cols = get_attr<int>(platform,"width");
	const int rows = get_attr<int>(platform,"height");
	ensure(cols > 0, "Width must be positive");
	ensure(rows > 0, "Height must be positive");

	xml_node router = platform.child("router");
	const int router_depth = get_opt_attr<int>(router,"depth",1); // default router depth is set to 1.
	xml_node link = platform.child("link");
	int link_depth = get_opt_attr<int>(link,"depth",0); // default link depth is set to 0.
	xml_node timeslots = platform.child("timeslots");
	int available_timeslots = get_opt_attr<int>(timeslots,"available",-1); // Default available timeslots is not limited, indicated by -1.

	this->n = new snts::network_t(rows, cols, router_depth, available_timeslots);

	string topology_type = get_opt_attr<string>(topology,"type","NOT FOUND");
	if (topology_type == "NOT FOUND"){
		topology_type = get_attr<string>(topology,"ttype");
	}
	if (topology_type == "custom") {
		this->parse_custom(topology, link_depth);
	} else if (topology_type == "bitorus") {
		ensure(cols == rows, "Graph does not qualify to be bi-torus");
		this->create_bitorus(link_depth);
		//ensure(false, "Not implemented yet");
	} else if (topology_type == "mesh") {
		ensure(cols == rows, "Graph does not qualify to be mesh");
		this->create_mesh(link_depth);
	} else {
		ensure(false, "Graph type not recognized");
	}

	this->n->shortest_path(); // Calculate all the shortests paths, and store in routing tables

	// Time to parse communication specification
	xml_document com_doc;

	if (com_file==""){
		if (!platform_doc.child("communication").empty()) {
			com_doc.reset(platform_doc);
		} else {
			std::cout << "No communication specification: Creating all-to-all communication pattern." << std::endl;
			this->create_all2all(1); // Default one phit
			return;
		}
	} else {
		xml_parse_result bar = com_doc.load_file(com_file.c_str());
		ensure(bar.status != status_file_not_found, "File " << com_file << " could not be found");
	}


	xml_node channels = com_doc.child("communication");
	ensure(!channels.empty(), "File " << com_file << " has no communication channels");

	string channel_type = get_opt_attr<string>(channels,"type","NOT FOUND");
	if (channel_type == "NOT FOUND"){
		channel_type = get_attr<string>(channels,"ctype");
	}
	if (channel_type == "custom") {
		for (EACH_TAG(node_itr, "channel", channels)) {
			channel c = this->parse_channel(node_itr);
		}
	} else if (channel_type == "all2all") {
		const int phits = get_opt_attr<int>(channels, "phits",1); // The default number of phits is set to 1.
		this->create_all2all(phits);
	} else {
		ensure(false, "Channel type not recognized");
	}


}

void parser::parse_custom(xml_node& graph, const int link_depth) {
	for (EACH_TAG(node_itr, "link", graph)) {
		const router_id r1 = get_attr<router_id > (node_itr, "source");
		const router_id r2 = get_attr<router_id > (node_itr, "sink");
		const int depth = get_opt_attr<int>(node_itr,"depth",link_depth);

		const router_id absdiff = abs(r1 - r2); // element-wise abs
		const bool same_row = (absdiff.second == 0); // equal y coords
		const bool same_col = (absdiff.first == 0); // equal x coords

		ensure(same_col | same_row, "Router " << r1 << " and " << r2 << " must be in same row or column");
		assert(same_col ^ same_row && "Can not be in same row and col at the same time");

		const int dist = util::max(absdiff.first, absdiff.second);
		const bool short_link = (dist == 1);
		const bool long_link = (dist == (same_row ? n->cols() : n->rows()) - 1) & !short_link;
		ensure(short_link | long_link, "Router " << r1 << " and " << r2 << " are too far apart to be connected directly");
		assert(short_link ^ long_link);

		port_id p1, p2;
		if (same_col) {
			assert(r1.second != r2.second && "Same coordinates");
			const bool r1_above_r2 = (r1.second < r2.second);
			const bool choose = r1_above_r2 ^ long_link; // change direction if long_link
			p1 = (choose ? S : N);
			p2 = (choose ? N : S);
		}
		if (same_row) {
			assert(r1.first != r2.first && "Same coordinates");
			const bool r1_left_of_r2 = (r1.first < r2.first);
			const bool choose = r1_left_of_r2 ^ long_link; // change direction if long_link
			p1 = (choose ? E : W);
			p2 = (choose ? W : E);
		}

		link_t *l = n->add(n->add(r1)->out(p1), n->add(r2)->in(p2)); // add routers r1 and r2, and the link between them
		l->wrapped = long_link;
		l->depth = depth;
	}
}

void parser::create_mesh(const int link_depth) {
	link_t *l;
	for (int i = 0; i < n->cols(); i++) {
		for (int j = 0; j < n->rows(); j++) {
			if (i == 0) {
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
				l->depth = link_depth;
			} else if (i == n->cols() - 1) {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l->depth = link_depth;
			} else {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
				l->depth = link_depth;
			}
			if (j == 0) {
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
				l->depth = link_depth;
			} else if (j == n->rows() - 1) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l->depth = link_depth;
			} else {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
				l->depth = link_depth;
			}

		}
	}
}


void parser::create_bitorus(const int link_depth) {
	link_t *l;
	for (int i = 0; i < n->cols(); i++) {
		for (int j = 0; j < n->rows(); j++) {
			if (i == 0) {
				l = n->add(n->add({i, j})->out({W}), n->add({n->cols() - 1, j})->in({E}));
				l->wrapped = true;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
				l->depth = link_depth;
			} else if (i == n->cols() - 1) {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({E}), n->add({0, j})->in({W}));
				l->wrapped = true;
				l->depth = link_depth;
			} else {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
				l->depth = link_depth;
			}
			if (j == 0) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, n->rows() - 1})->in({S}));
				l->wrapped = true;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
				l->depth = link_depth;
			} else if (j == n->rows() - 1) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({S}), n->add({i, 0})->in({N}));
				l->wrapped = true;
				l->depth = link_depth;
			} else {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l->depth = link_depth;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
				l->depth = link_depth;
			}
		}
	}
}

channel parser::parse_channel(xml_node& chan) {
	channel c;

	const router_id r1 = get_attr<router_id > (chan, "from");
	const router_id r2 = get_attr<router_id > (chan, "to");
	const int bw = get_opt_attr<int>(chan, "bandwidth",1);
	const int phits = get_opt_attr<int>(chan, "phits",1);

	c.from = r1;
	c.to = r2;
	c.phits = phits;

	ensure(r1 != r2, "Channel from " << r1 << " to " << r2 << " has same source and destination.");
	ensure(this->n->has(c.from), "Network does not have router " << c.from << " used in channel.");
	ensure(this->n->has(c.to), "Network does not have router " << c.to << " used in channel.");

	bool pathexist = !this->n->router(c.from)->next[c.to].empty();
	ensure(pathexist, "The path from " << c.from << " to " << c.to << " is not present in the network.");
	for(int i = 0; i < bw; i++){
		c.channel_id = i;
		this->n->specification.push_back(c);
	}
	return c;
}

void parser::create_all2all(int phits){
	for_each(this->n->routers(),[&](router_t *r1){
		for_each(this->n->routers(),[&](router_t *r2){
			if(r1 != r2){
				channel c;
				c.from = r1->address;
				c.to = r2->address;
				c.phits = phits;
				c.channel_id = 0;
				this->n->specification.push_back(c);
			}
		});
	});

}

