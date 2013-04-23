package converter;

import java.util.List;

public class JOPDMAPrinter extends Printer {
	
	private static int indent = 0;
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


	private String ind(){
		String str = "\n";
		if (indent < 0) {
			return "";
		}
		for (int i = 0; i < indent; i++) {
			str += "\t";
		}
		return str;
	}

	private String openBrac(){
		indent++;
		return "{";
	}

	private String closeBrac(int b){
		String str = "";
		for (int i = 0 ; i < b; i++) {
			str += closeBrac();
		}
		return str;
	}

	private String closeBrac(){
		if (indent > 0) {
			indent--;
		}
		String str = ind();
		return str + "}";
	}
}