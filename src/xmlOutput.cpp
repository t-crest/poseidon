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

	int schedule_overlap = n.get_schedule_overlap();
	int schedule_length = n.p_best()-schedule_overlap;

	xml_document doc;
	xml_node schedule = doc.append_child("schedule");
	schedule.append_attribute("length").set_value(schedule_length);
	schedule.append_attribute("width").set_value(n.cols());
	schedule.append_attribute("height").set_value(n.rows());

	for(vector<router_t*>::const_iterator r = n.routers().begin(); r != n.routers().end(); r++){ // For each router, write Network Adapter Table and Router Table
		// New xml tile
		xml_node tile = schedule.append_child("tile");
		char co [2*max(n.rows(),n.cols())+3];
		print_coord((*r)->address,co,max(n.rows(),n.cols()));
		tile.append_attribute("id") = co;
		// Vector for saving data to calculate Worst-Case Latencies

		vector<uint> destinations(schedule_length,::numeric_limits<uint>::max());
		debugs("Tile: " << (*r)->address);
		int router_depth = n.get_router_depth();
		for(timeslot t = 0; t < schedule_length; t++){ // Write table row for each timeslot
			// New timeslot
			xml_node ts = tile.append_child("timeslot");
			ts.append_attribute("value") = t;

			int t_in = t;
			int t_out = (t+router_depth)%(n.best+router_depth);
			debugs("t_in: " << t_in << " t_out: " << t_out);
			// Write row in Network Adapter table
			router_id dest_id = (*r)->address;
			router_id src_id = (*r)->address;
			const channel* dest_chan = NULL;
			if ((*r)->local_in_best_schedule.has((t)%n.best)){
				debugs( "Timeslot: " << t << " = " << (*r)->local_in_best_schedule.get(t)->to );
				dest_chan = (*r)->local_in_best_schedule.get((t)%n.best);
				dest_id = dest_chan->to;
				destinations[t] = dest_chan->channel_id;
			} else {
				debugs("Timeslot: " << t);
			}
			if ((*r)->local_out_best_schedule.has(t))
				src_id = (*r)->local_out_best_schedule.get(t)->from;

			// New na
			xml_node na = ts.append_child("na");
			print_coord(src_id,co,max(n.rows(),n.cols()));
			na.append_attribute("rx") = co;
			print_coord(dest_id,co,max(n.rows(),n.cols()));
			na.append_attribute("tx") = co;
			string route;
			route = n.get_route(dest_chan);
			if(route.length() > 0){
				na.append_attribute("route") = route.c_str();
				na.append_attribute("chan-id").set_value(dest_chan->channel_id);
				na.append_attribute("pkt-id").set_value(dest_chan->pkt_id);
			}

			// Write row in Router table
			port_id ports[5] = {__NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS};
			// New router
			xml_node router = ts.append_child("router");


			for(int out_p = 0; out_p < __NUM_PORTS-1; out_p++){
				timeslot link_depth;
				debugs("Out port: " << p2c((port_id)out_p));
				// For all 4 output ports not being the local port.
				if(!(*r)->out((port_id)out_p).has_link()){
					continue; // No outgoing link from the port.
				}
				link_depth = (*r)->out((port_id)out_p).link().depth;
				if(!(*r)->out((port_id)out_p).link().best_schedule.has(t_out+link_depth)){
					ports[(port_id)out_p] = __NUM_PORTS; // No outgoing channel on link
					continue;
				}
				// If there is a channel comming out of the port, find the
				// input port from which the channel is comming from.
				const channel* out_c =(*r)->out((port_id)out_p).link().best_schedule.get(t_out+link_depth);
				ports[(port_id)out_p] = find_in_port((*r),out_c,t_in);
			}

			if((*r)->local_out_best_schedule.has(t_out)){ // For the local out port.
				const channel* out_c = (*r)->local_out_best_schedule.get(t_out);
				ports[L] = find_in_port((*r),out_c,t_in);
				ensure(ports[L] != L,"Channel not allowed to route back into local port. Channel: " << out_c);
			}
			for(int p = 0; p < __NUM_PORTS; p++){
				// New output
				xml_node output = router.append_child("output");
				snprintf(co,sizeof(co),"%c",p2c((port_id)p));
				output.append_attribute("id") = co;
				snprintf(co,sizeof(co),"%c",p2c(ports[(port_id)p]));
				output.append_attribute("input") = co;
			}

		}
		add_latency(n,&tile,&destinations,(*r));
	}

	doc.save_file(output_dir.c_str());

	delete this;
	return true;
}

