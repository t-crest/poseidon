/* 
 * File:   vhdlOutput.h
 * Author: Rasmus Bo Sorensen
 *
 * Created on 6. august 2012, 11:13
 */

#ifndef VHDLOUTPUT_H
#define	VHDLOUTPUT_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <unordered_map>
#include "IOutput.h"
#include "lex_cast.h"


class vhdlOutput: public IOutput {
private:
    enum port {North,East,South,West,Local,DC};

//    class STslot{
//    public:
//            port ports[5];
//            int x_dest;
//            int y_dest;
//            int x_src;
//            int y_src;
//            STslot(){
//                    for(int i = 0; i < 5; i++){
//                            ports[i] = DC;
//                    }
//                    x_dest = 0;
//                    y_dest = 0;
//                    x_src = 0;
//                    y_src = 0;
//            }
//
//    };
    
    std::ofstream niST;
    std::ofstream routerST;
    std::string numOfNodesStr;

    std::string bin(int val, int bits);
    char p2c(port_id p);
    void startST(int num, std::ofstream* ST);
    void writeHeaderRouter(int countWidth);
    void endArchRouter();
    void writeSlotRouter(int slotNum, int countWidth, port_id* ports);
    void writeHeaderNI(int countWidth, int numOfNodes);

    void writeSlotNIDest(int slotNum, int countWidth, int dest);
    void writeSlotNISrc(int src);

    void startniST(int num);
    void startrouterST(int num);
    void endniST(int num);
    void endrouterST(int num);

    void endArchNI();
public:
    vhdlOutput(std::string output_dir);
    ~vhdlOutput();
    
//    bool output_schedule(network_t& n);
    bool output_schedule(const network_t& n);
    
};

#endif	/* VHDLOUTPUT_H */

