/**
 * Generates a random graph to schedule onto the NoC
 */

#include "graph_generator.h"


graph_generator::graph_generator(int _n, float channel_factor, int bw_min, int bw_max, string type, string _output_dir)
:	n(_n), max_channels((_n*(_n-1))), output_dir(_output_dir)
{
	
	assert(n > 0);
	assert(channel_factor >= 0.0 && channel_factor <= 1.0);
	assert(bw_min > 0);
	assert(bw_max >= bw_min);
	assert((type.compare("b") != 0) || (type.compare("m") != 0));

//	cout << "assertions done" << endl;
	// Initialize
	pugi::xml_document doc;
	
	pugi::xml_node topology = doc.append_child("topology");
	topology.append_attribute("width").set_value(n);
	topology.append_attribute("height").set_value(n);
	
	pugi::xml_node graph = topology.append_child("graph");
	if (type == "b") { // b = "bitorus"
		graph.append_attribute("type").set_value("bitorus");
	}
	else if (type == "m") { // m = "mesh"
		graph.append_attribute("type").set_value("mesh");
	}

	srand(time(NULL));

//	cout << "init done" << endl;
	// Channels
	pugi::xml_node channels = doc.append_child("channels");
	channels.append_attribute("type").set_value("arbitrary");
	
	for (int i = 0; i < n; i++) { // first coordinate from
		for (int j = 0; j < n; j++) { // second coordinate from
			for (int k = 0; k < n; k++) { // first coordinate to
				for (int l = 0; l < n; l++) { // second coordinate to
					if(i == k && j == l) // Don't schedule channels to same node
						continue;
					
					if ((rand() % 100) <= channel_factor*100) {
						char* from_node = new char[9];
						char* to_node = new char[9];
						pugi::xml_node channel = channels.append_child("channel");
						sprintf(from_node,"(%i,%i)",i,j);
						channel.append_attribute("from").set_value(from_node);
						sprintf(to_node,"(%i,%i)",k,l);
						channel.append_attribute("to").set_value(to_node);
						channel.append_attribute("bandwidth").set_value((rand() % bw_max) + (bw_min + 1));
						channel.append_attribute("repsonse").set_value("false");
					}
				}
			}
			
		}
	}
//	cout << "loops done" << endl;
	
	sprintf(default_file_name,"%s%s%ix%i_cf%.2f_(%i_%i).xml",output_dir.c_str(),type.c_str(),n,n,channel_factor,bw_min,bw_max);
	doc.save_file(default_file_name);
	
//	cout << "file done" << endl;
}

graph_generator::~graph_generator() {
	
}