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

public class Argo2Printer extends Printer {

	private static final int SCHED_TBL = 0;
	private static final int CH_ID_TBL = 1;

	public Argo2Printer() {
		
	}

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

	@Override
	public void printData(List<List<List<Integer> > > initArray){
	}

	public void printMCData(List<List<List<List<Integer> > > > initArray) {
		String str = openBrac();
		String head = "";
		int tables = 2;
		int numModes = initArray.size();
		int numNodes = initArray.get(0).size();

		int maxSchedEnts = 0;
//		for (List<List<List<Integer> > > initMode : initArray) {
//			for(List<List<Integer> > initCpu: initMode){
//				int schedEnts = initCpu.get(SCHED_TBL).size();
//				if (schedEnts > maxSchedEnts) {
//					maxSchedEnts = schedEnts;
//				}
//			}			
//		}

		for (int i = 0; i < initArray.size(); i++) { // For each mode
			List<List<List<Integer> > > initMode = initArray.get(i);
			str += ind() + openBrac();
			for (int j = 0; j < initMode.size(); j++) { // For each core
				List<List<Integer> > initCpu = initMode.get(j);
				List<Integer> schedTbl = initCpu.get(SCHED_TBL);
				int schedEnts = schedTbl.size();
				if (schedEnts > maxSchedEnts) {
					maxSchedEnts = schedEnts;
				}
				str += ind() + openBrac()
					 + ind() + "{";
				str += schedEnts + ",";
				for(int k = 0; k < schedEnts; k++){
					str += schedTbl.get(k) + ",";
				}
				str = str.substring(0, str.length()-1);
				str += "}," + ind() + "{";
				str += schedEnts + ",";
				for(int k = 0; k < schedEnts; k++) {
					//int route: initCpu.get(CH_ID_TBL)){
					//str += route + ",";
					str += 0 + ",";
				}
				str = str.substring(0, str.length()-1);
				str += "}" + closeBrac() + ",";

			//	str += "\n";
			}
			str = str.substring(0, str.length()-1);
			str += closeBrac() + ",";
		}
		str = str.substring(0, str.length()-1);


		head =    "\nconst int NOC_MODES = " + numModes + ";"
				+ "\nconst int NOC_CORES = " + numNodes + ";"
				+ "\nconst int NOC_TABLES = " + tables + ";"
				+ "\nconst int NOC_SCHEDULE_ENTRIES = " + (maxSchedEnts+1) + ";"
				+ "\n"
				+ "\nconst int noc_init_array[NOC_MODES][NOC_CORES][NOC_TABLES][NOC_SCHEDULE_ENTRIES] = ";

		str = head + str;
		try{
			ofile.write(str);
		} catch(Exception e){
			e.printStackTrace();
		}
	}
}