void xmlOutput::print_coord(const pair<int, int> r,char* co, const int max_dimension){
//	sprintf(co,"(%i,%i)",r.first,r.second); // Should be snprintf, avoiding buffer overflow
	const int buffer_size = 2*max_dimension + 3;
	snprintf(co,buffer_size,"(%i,%i)",r.first,r.second);
}

void xmlOutput::add_latency(const network_t& n, xml_node* tile, const vector<uint>* destinations, router_t* r){
	xml_node latency = (*tile).append_child("latency");
	char co[2*max(n.rows(),n.cols())+3];
	char *buf = co; // clang does not allow references to flexible arrays in lambda expressions
	std::vector<bool> printed_chans(n.channels().size(),false);

	int schedule_length = n.p_best()-n.get_schedule_overlap();
	// The following for loop is slow and unnecessary, can be changed to improve runtime.
	// What is needed is all the channels with the current router as the source.
	for_each(n.channels(), [&](const channel & c) {
		if(c.from != r->address){
			return; // Channel not from router
		}
		if (printed_chans[c.channel_id]) {
			return;
		}
		printed_chans[c.channel_id] = true;
		// For each channel from router
		int slotswaittime = 0;
		int channellatency = 0;
		int num_phits = 0;
		double rate = 0.0;
		int late = 0;
		int inlate = 0;
		bool init = true;
		for(int i = 0; i < schedule_length; i++){
			if(c.channel_id != (*destinations)[i]){
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
		channellatency = r->hops[c.to];
		rate = ((double)num_phits)/schedule_length;
		// Analyze the latency
		xml_node destination = latency.append_child("destination");
		print_coord(c.to, buf, max(n.rows(),n.cols()));
		destination.append_attribute("id") = buf;
		destination.append_attribute("slotwaittime") = slotswaittime;
		destination.append_attribute("channellatency") = channellatency;
		destination.append_attribute("chan-id") = c.channel_id;
		destination.append_attribute("chan-bw") = c.ch_bw;
		destination.append_attribute("pkt-len") = c.phits;
		destination.append_attribute("rate") = rate;
	});
}

port_id xmlOutput::find_in_port(router_t* r, const channel* out_c, timeslot t_in){
	port_id ret_port = __NUM_PORTS;
	for(int in_p = 0; in_p < __NUM_PORTS-1; in_p++){
		debugs("In port: " << p2c((port_id)in_p));
		// For all 4 input ports not being the local port.
		if(!r->in((port_id)in_p).has_link())
			continue; // No link into this port
		if(r->in((port_id)in_p).link().best_schedule.has(t_in)){
			const channel* in_c =r->in((port_id)in_p).link().best_schedule.get(t_in);
			debugs("Channel:\n\tout_c:\t" << *out_c << "\n\tin_c:\t" << *in_c);
			if(out_c == in_c){ // The correct link found
				ret_port = (port_id)in_p;
				break;
			}
		}
	}
	if(ret_port == __NUM_PORTS){
		if(r->local_in_best_schedule.has(t_in)){
			const channel* in_c = r->local_in_best_schedule.get(t_in);
			ensure(out_c == in_c,"Channel rose from nothing like a fenix.\nChannel:\n\tout_c:\t" << *out_c << "\n\tin_c:\t" << *in_c << "\n");
			// The correct link found
			ret_port = L;
		} else {
			ensure(false,"Channel rose from nothing like a fenix.\nChannel:\n\tout_c:\t" << *out_c <<"\n");
		}
	}
	ensure(ret_port != __NUM_PORTS, "Channel rose from nothing like a fenix.\nChannel:\n\tout_c:\t" << *out_c << "\n");
	return ret_port;
}

xmlOutput::xmlOutput(string _output_dir) : output_dir(_output_dir){
}

xmlOutput::~xmlOutput(){

}

}
