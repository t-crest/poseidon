/* 
 * File:   main.cpp
 * Author: Rasmus Bo Sorensen
 *
 * Created on 18. februar 2013, 10:31
 */

#include <string>
#include <streambuf>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <cmath>
#include "pugixml.hpp"
#include "options.h"

//#define debugging

#ifdef debugging
#define debugf(x)	{std::cout << __FILE__ << ":" << __LINE__ << ":\t " #x " = '" << (x) << "'" << std::endl;}
#else
#define debugf(x)	{}
#endif

using namespace std;

typedef pair<int,int> coord;


bool print_xml(int degen_deg){
	int num_nodes = global::opts->network_size * global::opts->network_size;
	pugi::xml_document doc;
	
	pugi::xml_node topology = doc.append_child("topology");
	topology.append_attribute("width").set_value(global::opts->network_size);
	topology.append_attribute("height").set_value(global::opts->network_size);
	
	pugi::xml_node graph = topology.append_child("graph");
	graph.append_attribute("type").set_value("bitorus");

	// Channels
	pugi::xml_node channels = doc.append_child("channels");
	channels.append_attribute("type").set_value("arbitrary");
	
	for(int i = 0; i < num_nodes; i++){
		for(int j = 0; j < num_nodes; j++){
			if(nodes[i].second[j].second == -1){
				//cout << "(" << i << "," << j << ") rate: " << nodes[i].second[j].second << endl;
			}
			else{
				//cout << "(" << i << "," << j << ") rate: " << ceil(nodes[i].second[j].second/min_rate) << endl;
				char* from_node = new char[30];
				char* to_node = new char[30];
				pugi::xml_node channel = channels.append_child("channel");
				//sprintf(from_node,"(%i,%i)",src_x,src_y);
				sprintf(from_node,"(%i,%i)",nodes[i].first.first,nodes[i].first.second);
				channel.append_attribute("from").set_value(from_node);
				//sprintf(to_node,"(%i,%i)",dest_x,dest_y);
				sprintf(to_node,"(%i,%i)",nodes[i].second[j].first.first,nodes[i].second[j].first.second);
				channel.append_attribute("to").set_value(to_node);
				channel.append_attribute("bandwidth").set_value(1);
				channel.append_attribute("repsonse").set_value("false");
			}
		}
	}
	
	
	char default_file_name [500];
	sprintf(default_file_name,"%sDegenerated_%ix%i_%i.xml",global::opts->output_dir.c_str(),global::opts->network_size,global::opts->network_size,degen_deg);
	doc.save_file(default_file_name);
	cout << default_file_name << endl;
	
	return true;
	
}

/*
 * 
 */
int main(int argc, char** argv) {
	
	global::opts = new options(argc, argv);
	
	// TODO: Generate complete graph
	// TODO: Calculate the edge weights
	for(int i = 0; i < global::opts->network_size-1; i++){
		// TODO: Find Maximum hamiltonian path
		// TODO: Remove Hamiltonian path
		// TODO: Print file containing the degenerated graph.
	}
	

	
	vector<pair<coord,vector<pair<coord,double> > > > nodes(pbs) ;
	for(size_t i = 0; i < nodes.size(); i++){
		nodes[i].first = make_pair(-1,-1);
		nodes[i].second.resize(pbs,make_pair(make_pair(-1,-1),-1));
	}
	
	
	
	
	
	return 0;
}



