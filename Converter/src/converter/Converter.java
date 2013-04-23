
package converter;

/**
 *
 * @author Rasmus Bo Sorensen
 */
public class Converter {
	public static Printer printer;
	public static Parser parser;
	/**
	 * @param args the command line arguments
	 */
	public static void main(String[] args) {
		/**
		 * Commandline options:
		 * 0: String inputFile
		 * 1: String outputFile
		 * 2: String printerName
		 */
		// TODO code application logic here
		if(args.length <= 2){
			System.out.println(
			"Input arguments missing\n" + 
			"Required:\n" + 
				"\t0: Inputfile\n" + 
				"\t1: Outputfile\n" + 
				"\t2: Printer name\n");
			return;
		}
		if ("Source-text".equals(args[2])){
			printer = new SourceTextPrinter();
			parser = new SourceParser();
			
		} else if ("Source-java".equals(args[2])){
			printer = new JOPDMAPrinter();
			parser = new SourceParser();
		} else if ("Dist-c".equals(args[2])) {
			throw new UnsupportedOperationException("Not supported yet."); 
		} else {
			System.out.println("No printer specified...");
			return;
		}
		
		parser.start(args[0],args[1],printer);
		
	}
}
