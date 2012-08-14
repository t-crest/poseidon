/* 
 * File:   vhdlOutput.cpp
 * Author: T410s
 * 
 * Created on 6. august 2012, 11:13
 */

#include "vhdlOutput.h"


bool vhdlOutput::output_schedule(const network_t& n)
{
	int numOfNodes = n.routers().size();
	int countWidth = ceil(log2(n.best));
	
	this->writeHeaderRouter(countWidth);
	this->writeHeaderNI(countWidth,numOfNodes);
	
	for(vector<router_t*>::const_iterator r = n.routers().begin(); r != n.routers().end(); r++){ // For each router, write Network Adapter Table and Router Table
		int r_id = (*r)->address.first + (*r)->address.second * n.cols();
		this->startniST(r_id);
		this->startrouterST(r_id);
		for(timeslot t = 0; t < n.best; t++){ // Write table row for each timeslot
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
			
			this->writeSlotNIDest(t,countWidth,dest);
			this->writeSlotNISrc(src);
			// Write row in Router table 
			//this->writeSlotRouter(t,countWidth,ports);
			port_id ports[5] = {__NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS, __NUM_PORTS};
			
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
				//ports[N] = ; 
				
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
			this->writeSlotRouter(t,countWidth,ports);
			
		}
		this->endniST(r_id);
		this->endrouterST(r_id);
	}
//	n.router(e)->next;
	//n.routers().at(n).local_out_best_schedule.get(t).from
	
	
	this->endArchRouter();
	this->endArchNI();
	delete this;
	return true;
}

string vhdlOutput::bin(int val, int bits) {
	int max = (int)pow(2.0,bits-1);
	string s = "";
	for(int i = 0; i < bits; i++){
		if(val/max >= 1){
			val -= max;
			s += "1";
		} else {
			s += "0";
		}
		max = max / 2;
	}
	return s;
}

char vhdlOutput::p2c(port_id p){
		char c;
		if (p == N) c = 'N';
		if (p == E)	c = 'E';
		if (p == S)	c = 'S';
		if (p == W)	c = 'W';
		if (p == L)	c = 'L';
		if (p == __NUM_PORTS)	c = 'D';

		return c;
	}

vhdlOutput::vhdlOutput(string output_dir){
	niST.open(output_dir + "ni_ST.vhd", ios::trunc);
	routerST.open(output_dir + "router_ST.vhd", ios::trunc);
	if(!niST.good()){
		niST.close();
		string new_file = output_dir + ::lex_cast<string>((int)time(NULL)) + "ni_ST.vhd";
		cout << "Warning: Output failure, new output name: " + new_file << endl;
		niST.open(new_file, ios::trunc);
	}
	if(!routerST.good()){
		routerST.close();
		string new_file = output_dir + ::lex_cast<string>((int)time(NULL)) + "router_ST.vhd";
		cout << "Warning: Output failure, new output name: " + new_file << endl;
		routerST.open(new_file, ios::trunc);
	}
	// TODO: Error handling + Specify output file name
}

vhdlOutput::~vhdlOutput(){
	niST.close();
	routerST.close();
}

void vhdlOutput::writeHeaderRouter(int countWidth){
	routerST << "-------------------------------------------------------------\n";
	routerST << "-- router_ST.vhd\n";
	routerST << "-- This is an auto generated file, do not edit by hand.\n";
	routerST << "-- These tables were generated from an application specific\n";
	routerST << "-- schedule by the SNTs project.\n";
	routerST << "-- https://github.com/rbscloud/SNTs\n";
	routerST << "-------------------------------------------------------------\n";
	routerST << "library ieee;\n";
	routerST << "use ieee.std_logic_1164.all;\n";
	routerST << "use ieee.numeric_std.all;\n\n";

	routerST << "use work.leros_types.all;\n";
	routerST << "use work.noc_types.all;\n\n";

	routerST << "entity router_ST is\n";
	routerST << "\tgeneric (\n";
	routerST << "\t\tNI_NUM\t: natural);\n";
	routerST << "\tport (\n";
	routerST << "\t\tcount\t: in unsigned(" << countWidth-1 << " downto 0);\n";
	routerST << "\t\tsels\t: out select_signals\n";
	routerST << "\t\t);\n";
	routerST << "end router_ST;\n\n";
	
	routerST << "architecture data of router_ST is\n";
	routerST << "begin -- data\n\n";

}

