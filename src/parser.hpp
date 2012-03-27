#ifndef PARSER_HPP
#define	PARSER_HPP

#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "schedule.hpp"
#include "pugixml.hpp"

#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;
using namespace pugi;

#define EACH_TAG(ITR, TAG, GRAPH)	xml_node ITR = GRAPH.child(TAG); ITR; ITR = ITR.next_sibling(TAG)
#define ALL_ATTR(ITR, NODE)			xml_attribute ITR = NODE.first_attribute(); ITR; ITR = ITR.next_attribute()


class parser {
public:
	network_t *n;
	parser(string file);
	
private:
	
	template<typename T>
	T get_attr(xml_node& xn, const string& attr_name) {
		auto attr = xn.attribute(attr_name.c_str());
		ensure(!attr.empty(), "XML attribute '"<<attr_name<<"' not found under '"<<xn.name()<<"' tag");
		return ::lex_cast<T>(attr.value());
	}
	
	void parse_arbitary(xml_node& graph);
	channel parse_channel(xml_node& chan);	
};

#endif	/* PARSER_HPP */

