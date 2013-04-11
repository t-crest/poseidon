#ifndef OUTPUT_HPP
#define	OUTPUT_HPP

#include "draw.hpp"
#include "svg.h"
#include "parser.hpp"
#include "higher_order.h"
#include "lex_cast.h"
#include "util.hpp"
#include "matrix.hpp"
#include "network_t.hpp"
#include "file.hpp"
#include "draw.hpp"
#include "options.h"
#include <array>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <memory>

namespace snts {

void draw_network(const network_t& n);
void draw_schedule(const network_t& n);

}

#endif	/* OUTPUT_HPP */

