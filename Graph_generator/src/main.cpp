#include "options.h"
#include "graph_generator.h"
#include <array>
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;


int main(int argc, char* argv[]) 
{
	global::opts = new options(argc, argv);

	if (!global::opts->output_dir.empty())
	{
		int sizes[6] = {3, 4, 5, 8, 10, 15}; // Sizes to create samples
		float channel_factors[5] = {0.1 , 0.3 , 0.5 , 0.7 , 0.9};
		string type = "b";
		
		for(int i = 0; i < 6; i++) {
			for(int j = 0; j < 5; j++) {
				new graph_generator(sizes[i], 
					channel_factors[j], 1, 5, 
					type, global::opts->output_dir);
			}
		}
	}

	delete global::opts;
	return 0;
}
