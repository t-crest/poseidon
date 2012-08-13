/* 
 * File:   xmlOutput.cpp
 * Author: T410s
 * 
 * Created on 6. august 2012, 11:13
 */

#include <string.h>

#include "xmlOutput.h"


bool xmlOutput::output_schedule(const network_t& n)
{	
	int numOfNodes = n.routers().size();
	int countWidth = ceil(log2(n.best));
	
	xml_document doc;
	xml_node schedule = doc.append_child("schedule");
	schedule.append_attribute("length").set_value(n.best);
	
	
	
	
	for(vector<router_t*>::const_iterator r = n.routers().begin(); r != n.routers().end(); r++){ // For each router, write Network Adapter Table and Router Table
		int r_id = (*r)->address.first + (*r)->address.second * n.cols();
		// New tile
		xml_node tile = schedule.append_child("tile");
		char co [10];
		sprintf(co,"(%i,%i)",(*r)->address.first,(*r)->address.second);
		tile.append_attribute("id") = co;
		//this->startniST(r_id);
		//this->startrouterST(r_id);
		for(timeslot t = 0; t < n.best; t++){ // Write table row for each timeslot
			// New timeslot
			xml_node ts = tile.append_child("timeslot");
			ts.append_attribute("value") = t;
			int t0 = t-1;
			int t1 = t;
			if(t == 0){
				t0 = n.best-1;
				t1 = n.best;
			}
			// Write row in Network Adapter table
			router_id dest_id = (*r)->address;
			router_id src_id = (*r)->address;
			if ((*r)->local_in_best_schedule.has((t+2)%n.best)){
				dest_id = (*r)->local_in_best_schedule.get((t+2)%n.best)->to;
			}
			if ((*r)->local_out_best_schedule.has(t1))
				src_id = (*r)->local_out_best_schedule.get(t1)->from;
			
			int dest = dest_id.first + dest_id.second * n.cols();
			int src = src_id.first + src_id.second * n.cols();
			
			// New na
			xml_node na = ts.append_child("na");
			sprintf(co,"(%i,%i)",src_id.first,src_id.second);
			na.append_attribute("rx") = co;
			sprintf(co,"(%i,%i)",dest_id.first,dest_id.second);
			na.append_attribute("tx") = co;
			
//			this->writeSlotNIDest(t,countWidth,dest);
//			this->writeSlotNISrc(src);
			// Write row in Router table 
			//this->writeSlotRouter(t,countWidth,ports);
			port_id ports[5] = {__NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS};
			// New router
			xml_node router = ts.append_child("router");
				
			
			for(int out_p = 0; out_p < __NUM_PORTS-1; out_p++){
				// For all 4 output ports not being the local port.
				if(!(*r)->out((port_id)out_p).has_link()){
					continue; // No outgoing channel from the port.
				}
				if((*r)->out((port_id)out_p).link().best_schedule.has(t)){
					// If there is a channel comming out of the port, find the
					// input port from which the channel is comming from.
					const channel* out_c =(*r)->out((port_id)out_p).link().best_schedule.get(t);
					for(int in_p = 0; in_p < __NUM_PORTS-1; in_p++){
						// For all 4 input ports not being the local port.
						if(!(*r)->in((port_id)in_p).has_link())
							continue; // No link into this port
						if((*r)->in((port_id)in_p).link().best_schedule.has(t0)){ // REMEMBER: Change back t-1 -> t 
							const channel* in_c =(*r)->in((port_id)in_p).link().best_schedule.get(t0);
							if(out_c == in_c){
								// The correct link found
								ports[(port_id)out_p] = (port_id)in_p;
								break;
							}
						}
					}
					if(ports[(port_id)out_p] == __NUM_PORTS){
						// If channel was not found on any of the 4 input ports.
						// It should be on the local in port, but we test it anyway.
						if((*r)->local_in_best_schedule.has(t)){
							const channel* in_c = (*r)->local_in_best_schedule.get(t);
							if(out_c == in_c){
								// The correct link found
								ports[(port_id)out_p] = L;
							} else {
								cout << "Failure: Channel rose from nothing like a fenix." << endl;
							}
						}
					}
				} else {
					ports[(port_id)out_p] = __NUM_PORTS;
				}
				
			}
			if((*r)->local_out_best_schedule.has(t1)){ // For the local out port.
				const channel* out_c = (*r)->local_out_best_schedule.get(t1);
				for(int in_p = 0; in_p < __NUM_PORTS-1; in_p++){
					// For all 4 input ports not being the local port.
					if(!(*r)->in((port_id)in_p).has_link())
						continue; // No link into this port
					if((*r)->in((port_id)in_p).link().best_schedule.has(t0)){
						const channel* in_c =(*r)->in((port_id)in_p).link().best_schedule.get(t0);
						if(out_c == in_c){
							// The correct link found
							ports[L] = (port_id)in_p;
							break;
						}
					}
				}
				if(ports[L] == __NUM_PORTS){
					// If channel was not found on any of the 4 input ports.
					// It should be on the local in port, but we test it anyway.
					cout << "Failure: Not allowed to route back in to local." << endl;
				}
				// and so on...
			} 
			for(int p = 0; p < __NUM_PORTS; p++){
				// New output
				xml_node output = router.append_child("output");
				sprintf(co,"%c",p2c((port_id)p));
				output.append_attribute("id") = co;
				sprintf(co,"%c",p2c(ports[(port_id)p]));
				output.append_attribute("input") = co;
			}
//			this->writeSlotRouter(t,countWidth,ports);
			
		}
//		this->endniST(r_id);
//		this->endrouterST(r_id);
	}
	char co [500];
	sprintf(co,"%soutput.xml",output_dir.c_str());
	doc.save_file(co);	

	delete this;
	return true;
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
