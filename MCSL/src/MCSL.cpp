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

//#define debugging

#ifdef debugging
#define debugf(x)	{std::cout << __FILE__ << ":" << __LINE__ << ":\t " #x " = '" << (x) << "'" << std::endl;}
#else
#define debugf(x)	{}
#endif

using namespace std;

typedef pair<int,int> coord;

/*
 * 
 */
int main(int argc, char** argv) {
	
	if (argc < 2){
		cout << "To few arguments" << endl;
		return 0;
	}
	
	//std::ifstream in("../traffic_data/mesh/mesh_3x3/Sparse_mesh_3x3.stp");
	std::ifstream in(argv[1]);
	cout << argv[1] << endl;
	if(in.fail()){
		cout << "File not found!" << endl;
		return 0;
	}
	
    std::cin.rdbuf(in.rdbuf()); //redirect std::cin
	
	int trace = 0;
	int topo = 0;
	int pbs = 0;
	int rows = 0;
	int cols = 0;
	int tasks = 0;
	int edges = 0;
	int trash_count = 0;
	
	for(int i = 0; i < 14; i++){
		cin.ignore(200,'\n');
	}
	cin >> trace;
	debugf(trace);
	cin >> topo >> pbs >> rows >> cols;
	debugf(topo);
	debugf(pbs);
	debugf(rows);
	debugf(cols);
	cin >> tasks >> edges;
	debugf(tasks);
	debugf(edges);
	cin >> trash_count;
	for(int i = 0; i <= trash_count; i++){
		cin.ignore(20,'\t');
	}
	cin >> trash_count;
	for(int i = 0; i <= trash_count; i++){
		cin.ignore(20,'\t');
	}
	
	pugi::xml_document doc;
	
	pugi::xml_node topology = doc.append_child("topology");
	topology.append_attribute("width").set_value(rows);
	topology.append_attribute("height").set_value(cols);
	
	pugi::xml_node graph = topology.append_child("graph");
	if (topo == 1) { // b = "bitorus"
		graph.append_attribute("type").set_value("bitorus");
	}
	else if (topo == 0) { // m = "mesh"
		graph.append_attribute("type").set_value("mesh");
	}
	else {
		cout << "Wrong topology found." << std::endl;
		return 0;
	}
	
	
	vector<pair<coord,int> > task_list;
	for(int i = 0; i < tasks; i++){
		// Parse the tasks of the graph
		int id;
		int avg ; 
		int x = 0;
		int y = 0;
		char trash;
		cin >> id;
		cin >> trash >> y >> trash >> x >> trash;
		debugf(id);
		//cout << "Coordinate: (" << x << "," << y << ")" << std::endl;
		cin.ignore(20,'\t');
		cin.ignore(20,'\t');
		cin >> avg;
		debugf(avg);
		cin.ignore(20,'\n');
		
		task_list.push_back(make_pair(make_pair(x,y),avg));
	}
	
	
	// Channels
	pugi::xml_node channels = doc.append_child("channels");
	channels.append_attribute("type").set_value("arbitrary");
	
	
	vector<pair<coord,vector<pair<coord,double> > > > nodes(pbs) ;
	for(size_t i = 0; i < nodes.size(); i++){
		nodes[i].first = make_pair(-1,-1);
		nodes[i].second.resize(pbs,make_pair(make_pair(-1,-1),-1));
	}
	
	
	for(int i = 0; i < edges; i++){
		// Parse the edges of the graph
		int id, src, dest;
		double mean, devi, rate;
		cin >> id >> src >> dest;
		cin.ignore(20,'\t');
		cin.ignore(20,'\t');
		cin.ignore(20,'\t');
		cin >> mean >> devi >> rate;
		debugf(id);
		//debugf(src);
		//debugf(dest);
		//debugf(mean);
		//debugf(devi);
		debugf(rate);
		
		int src_x, src_y, dest_x, dest_y;
		int src_id, dest_id;
		src_x = task_list.at(src).first.first; // x
		src_y = task_list.at(src).first.second; // y
		src_id = src_x + src_y*rows;
		dest_x = task_list.at(dest).first.first; // x
		dest_y = task_list.at(dest).first.second; // y
		dest_id = dest_x + dest_y*rows;
		
		if (src_x == dest_x && src_y == dest_y)
			continue;

		nodes[src_id].first = make_pair(src_x,src_y);
		nodes[src_id].second[dest_id].first = make_pair(dest_x,dest_y);
		if(nodes[src_id].second[dest_id].second == -1){
			nodes[src_id].second[dest_id].second = 0;
		}
		nodes[src_id].second[dest_id].second += rate; 
		
	}
	
	double min_rate = numeric_limits<double>::max();
	double max_rate = 0.0;
	for(int i = 0; i < pbs; i++){
		for(int j = 0; j < pbs; j++){
			if(nodes[i].second[j].second == -1){
				continue;
			}
			if(min_rate > nodes[i].second[j].second){
				min_rate = nodes[i].second[j].second;
			}
			if(max_rate < nodes[i].second[j].second){
				max_rate = nodes[i].second[j].second;
			}
		}
	}
	cout << "Min_rate: " << min_rate << endl;
	cout << "Max_rate: " << max_rate << endl;
	
	for(int i = 0; i < pbs; i++){
		for(int j = 0; j < pbs; j++){
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
				channel.append_attribute("bandwidth").set_value(ceil(nodes[i].second[j].second/min_rate));
				channel.append_attribute("repsonse").set_value("false");
			}
		}
	}
	
	
	//char default_file_name [500] = "testout.xml";
	//sprintf(default_file_name,"%s%s%ix%i_cf%.2f_(%i_%i).xml");
	//doc.save_file(default_file_name);
	doc.save_file(argv[2]);
		
	return 0;
}



