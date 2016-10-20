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

public class JOPDMAPrinter extends Printer {
	
	private static final int SLOT_TABLE = 0;
	private static final int ROUTE_TABLE = 1;

	@Override
	protected void printHeader(){
		String str =  ind() + "/**"
					+ ind() + " * AUTO-Generated file DO NOT EDIT!!!"
					+ ind() + " * Loads the pre calculated schedule into the Slot and Route tables."
					+ ind() + " * @author package dk.rbscloud.tcrest.SNTs"
					+ ind() + " */"
					+ ind() + "package dk.rbscloud.tcrest.API;"
					+ ind() + "import com.jopdesign.sys.Native;"
					+ ind() + ""
					+ ind() + "public class Tables" + openBrac()
					+ ind() + "public static final int[][][] initArray = " + openBrac()
					+ ind() + ""; 
		try{
			ofile.write(str);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	protected void printFooter(){
		String str =  closeBrac() + ";"
					+ ind() + "private static int[] getSlotTable(int cpuId)" + openBrac()
					+ ind() + "return initArray[cpuId][0];"
					+ closeBrac()
					+ ind() + ""
					+ ind() + "private static int[] getDmaTable(int cpuId)" + openBrac()
					+ ind() + "return initArray[cpuId][1];"
					+ closeBrac()
					+ ind() + ""
					+ ind() + "public static void load(int cpuId)" + openBrac()
					+ ind() + "// Loading the slot table"
					+ ind() + "int[] slotTable = Tables.getSlotTable(cpuId);"
					+ ind() + "for(int i = 0; i < slotTable.length; i++)" + openBrac()
					+ ind() + "Native.wrMem(slotTable[i], Const.SLOT_TBL_BASE+i);"
					+ closeBrac()
					+ ind() + "// Loading the dma table"
					+ ind() + "int[] dmaTable = Tables.getDmaTable(cpuId);"
					+ ind() + "for(int i = 0; i < dmaTable.length; i++)" + openBrac()
					+ ind() + "Native.wrMem(dmaTable[i], Const.DMA_P_BASE+i);"
					+ closeBrac()
					+ closeBrac()
					+ ind() + ""
					+ ind() + "public static boolean verify(int cpuId)" + openBrac()
					+ ind() + "// Reading and verifying the dma table"
					+ ind() + "int[] dmaTable = getDmaTable(cpuId);"
					+ ind() + "for(int i = 0; i < dmaTable.length; i++)" + openBrac()
					+ ind() + "int dmaData = Native.rdMem(Const.DMA_P_BASE+i);"
					+ ind() + "if (dmaData != dmaTable[i])" + openBrac()
					+ ind() + "System.out.println(\"DMA_P_BASE faliure\");"
					+ ind() + "return false;"
					+ closeBrac()
					+ closeBrac()
					+ ind() + "return true;"
					+ closeBrac()
					+ closeBrac()
					+ ind();
		try {
			ofile.write(str);
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	

	public void printData(List<List<List<Integer> > > initArray){
		String str = "";
		for(List<List<Integer> > initCpu: initArray){
			str += ind() + openBrac()
					+ ind() + "{";
			for(int slot : initCpu.get(SLOT_TABLE)){
				str += slot + ",";
			}
			str = str.substring(0, str.length()-1);
			str += "}," + ind() + "{";
			for(int route: initCpu.get(ROUTE_TABLE)){
				str += route + ",";
			}
			str = str.substring(0, str.length()-1);
			str += "}" + closeBrac() + ",";
		}
		str = str.substring(0, str.length()-1);
		try{
			ofile.write(str);
		} catch(Exception e){
			e.printStackTrace();
		}
	}
}