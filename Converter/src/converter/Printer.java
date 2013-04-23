
package converter;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Rasmus Bo Sorensen
 */
public abstract class Printer {
	protected static FileWriter ofile;
	/**
	 *
	 * @param outFile The name of the output file for the converter
	 */
	public void open(String outFile){
		try {
			ofile = new FileWriter(new File(outFile));
		} catch (IOException ex) {
			Logger.getLogger(JOPDMAPrinter.class.getName()).log(Level.SEVERE, null, ex);
		}
		printHeader();
	}
	
	protected abstract void printHeader();
	
	public abstract void printData(List<List<List<Integer> > > initArray);
	
	protected abstract void printFooter();
	
	public void close() {
		printFooter();
		try {
			ofile.close();
		} catch (IOException ex) {
			Logger.getLogger(Printer.class.getName()).log(Level.SEVERE, null, ex);
		}
	}
	
}
