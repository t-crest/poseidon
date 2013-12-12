/*******************************************************************************
 * Copyright 2012 Rasmus Bo Soerensen <rasmus@rbscloud.dk>
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

package converter;

import java.util.List;

/**
 *
 * @author Rasmus Bo Sorensen
 */
class SourcePhaseTextPrinter extends Printer {
	private static final int SLOT_TABLE = 0;
	private static final int ROUTE_TABLE = 1;

	public SourcePhaseTextPrinter() {
	}

	@Override
	protected void printHeader() {
		String str =	"# AUTO-Generated file DO NOT EDIT!!!\n"+
						"# This file contains configuration data for the T-CREST NoC\n"+
						"# with DMA controllers in network interfaces and source routing\n\n";
		try{
			ofile.write(str);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	protected void printFooter() {

	}

	@Override
	public void printData(List<List<List<Integer>>> initArray) {
		String str = "";
		int numNodes = initArray.size();
		int slotTableWidth = (int)Math.ceil(Math.log(numNodes)/Math.log(2));
		int cycles = initArray.get(0).get(SLOT_TABLE).size();
		int timeslots = (int)Math.ceil(cycles/3.0);

		str += "# Number of CPUs is: " + numNodes + "\n"+
			   numNodes + "\n";
		str += "# Number of timeslots is: " + timeslots + "\n"+
			   timeslots + "\n";

		int i = 0;
		for(List<List<Integer> > initCpu: initArray){
			str += "# Configuration of network interface: " + i + "\n"+
				   "# NI" + i + "\n"+
				   "# SLOT_TABLE\n"+
                   timeslots + "\n";
			for(int j = 0; j < timeslots; j++){
				int phase = 0;
				int slot = i;

				int slot0 = initCpu.get(SLOT_TABLE).get(j*3+0);
				int slot1 = (j*3+1 >= cycles) ? i : initCpu.get(SLOT_TABLE).get(j*3+1);
				int slot2 = (j*3+2 >= cycles) ? i : initCpu.get(SLOT_TABLE).get(j*3+2);

				if((slot0 == slot1) && (slot1 == slot2)){
					phase = 0;
					slot = slot0;
				}
				if((slot0 != slot1) && (slot1 == slot2) && (slot1 != i)){
					phase = 1;
					slot = slot1;
				}
				if((slot1 != slot2) && (slot2 != i)){
					phase = 2;
					slot = slot2;
				}
				int valid = (slot != i) ? 1 : 0;
				int out = (valid << (slotTableWidth+2)) | (slot << 2) | phase;
				str += Integer.toBinaryString((1<<(1+slotTableWidth+2)) | out).substring(1) + "\n";
			}
//			for(int slot : initCpu.get(SLOT_TABLE)){
//				str += Integer.toBinaryString(slot | (1 << slotTableWidth)) + "\n";
//			}
			str += "# ROUTE_TABLE\n";
			for(int route: initCpu.get(ROUTE_TABLE)){
				str += Integer.toBinaryString((1 << 16) | route).substring(1) + "\n";
			}
			str += "\n";
			i++;
		}
		try{
			ofile.write(str);
		} catch(Exception e){
			e.printStackTrace();
		}

	}
}
