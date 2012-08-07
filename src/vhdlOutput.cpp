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
			// Write row in Network Adabpter table
			router_id dest_id = (*r)->address;
			router_id src_id = (*r)->address;
			if ((*r)->local_in_best_schedule.has(t)){
				dest_id = (*r)->local_in_best_schedule.get(t)->to;
			}
			if ((*r)->local_out_best_schedule.has(t))
				src_id = (*r)->local_out_best_schedule.get(t)->from;
			
			int dest = dest_id.first + dest_id.second * n.cols();
			int src = src_id.first + src_id.second * n.cols();
			
			this->writeSlotNIDest(t,countWidth,dest);
			this->writeSlotNISrc(src);
			// Write row in Router table 
//			this->writeSlotRouter(t,countWidth,ports);
			port_id ports[5];
			for(int in_p = 0; in_p < __NUM_PORTS; in_p++){
				if(!(*r).in((port_id)in_p).has_link())
					continue; // Route input
				if((*r).in((port_id)in_p).link().best_schedule.has(t)){
					channel* in_c =(*r).in((port_id)in_p).link().best_schedule.get(t);
					for(int out_p = 0; out_p < __NUM_PORTS; out_p++){
						if(!(*r).out((port_id)out_p).has_link())
							continue; // Route input
						if((*r).out((port_id)out_p).link().best_schedule.has(t)){
							channel* out_c =(*r).out((port_id)out_p).link().best_schedule.get(t);
							
						}
					}
				}
				//ports[N] = ; 
				
			}
			
			
		}
		//(*r).
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
	string s = "";
	for (int i = 0; i < bits; ++i) {
		s += (val & (1 << (bits - i - 1))) != 0 ? "1" : "0";
	} // TODO: Correct endianness by dividing  and subtracting
	// bitset STL
	return s;
}

vhdlOutput::vhdlOutput(string output_dir){
	niST.open(output_dir + "ni_ST.vhd", ios::trunc);
	routerST.open(output_dir + "router_ST.vhd", ios::trunc);
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
	routerST << "\t\twhen others => sels <= (others => 0);\n\n";
	routerST << "\tend case;\n";
	routerST << "end process;\n\n";
	routerST << "end data;\n";
}

void vhdlOutput::writeSlotRouter(int slotNum, int countWidth, port* ports){
	routerST << "\t\twhen \"" << bin(slotNum,countWidth) << "\" =>\n";
	if(ports[0] == 5 || ports[0] == 0){
		routerST << "\t\t\tsels(0) <= " << 0 << ";\n";
	} else{
		routerST << "\t\t\tsels(0) <= " << ports[0]-1 << ";\n";
	}
	if(ports[1] == 5 || ports[1] == 1){
		routerST << "\t\t\tsels(1) <= " << 0 << ";\n";
	} else if(ports[1] == 0){
		routerST << "\t\t\tsels(1) <= " << 3 << ";\n";
	} else{
		routerST << "\t\t\tsels(1) <= " << ports[1]-2 << ";\n";
	}
	if(ports[2] == 5 || ports[2] == 2){
		routerST << "\t\t\tsels(2) <= " << 0 << ";\n";
	} else if(ports[2] == 0 || ports[2] == 1){
		routerST << "\t\t\tsels(2) <= " << ports[2]+2 << ";\n";
	} else{
		routerST << "\t\t\tsels(2) <= " << ports[2]-3 << ";\n";
	}
	if(ports[3] == 5 || ports[3] == 3){
		routerST << "\t\t\tsels(3) <= " << 0 << ";\n";
	} else if(ports[3] == 4){
		routerST << "\t\t\tsels(3) <= " << 0 << ";\n";
	} else{
		routerST << "\t\t\tsels(3) <= " << ports[3]+1 << ";\n";
	}
	if(ports[4] == 5 || ports[4] == 4){
		routerST << "\t\t\tsels(4) <= " << 0 << ";\n";
	} else{
		routerST << "\t\t\tsels(4) <= " << ports[4] << ";\n";
	}
}

void vhdlOutput::writeHeaderNI(int countWidth, int numOfNodes){
	niST << "-------------------------------------------------------------\n";
	niST << "-- ni_ST.vhd\n";
	niST << "-- This is an auto generated file, do not edit by hand.\n";
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
	niST << "\t\tdest\t: out integer range 0 to " << numOfNodes << ";\n";
	niST << "\t\tsrc\t: out integer range 0 to " << numOfNodes << "\n";
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

void vhdlOutput::endniST(int num){
	endST(num,&this->niST);
}

void vhdlOutput::endrouterST(int num){
	endST(num,&this->routerST);
}

void vhdlOutput::endST(int num, ofstream* ST){
	*ST << "\t\t\twhen others =>\n";
	*ST << "\t\t\t\tdest <= " << num << ";\n";
	*ST << "\t\t\t\tsrc <= " << num << ";\n\n";
	*ST << "\t\tend case;\n";
	*ST << "\tend process;\n\n";
	*ST << "\tend generate NI_NUM" << num << ";\n\n";
}

void vhdlOutput::endArchNI(){
	niST << "end data;\n";
}


