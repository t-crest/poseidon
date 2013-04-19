#include "output.hpp"

using namespace std;

namespace snts {

/**
 * Draws a picture of the network topology and output it to an svg file.
 * @param n The data structure representing the network.
 */
void draw_network(const network_t& n) 
{
	using snts::file;
	
	draw d(n);
	file f("network.svg", fstream::out);
	f << d.root.toString() << "\n";
}

/**
 * Draws a picture of the network topology and the communication channels for each timeslot of the generated schedule.
 * @param n The data structure representing the network.
 */
void draw_schedule(const network_t& n) 
{
	using snts::file;
	
	timeslot length = n.p_best();
	for (timeslot t = 0; t < length; t++) {
		draw d(n, t);
		file f("./cartoon/t" + ::lex_cast<string > (t) + ".svg", fstream::out);
		f << d.root.toString() << "\n";
	}
}

}