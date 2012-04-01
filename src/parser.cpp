#include "parser.hpp"

parser::parser(string file) 
{
	xml_document doc;
	xml_parse_result foo = doc.load_file(file.c_str());
	ensure(foo.status != status_file_not_found, "File " << file << " could not be found");
	
	xml_node topology =	doc.child("topology");
	xml_node channels =	doc.child("channels");
	xml_node graph =	topology.child("graph");
	ensure(!topology.empty(),	"File " << file << " has no topology");
	ensure(!channels.empty(),	"File " << file << " has no channels");
	ensure(!graph.empty(),		"File " << file << " has no topology-graph");

	const int cols = ::lex_cast<int>(topology.attribute("width").value());
	const int rows = ::lex_cast<int>(topology.attribute("height").value());
	ensure(cols > 0, "Width must be positive");
	ensure(rows > 0, "Height must be positive");
	
	this->n = new network_t(rows, cols);
	
	const string graph_type = graph.attribute("type").value();
	if (graph_type == "arbitary") {
		this->parse_arbitary(graph);
	} else if (graph_type == "bitorus") {
		ensure(false, "Not implemented yet");
	} else {
		ensure(false, "Graph type not recognized");
	}

	this->n->shortest_path();	// Calculate all the shortests paths, and store in routing tables
	
	for (EACH_TAG(node_itr, "channel", channels)) { 
		channel c = this->parse_channel(node_itr);
//		debugf(c.from);
//		debugf(c.to);
//		debugf(c.bandwidth);
	}
}

void parser::parse_arbitary(xml_node& graph) {
	for (EACH_TAG(node_itr, "link", graph)) { 
		const router_id r1 = get_attr<router_id>(node_itr, "source");
		const router_id r2 = get_attr<router_id>(node_itr, "sink");
		
		const router_id absdiff = abs(r1 - r2); // element-wise abs
		const bool same_row = (absdiff.second == 0); // equal y coords
		const bool same_col = (absdiff.first == 0); // equal x coords

		ensure(same_col | same_row, "Router " << r1 << " and " << r2 << " must be in same row or column");
		assert(same_col ^ same_row && "Can not be in same row and col at the same time");

		const int dist = util::max(absdiff.first, absdiff.second);
		const bool short_link = (dist == 1);
		const bool long_link  = (dist == (same_row ? n->cols():n->rows())-1) & !short_link;
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

channel parser::parse_channel(xml_node& chan) {
	channel ret;

	for (ALL_ATTR(attr_itr, chan)) {
		const string key = attr_itr.name(); // Name of XML attribute
		const string val = attr_itr.value(); // Value of XML attribute

		if (key == "from"){				ret.from = ::lex_cast<router_id>(val);} 
		else if (key == "to"){			ret.to = ::lex_cast<router_id>(val);} 
		else if (key == "bandwidth"){	ret.bandwidth = ::lex_cast<int>(val);}
	}
		
	return ret;
}

