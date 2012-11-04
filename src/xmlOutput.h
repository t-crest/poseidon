/* 
 * File:   xmlOutput.h
 * Author: T410s
 *
 * Created on 6. august 2012, 11:13
 */

#ifndef XMLOUTPUT_H
#define	XMLOUTPUT_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <unordered_map>
#include <cstdio>
#include "IOutput.h"
#include "lex_cast.h"
#include "pugixml.hpp"


class xmlOutput: public IOutput {
private:
    string output_dir;

    char p2c(port_id p);
	void print_coord(const pair<int, int> r, char* co, const size_t buffer_size);
   
public:
    xmlOutput(string _output_dir);
    ~xmlOutput();
    
    bool output_schedule(const network_t& n);
    
};

#endif	/* XMLOUTPUT_H */

