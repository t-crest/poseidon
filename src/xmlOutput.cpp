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

#include "xmlOutput.h"

using namespace std;
using namespace pugi;

namespace snts {

bool xmlOutput::output_schedule(const network_t& n)
{
	int numOfNodes = n.routers().size();
	int countWidth = ceil(log2(n.best));

	xml_document doc;
	xml_node schedule = doc.append_child("schedule");
	schedule.append_attribute("length").set_value(n.best);
	schedule.append_attribute("width").set_value(n.cols());
	schedule.append_attribute("height").set_value(n.rows());

	for(vector<router_t*>::const_iterator r = n.routers().begin(); r != n.routers().end(); r++){ // For each router, write Network Adapter Table and Router Table
		// New xml tile
		xml_node tile = schedule.append_child("tile");
		char co [10];
		print_coord((*r)->address,co,sizeof(co));
		tile.append_attribute("id") = co;
		// Vector for saving data to calculate Worst-Case Latencies

		vector<router_id> destinations(n.best,(*r)->address);
//		std::cout << "Tile: " << (*r)->address << std::endl; // Only for debugging
		int router_depth = n.get_router_depth();
		for(timeslot t = 0; t < n.best; t++){ // Write table row for each timeslot
//			if ((*r)->local_in_best_schedule.has(t)) // Only for debugging purposes.
//			{
//				const channel* c = (*r)->local_in_best_schedule.get(t);
//				std::cout << "Timeslot: " << t << " = " << c->to << std::endl;
//			} else{
//				std::cout << "Timeslot: " << t << std::endl;
//			}
			// New timeslot
			xml_node ts = tile.append_child("timeslot");
			ts.append_attribute("value") = t;

			int t_in = t;
			int t_out = (t+router_depth)%(n.best+router_depth);
//			if(t == 0){
//				t_in = n.best-1;
//				t_out = n.best;
//			}
			// Write row in Network Adapter table
			router_id dest_id = (*r)->address;
			router_id src_id = (*r)->address;
			const channel* dest_chan = NULL;
			if ((*r)->local_in_best_schedule.has((t)%n.best)){	// Used to be t+2 to account for pipelining in the network interface.
				dest_chan = (*r)->local_in_best_schedule.get((t)%n.best);
				dest_id = dest_chan->to;	// Used to be t+2 to account for pipelining in the network interface.
			}
			if ((*r)->local_out_best_schedule.has(t))
				src_id = (*r)->local_out_best_schedule.get(t)->from;
			destinations[t] = dest_id;
			// New na
			xml_node na = ts.append_child("na");
			print_coord(src_id,co,sizeof(co));
			na.append_attribute("rx") = co;
			print_coord(dest_id,co,sizeof(co));
			na.append_attribute("tx") = co;
			string route;
			route = n.get_route(dest_chan);
			if(route.length() > 0){
				na.append_attribute("route") = route.c_str();
			}

			// Write row in Router table
			port_id ports[5] = {__NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS};
			// New router
			xml_node router = ts.append_child("router");


			for(int out_p = 0; out_p < __NUM_PORTS-1; out_p++){
				// For all 4 output ports not being the local port.
				if(!(*r)->out((port_id)out_p).has_link()){
					continue; // No outgoing link from the port.
				}
				if(!(*r)->out((port_id)out_p).link().best_schedule.has(t_out)){
					ports[(port_id)out_p] = __NUM_PORTS; // No outgoing channel on link
					continue;
				}
				// If there is a channel comming out of the port, find the
				// input port from which the channel is comming from.
				const channel* out_c =(*r)->out((port_id)out_p).link().best_schedule.get(t_out);
				for(int in_p = 0; in_p < __NUM_PORTS-1; in_p++){
					// For all 4 input ports not being the local port.
					if(!(*r)->in((port_id)in_p).has_link())
						continue; // No link into this port
					if((*r)->in((port_id)in_p).link().best_schedule.has(t_in)){
						const channel* in_c =(*r)->in((port_id)in_p).link().best_schedule.get(t_in);
						if(out_c == in_c){
							// The correct link found
							ports[(port_id)out_p] = (port_id)in_p;
							break;
						}
					}
				}
				if(ports[(port_id)out_p] != __NUM_PORTS){
					continue; // Channel was found on one of the input ports.
				}
				// It should be on the local in port, but we test it anyway.
				if((*r)->local_in_best_schedule.has(t_in)){
					const channel* in_c = (*r)->local_in_best_schedule.get(t_in);
					ensure(out_c == in_c,"Channel rose from nothing like a fenix. Channel: " << out_c);
					// The correct link found
					ports[(port_id)out_p] = L;
				}
			}

			if((*r)->local_out_best_schedule.has(t_out)){ // For the local out port.
				const channel* out_c = (*r)->local_out_best_schedule.get(t_out);
				for(int in_p = 0; in_p < __NUM_PORTS-1; in_p++){
					// For all 4 input ports not being the local port.
					if(!(*r)->in((port_id)in_p).has_link())
						continue; // No link into this port
					if((*r)->in((port_id)in_p).link().best_schedule.has(t_in)){
						const channel* in_c =(*r)->in((port_id)in_p).link().best_schedule.get(t_in);
						if(out_c == in_c){
							// The correct link found
							ports[L] = (port_id)in_p;
							break;
						}
					}
				}
				ensure(ports[L] != __NUM_PORTS,"Channel not allowed to route back into local port. Channel: " << out_c);
//				if(ports[L] == __NUM_PORTS){
//					// If channel was not found on any of the 4 input ports.
//					// It should be on the local in port, but we test it anyway.
//					cout << "Failure: Not allowed to route back in to local." << endl;
//				}
				// and so on...
			}
			for(int p = 0; p < __NUM_PORTS; p++){
				// New output
				xml_node output = router.append_child("output");
				sprintf(co,"%c",p2c((port_id)p)); // Should be snprintf, avoiding buffer overflow
//				sprintf(co,sizeof(co),"%c",p2c((port_id)p));
				output.append_attribute("id") = co;
				sprintf(co,"%c",p2c(ports[(port_id)p])); // Should be snprintf, avoiding buffer overflow
//				sprintf(co,sizeof(co),"%c",p2c(ports[(port_id)p]));
				output.append_attribute("input") = co;
			}

		}
		xml_node latency = tile.append_child("latency");
		// The following for loop is slow and unnecessary, can be changed to improve runtime.
		// What is needed is all the channels with the current router as the source.
		for_each(n.channels(), [&](const channel & c) {
			if(c.from != (*r)->address){
				return; // Channel not from router
			}
			// For each channel from router
			int slotswaittime = 0;
			int channellatency = 0;
			int num_phits = 0;
			double rate = 0.0;
			int late = 0;
			int inlate = 0;
			bool init = true;
			for(int i = 0; i < n.best; i++){
				if(c.to != destinations[i]){
					// Increment latency
					late++;
					continue;
				}
				// Correct destination
				num_phits++; // Used to calculate rate.
				if(init){
					init = false;
					inlate = late;
				}
				if(late > slotswaittime){
					slotswaittime = late;
				}
				late = 0;
			}
			late += inlate;
			if(late > slotswaittime){
				slotswaittime = late;
			}
			slotswaittime += c.phits; // Only in the beginning of a timeslot can a packet be transmitted
			channellatency = (*r)->hops[c.to];
			rate = ((double)num_phits)/n.p_best();
			// Analyze the latency
			xml_node destination = latency.append_child("destination");
			print_coord(c.to,co,sizeof(co));
			destination.append_attribute("id") = co;
			destination.append_attribute("slotwaittime") = slotswaittime;
			destination.append_attribute("channellatency") = channellatency;
			destination.append_attribute("rate") = rate;
		});
	}
	//char co [500];
	//sprintf(co,"%s",); // Should be snprintf, avoiding buffer overflow
	//sprintf(co,sizeof(co),"%soutput.xml",output_dir.c_str());
	doc.save_file(output_dir.c_str());

	delete this;
	return true;
}

void xmlOutput::print_coord(const pair<int, int> r,char* co, const size_t buffer_size){
	sprintf(co,"(%i,%i)",r.first,r.second); // Should be snprintf, avoiding buffer overflow
//	sprintf(co,buffer_size,"(%i,%i)",c.to.first,c.to.second);
}

char xmlOutput::p2c(port_id p){
		char c;
		if (p == N) c = 'N';
		if (p == E)	c = 'E';
		if (p == S)	c = 'S';
		if (p == W)	c = 'W';
		if (p == L)	c = 'L';
		if (p == __NUM_PORTS)	c = 'D';

		return c;
	}

xmlOutput::xmlOutput(string _output_dir) : output_dir(_output_dir){
}

xmlOutput::~xmlOutput(){

}

}
