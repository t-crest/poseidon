/*******************************************************************************
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 * Copyright 2013 Technical University of Denmark, DTU Compute.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with||without
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
 * NO EXPRESS||IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS||IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER||CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS||SERVICES; LOSS OF USE, DATA,||PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,||TORT (INCLUDING NEGLIGENCE
 *||OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed||implied, of the copyright holder.
 ******************************************************************************/

package converter;

import java.util.ArrayList;
import java.util.List;

import org.w3c.dom.*;

import java.lang.System;

/**
*	Swap inputs and outputs for the ASIC implementation
*	@author Christoph Mueller
*
*	The ASIC variant swaps some ports to simplify routing 
*	(minimize interleaving of wires and simplify wrap-arround
*   for the edges/corners with folded bitorus layout)
*   
*   The result is a checkerboard pattern of applied port inversion for 
*   the placed folded torrus with special cases for the corners:
*   
*   checkerboard pattern 
* 
* 	Sample 4x4 placement
* 
* 	[*,*] -> do not swap 
* 	(*,*) -> swap with opposite port 
* 
* 	(2,0) [2,3] (2,1) [2,2]
* 	[1,0] (1,3) [1,1] (1,2)
* 	(3,0) [3,3] (3,1) [3,2]
* 	[0,0] (0,3) [0,1] (0,2)
* 
* 	connections are as follows:
*       _____________________________
*  	   |                             |
*      '-> 0,0 -> 0,1 -> 0,2 -> 0,3 -'
*   
*   Thus, in the layout every second tile is skipped.
*/
public class ASICSourceParser extends SourceParser {
	
	public ASICSourceParser(int routerDepth){
		super(routerDepth);
	}

	/**
	 * Port swapping based on tile and intended direction
	 * @param TileCoord tileCoord the tile for which to swap
	 * @param char requested the intended direction
	 * @return char the actual port to use to route into the 
	 * 		   intended direction
	 */
	private char swap_ports(TileCoord tileCoord, char requested){
		/* This variables are directly ported from tiled_noc.vhd*/
		int N = tileCoord.getHeight();
		int M = tileCoord.getWidth();
		int i = tileCoord.x;
		int j = tileCoord.y;
		
		/* checkerboard implementation */
		boolean do_flip = !(
							  (	/* both in lower half*/
							      ((double)i) < (((double)M)/2.0) 
							   && ((double)j) < (((double)N)/2.0)
							  ) || (  
								/* both in upper half */
								  ((double)i) >= (((double)M)/2.0) 
							   && ((double)j) >= (((double)N)/2.0)
							  )
						   );
		
	    boolean center  = ((i != 0) && (i != M/2) && (j != 0) && (j != N/2));

	    boolean south_edge = (i == 0);
	    boolean north_edge = (i == M/2);
	    boolean west_edge  = (j == 0);
	    boolean east_edge  = (j == N/2);

	    boolean corner_sw = (south_edge && west_edge);
	    boolean corner_se = (south_edge && east_edge);
	    boolean corner_nw = (north_edge && west_edge);
	    boolean corner_ne = (north_edge && east_edge);

	    boolean corner = (corner_sw || corner_se || corner_nw || corner_ne);

	    boolean even_N = ((N%2)==0);
	    boolean even_M = ((M%2)==0);
	    
	    char mapped = requested;
	    
	    if (!corner) {
	    	if (do_flip) {
	    		mapped = oppositPort(requested);
	    	} else {
	    		mapped = requested;
	    	}
	    } else {
	    	if (corner_sw) {
	    		if(requested == 'N'){mapped = 'W';}
	    		else if(requested == 'E'){mapped = 'E';}
	    		else if(requested == 'S'){mapped = 'S';}
	    		else if(requested == 'W'){mapped = 'N';}
	    	}
	    	
	    	if (corner_se) {
	    		if (even_M) {
	    			if(requested == 'N'){mapped = 'E';}
	    			else if(requested == 'E'){mapped = 'W';}
	    			else if(requested == 'S'){mapped = 'N';}
	    			else if(requested == 'W'){mapped = 'S';}
	    		} else {
	    			if(requested == 'N'){mapped = 'E';}
	    			else if(requested == 'E'){mapped = 'N';}
	    			else if(requested == 'S'){mapped = 'S';}
	    			else if(requested == 'W'){mapped = 'W';}
	    		}
	    	}
	    	
	    	if (corner_nw) {
	    		if (even_N) {
	    			if(requested == 'N'){mapped = 'E';}
	    			else if(requested == 'E'){mapped = 'W';}
	    			else if(requested == 'S'){mapped = 'N';}
	    			else if(requested == 'W'){mapped = 'S';}
	    		} else {
	    			mapped = requested;
	    		}
	    	}
	    	
	    	/* current implementation supports only N==M for now - so does the vhdl implementation also...*/
	    	if (corner_ne) {
	    		if (even_N) {
	    			if(requested == 'N'){mapped = 'W';}
	    			else if(requested == 'E'){mapped = 'E';}
	    			else if(requested == 'S'){mapped = 'S';}
	    			else if(requested == 'W'){mapped = 'N';}
	    		} else {
	    			if(requested == 'N'){mapped = 'N';}
	    			else if(requested == 'E'){mapped = 'S';}
	    			else if(requested == 'S'){mapped = 'E';}
	    			else if(requested == 'W'){mapped = 'W';}
	    		}
	    	}
	    	
	    	
	    }
	    
		// If local port do nothing
	    return mapped;
	}
	
	/** 
	 * Port swapping find_route variant 
	 */
	 int find_route(TileCoord destCoord, TileCoord tileCoord, int slotIdx) throws NumberFormatException {
		/* For each transmission slot write an entry in the route table */
		String binRoute = "";
		if (destCoord.getTileId() != tileCoord.getTileId()){
			TileCoord tempTileCoord = new TileCoord(tileCoord.x,tileCoord.y); // Make temporary tile
			char inPort = 'L';
			for(int i = 0; tempTileCoord.getTileId() != destCoord.getTileId(); i++){ // Increment i with the router depth, plus the link depth on the given link
				NodeList ports = getPorts(tempTileCoord,slotIdx+(i*routerDepth));
				char outPort = findOutputPort(ports,inPort);
				binRoute = port2bin(swap_ports(tempTileCoord, outPort)) + binRoute; // Must not be changed to binRoute += port2bin(outport), this is string concatenation
				inPort = oppositPort(outPort);
				nextTile(tempTileCoord,outPort);
				if (i >= destCoord.getNumOfNodes()) {
					System.out.println("Binroute: " +binRoute);
					System.out.print("find_route() missed destination coordinate:\n\ti = " + i + " destCoord.getNumOfNodes() :"+destCoord.getNumOfNodes());
					System.exit(-1);
				}
			}
			// Must not be changed to binRoute += port2bin(outport), this is string concatenation
			binRoute = port2bin(swap_ports(destCoord, inPort)) + binRoute; // Route to local port
		}
		//System.out.println("binRoute:" + binRoute);
		return Integer.parseInt("0" + binRoute, 2);
	}


}





