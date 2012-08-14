#include "parser.hpp"

parser::parser(string file) {
	xml_document doc;
	xml_parse_result foo = doc.load_file(file.c_str());
	ensure(foo.status != status_file_not_found, "File " << file << " could not be found");

	xml_node topology = doc.child("topology");
	xml_node channels = doc.child("channels");
	xml_node graph = topology.child("graph");
	ensure(!topology.empty(), "File " << file << " has no topology");
	ensure(!channels.empty(), "File " << file << " has no channels");
	ensure(!graph.empty(), "File " << file << " has no topology-graph");
	
	const int cols = ::lex_cast<int>(topology.attribute("width").value());
	const int rows = ::lex_cast<int>(topology.attribute("height").value());
	ensure(cols > 0, "Width must be positive");
	ensure(rows > 0, "Height must be positive");
	
	this->n = new network_t(rows, cols);

	const string graph_type = graph.attribute("type").value();
	if (graph_type == "arbitary") {
		this->parse_arbitary(graph);
	} else if (graph_type == "bitorus") {
		ensure(cols == rows, "Graph does not qualify to be bi-torus");
		this->create_bitorus();
		//ensure(false, "Not implemented yet");
	} else if (graph_type == "mesh") {
		ensure(cols == rows, "Graph does not qualify to be mesh");
		this->create_mesh();
	} else {
		ensure(false, "Graph type not recognized");
	}

	this->n->shortest_path(); // Calculate all the shortests paths, and store in routing tables

	const string channel_type = channels.attribute("type").value();
	if (channel_type == "arbitrary") {
		for (EACH_TAG(node_itr, "channel", channels)) {
			channel c = this->parse_channel(node_itr);

			ensure(this->n->has(c.from), "Network does not have router " << c.from << " used in channel.");
			ensure(this->n->has(c.to), "Network does not have router " << c.to << " used in channel.");

			bool pathexist = !this->n->router(c.from)->next[c.to].empty();
			ensure(pathexist, "The path from " << c.from << " to " << c.to << " is not present in the network.");
			for(int i = 0; i < c.bandwidth; i++){
				this->n->specification.push_back(c);
			}
		}
	} else if (channel_type == "all2all") {
		this->create_all2all();
	} else {
		ensure(false, "Channel type not recognized");
	}


}

void parser::parse_arbitary(xml_node& graph) {
	for (EACH_TAG(node_itr, "link", graph)) {
		const router_id r1 = get_attr<router_id > (node_itr, "source");
		const router_id r2 = get_attr<router_id > (node_itr, "sink");

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
	}
}

void parser::create_mesh() {
	link_t *l;
	for (int i = 0; i < n->cols(); i++) {
		for (int j = 0; j < n->rows(); j++) {
			if (i == 0) {
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
			} else if (i == n->cols() - 1) {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
			} else {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
			}
			if (j == 0) {
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
			} else if (j == n->rows() - 1) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
			} else {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
			}
		}
	}
}


void parser::create_bitorus() {
	link_t *l;
	for (int i = 0; i < n->cols(); i++) {
		for (int j = 0; j < n->rows(); j++) {
			if (i == 0) {
				l = n->add(n->add({i, j})->out({W}), n->add({n->cols() - 1, j})->in({E}));
				l->wrapped = true;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
			} else if (i == n->cols() - 1) {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({E}), n->add({0, j})->in({W}));
				l->wrapped = true;
			} else {
				l = n->add(n->add({i, j})->out({W}), n->add({i - 1, j})->in({E}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({E}), n->add({i + 1, j})->in({W}));
				l->wrapped = false;
			}
			if (j == 0) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, n->rows() - 1})->in({S}));
				l->wrapped = true;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
			} else if (j == n->rows() - 1) {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({S}), n->add({i, 0})->in({N}));
				l->wrapped = true;
			} else {
				l = n->add(n->add({i, j})->out({N}), n->add({i, j - 1})->in({S}));
				l->wrapped = false;
				l = n->add(n->add({i, j})->out({S}), n->add({i, j + 1})->in({N}));
				l->wrapped = false;
			}
		}
	}
}

channel parser::parse_channel(xml_node& chan) {
	channel ret;

	const router_id r1 = get_attr<router_id > (chan, "from");
	const router_id r2 = get_attr<router_id > (chan, "to");
	const int bw = get_attr<int>(chan, "bandwidth");

	ret.from = r1;
	ret.to = r2;
	ret.bandwidth = bw;

	ensure(r1 != r2, "Channel from " << r1 << " to " << r2 << " has same source and destination.");
	return ret;
}

void parser::create_all2all(){
	for_each(this->n->routers(),[&](router_t *r1){
		for_each(this->n->routers(),[&](router_t *r2){
			if(r1 != r2){
				channel c;
				c.from = r1->address;
				c.to = r2->address;
				c.bandwidth = 1;
				this->n->specification.push_back(c);
				
			}
			
		});
	});
//	debugf(this->n->specification.size());
	
}

