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
	
	for (EACH_TAG(node_itr, "channel", channels)) { 
		channel c = this->parse_channel(node_itr);
//		debugf(c.from);
//		debugf(c.to);
//		debugf(c.bandwidth);
	}
}

void parser::parse_arbitary(xml_node& graph) {
	for (EACH_TAG(node_itr, "link", graph)) { 
		const routerport_id source = get_attr<routerport_id>(node_itr, "source");
		const routerport_id sink = get_attr<routerport_id>(node_itr, "sink");
		
//		debugf(source);
//		debugf(sink);

		const router_id r1 = source.first;	const router_id r2 = sink.first;
		const port_id p1 = source.second;	const port_id p2 = sink.second;
		
		n->add(n->add(r1)->out(p1), n->add(r2)->in(p2)); // add routers r1 and r2, and the link between them
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

