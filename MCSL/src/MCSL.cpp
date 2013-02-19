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
#include "pugixml.hpp"

#define debugging

#ifdef debugging
#define debugf(x)	{std::cout << __FILE__ << ":" << __LINE__ << ":\t " #x " = '" << (x) << "'" << std::endl;}
#else
#define debugf(x)	{}
#endif

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
	
	std::ifstream in("../traffic_data/mesh/mesh_3x3/Sparse_mesh_3x3.stp");
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
	cout << "Startlist: " << trash_count << std::endl;
	for(int i = 0; i <= trash_count; i++){
		cin.ignore(20,'\t');
	}
	cin >> trash_count;
	cout << "Endlist: " << trash_count << std::endl;
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
	
	
	vector<pair<pair<int,int>,int> > task_list;
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
		cout << "Coordinate: (" << x << "," << y << ")" << std::endl;
		cin.ignore(20,'\t');
		cin.ignore(20,'\t');
		cin >> avg;
		cout << "AVG: " << avg << std::endl;
		cin.ignore(20,'\n');
		
		task_list.push_back(make_pair(make_pair(x,y),avg));
	}
	
	
	// Channels
	pugi::xml_node channels = doc.append_child("channels");
	channels.append_attribute("type").set_value("arbitrary");
	
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
		debugf(src);
		debugf(dest);
		debugf(mean);
		debugf(devi);
		debugf(rate);
		
		int src_x, src_y, dest_x, dest_y;
		src_x = task_list.at(src).first.first; // x
		src_y = task_list.at(src).first.second; // y
		
		dest_x = task_list.at(dest).first.first; // x
		dest_y = task_list.at(dest).first.second; // y
		
		char* from_node = new char[9];
		char* to_node = new char[9];
		pugi::xml_node channel = channels.append_child("channel");
		sprintf(from_node,"(%i,%i)",src_x,src_y);
		channel.append_attribute("from").set_value(from_node);
		sprintf(to_node,"(%i,%i)",dest_x,dest_y);
		channel.append_attribute("to").set_value(to_node);
		channel.append_attribute("bandwidth").set_value(1);
		channel.append_attribute("repsonse").set_value("false");

		
	}
	char default_file_name [500] = "testout.xml";
	//sprintf(default_file_name,"%s%s%ix%i_cf%.2f_(%i_%i).xml");
	doc.save_file(default_file_name);
		
	return 0;
}



