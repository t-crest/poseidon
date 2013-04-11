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

    template<typename T>
    T get_attr(pugi::xml_node& xn, const std::string& attr_name) {
        auto attr = xn.attribute(attr_name.c_str());
        ensure(!attr.empty(), "XML attribute '" << attr_name << "' not found under '" << xn.name() << "' tag");
        return ::lex_cast<T > (attr.value());
    }

    void parse_custom(pugi::xml_node& graph, const int link_depth);
    void create_mesh(const int link_depth);
    void create_bitorus(const int link_depth);
    channel parse_channel(pugi::xml_node& chan);
    void create_all2all(int phits);
};

#endif	/* PARSER_HPP */

