#include <cassert>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>

typedef unsigned int uint;
//typedef int channel_t;
typedef uint timeslot;
typedef uint coord;
typedef std::pair<coord,coord> router_id;

template <class T>
class matrix {
	const uint row;
	const uint col;
	std::vector<T> data;

public:
	matrix(uint _row, uint _col) 
	:	row(_row), col(_col)
	{
		std::assert(this->row > 0);
		std::assert(this->col > 0);
		data.resize(this->row * this->col);
	}

	inline T& operator() (uint r, uint c) {
		std::assert(0 <= r && r <= this->row);
		std::assert(0 <= c && c <= this->col);
		return this->data.at(c*this->row + r);
	}

	inline T operator() (uint r, uint c) const {
		std::assert(0 <= r && r <= this->row);
		std::assert(0 <= c && c <= this->col);
		return this->data.at(c*this->row + r);
	}

	template <typename X, typename Y>
	inline T& operator() (std::pair<X,Y> arg) {
		return (*this)(arg.fist, arg.second);
	}
};

class schedule {

	enum ports {N,S,E,W,L, __NUM_PORTS} ;

	class channel_t {
		router_id src;
		router_id dest;
		int bandwidth; // We desire so many packets in the period p.
		// Add Response delay
	};

	class link {
		boost::optional<channel_t*> channel; // The channel which is routed onto this link.
//		router_id next; // Destination node of the link.
	public:
		
	};

	class router {
		bool dual_ported;
		boost::optional<link> local_in;
		boost::optional<link> outputs[__NUM_PORTS];
		boost::optional<router_id> inputs[__NUM_PORTS];
	};

	class frame {
		matrix<router> routers;
	};

	std::vector<frame> frames;
	matrix<std::vector<router_id>> network_topology;

public:
	schedule(uint rows, uint cols)
	:	network_topology(rows,cols)
	{

	}

	void add_directed_link(router_id src, ports src_port, router_id dest){
		auto &src_links = this->network_topology(src);
		src_links.at(src_port) = dest;
	}



	void insert_link(router_id current, ports output, timeslot slot, channel_t chan) {

	}

	boost::optional<channel_t> read_link(router_id current, ports output, timeslot slot){

	}

	void free_link(router_id current, ports output, timeslot slot){

	}


	std::vector<ports> get_next_links(router_id current, timeslot slot, router_id dest){

	}

	router_id get_previous_router(router_id current, ports input, timeslot slot){
	
	}














};