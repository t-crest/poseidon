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

public class AegeanPrinter extends Printer {

	private static final int SLOT_TABLE = 0;
	private static final int ROUTE_TABLE = 1;

	@Override
	protected void printHeader(){
		String str =  ind() + "/**"
					+ ind() + " * AUTO-Generated file DO NOT EDIT!!!"
					+ ind() + " * Loads the pre calculated schedule into the Slot and Route tables."
					+ ind() + " */"
					+ ind() + "";
		try{
			ofile.write(str);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	protected void printFooter(){
		String str =  closeBrac() + ";";
		try {
			ofile.write(str);
		} catch(Exception e) {
			e.printStackTrace();
		}
	}


	public void printData(List<List<List<Integer> > > initArray){
		int cores = 0;
		int tables = 2;
		int timeslots = 0;
		int numNodes = initArray.size();
		int slotTableWidth = (int)Math.ceil(Math.log(numNodes)/Math.log(2));
		int dmas = 0;
		String str = openBrac();
		String head = "";
		String var = "TIMESLOTS";

		for(List<List<Integer> > initCpu: initArray){
			cores++;
			timeslots = 0;
			dmas = 0;
			str += ind() + openBrac()
					+ ind() + "{";
			for(int slot : initCpu.get(SLOT_TABLE)){
				timeslots++;
				int out = (1 << (slotTableWidth)) | slot;
				str += out + ",";
			}
			str = str.substring(0, str.length()-1);
			str += "}," + ind() + "{";
			for(int route: initCpu.get(ROUTE_TABLE)){
				dmas++;
				str += route + ",";
			}
			str = str.substring(0, str.length()-1);
			str += "}" + closeBrac() + ",";
		}
		str = str.substring(0, str.length()-1);


		if (dmas > timeslots) {
			var = "DMAS";
		}
		head =    "\n#define CORES " + cores
				+ "\n#define TABLES " + tables
				+ "\n#define TIMESLOTS " + timeslots
				+ "\n#define DMAS " + dmas
				+ "\n"
				+ "\nconst int init_array[CORES][TABLES][" + var + "] = ";

		str = head + str;
		try{
			ofile.write(str);
		} catch(Exception e){
			e.printStackTrace();
		}
	}

}
