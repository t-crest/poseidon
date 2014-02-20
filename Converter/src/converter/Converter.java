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
				"\t2: Printer name\n" +
				"\t\tSource-Phase-text\n" +
				"\t\tSource-Phase-ASIC-text\n" +
				"\t\tSource-text\n" +
				"\t\tSource-java\n" +
				"\t\tAegean-c\n" +
				"\t\tAegean-phase-c\n" +
				"\t\tAegean-ASIC-phase-c\n" +
				"\t\tDist-c\n"+
				"\t3: Router depth\n");
			return;
		}
		int routerDepth;
		if (args.length > 3) {
			routerDepth = Integer.parseInt(args[3]);
		} else {
			routerDepth = 1;
		}
		if ("Source-Phase-text".equals(args[2])){
			printer = new SourcePhaseTextPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Source-Phase-ASIC-text".equals(args[2])){
			printer = new SourcePhaseTextPrinter();
			parser = new ASICSourceParser(routerDepth);
		} else if ("Source-text".equals(args[2])){
			printer = new SourceTextPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Source-java".equals(args[2])){
			printer = new JOPDMAPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-c".equals(args[2])){
			printer = new AegeanPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-phase-c".equals(args[2])){
			printer = new AegeanAsyncPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-ASIC-phase-c".equals(args[2])){
			printer = new AegeanAsyncPrinter();
			parser = new ASICSourceParser(routerDepth);
		} else if ("Dist-c".equals(args[2])) {
			throw new UnsupportedOperationException("Dist-c: Not supported yet.");
		} else {
			System.out.println("No printer specified...");
			return;
		}
		System.out.print("Printing configuration file...");
		parser.start(args[0],args[1],printer);
		System.out.println("Done");
	}
}