void vhdlOutput::endArchRouter(){
	routerST << "end data;\n";
}

void vhdlOutput::writeSlotRouter(int slotNum, int countWidth, port_id* ports){
	routerST << "\t\twhen \"" << bin(slotNum,countWidth) << "\" =>\n";
	routerST << "\t\t\tsels(N) <= " << p2c(ports[N]) << ";\n";
	routerST << "\t\t\tsels(E) <= " << p2c(ports[E]) << ";\n";
	routerST << "\t\t\tsels(S) <= " << p2c(ports[S]) << ";\n";
	routerST << "\t\t\tsels(W) <= " << p2c(ports[W]) << ";\n";
	routerST << "\t\t\tsels(L) <= " << p2c(ports[L]) << ";\n";	
}

void vhdlOutput::writeHeaderNI(int countWidth, int numOfNodes){
	niST << "-------------------------------------------------------------\n";
	niST << "-- ni_ST.vhd\n";
	niST << "-- This is an auto generated file, do not edit by hand.\n";
	niST << "-- These tables were generated from an application specific\n";
	niST << "-- schedule by the SNTs project.\n";
	niST << "-- https://github.com/rbscloud/SNTs\n";
	niST << "-------------------------------------------------------------\n";
	niST << "library ieee;\n";
	niST << "use ieee.std_logic_1164.all;\n";
	niST << "use ieee.numeric_std.all;\n\n";

	niST << "use work.leros_types.all;\n";
	niST << "use work.noc_types.all;\n\n";

	niST << "entity ni_ST is\n";
	niST << "\tgeneric (\n";
	niST << "\t\tNI_NUM\t: natural);\n";
	niST << "\tport (\n";
	niST << "\t\tcount\t: in unsigned(" << countWidth-1 << " downto 0);\n";
	niST << "\t\tdest\t: out integer range 0 to " << numOfNodes-1 << ";\n";
	niST << "\t\tsrc\t: out integer range 0 to " << numOfNodes-1 << "\n";
	niST << "\t\t);\n";

	niST << "end ni_ST;\n\n";
	niST << "architecture data of ni_ST is\n";
	niST << "begin -- data\n\n";

}

void vhdlOutput::startniST(int num){
	startST(num,&this->niST);
}

void vhdlOutput::startrouterST(int num){
	startST(num,&this->routerST);
}

void vhdlOutput::startST(int num, ofstream* ST){
	*ST << "\tNI_NUM" << num << " : if NI_NUM = " << num << " generate\n";
	*ST << "\tprocess(count) begin\n\n";
	*ST << "\t\tcase count is\n\n";
}

void vhdlOutput::writeSlotNIDest(int slotNum, int countWidth, int dest){
	niST << "\t\t\twhen \"" << bin(slotNum,countWidth) << "\" =>\n";
	niST << "\t\t\t\tdest <= " << dest << ";\n";
}

void vhdlOutput::writeSlotNISrc(int src){
	niST << "\t\t\t\tsrc <= " << src << ";\n";
}

void vhdlOutput::endrouterST(int num){
	routerST << "\t\twhen others =>\n";
	routerST << "\t\t\tsels(N) <= D;\n";
	routerST << "\t\t\tsels(E) <= D;\n";
	routerST << "\t\t\tsels(S) <= D;\n";
	routerST << "\t\t\tsels(W) <= D;\n";
	routerST << "\t\t\tsels(L) <= D;\n";
	routerST << "\t\tend case;\n";
	routerST << "\tend process;\n\n";
	routerST << "\tend generate NI_NUM" << num << ";\n\n";
}

void vhdlOutput::endniST(int num){
	niST << "\t\t\twhen others =>\n";
	niST << "\t\t\t\tdest <= " << num << ";\n";
	niST << "\t\t\t\tsrc <= " << num << ";\n\n";
	niST << "\t\tend case;\n";
	niST << "\tend process;\n\n";
	niST << "\tend generate NI_NUM" << num << ";\n\n";
}

void vhdlOutput::endArchNI(){
	niST << "end data;\n";
}


