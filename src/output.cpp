#include "output.hpp"


void draw_network(network_t& n) 
{
	using snts::file;
	
	draw d(n);
	file f("network.svg", fstream::out);
	f << d.root.toString() << "\n";
}

void draw_schedule(network_t& n) 
{
	using snts::file;
	
	timeslot length = n.p_best();
	debugf(n.p());
	debugf(n.p_best());
	for (timeslot t = 0; t < length; t++) {
		debugf(t);
		draw d(n, t);
		file f("./cartoon/t" + ::lex_cast<string > (t) + ".svg", fstream::out);
		f << d.root.toString() << "\n";
	}
}
