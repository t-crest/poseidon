/*******************************************************************************
 * Copyright 2012 Mark Ruvald Pedersen <mark@radix63.dk>
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2012 Jaspur Hoejgaard <jaspurh@gmail.com>
 * Copyright 2013 Technical University of Denmark, DTU Compute.
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of the copyright holder.
 ******************************************************************************/
 
#ifndef VHDLOUTPUT_H
#define	VHDLOUTPUT_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <unordered_map>
#include "IOutput.h"
#include "lex_cast.h"

namespace snts{

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

}

#endif	/* VHDLOUTPUT_H */

