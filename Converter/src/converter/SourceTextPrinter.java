
package converter;

import java.util.List;

/**
 *
 * @author Rasmus Bo Sorensen
 */
class SourceTextPrinter extends Printer {
	private static final int SLOT_TABLE = 0;
	private static final int ROUTE_TABLE = 1;
	
	public SourceTextPrinter() {
	}

	@Override
	protected void printHeader() {
		String str =	"# AUTO-Generated file DO NOT EDIT!!!\n"+
						"# This file contains configureation data for T-CREST NoC\n"+
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
				   "# SLOT_TABLE\n";
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
				str += Integer.toBinaryString(route) + "\n";
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
