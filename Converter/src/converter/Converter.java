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

import org.apache.commons.cli.*;
import java.lang.reflect.*;

/**
 *
 * @author Rasmus Bo Sorensen
 */
public class Converter {
	public static Printer printer;
	public static Parser parser;
	public static String[] inputSchedules;
	/**
	 * @param args the command line arguments
	 */
	public static void main(String[] args) {
		int routerDepth = 1;
		String printerName = "Argo2-c";
		boolean showStats = false;
		boolean showMinStats = false;
		boolean verbose = false;
		String outFile = "";
		String[] inFiles = {""};
		int numModes = 0;

		// create the command line parser
		CommandLineParser cmdParser = new DefaultParser();

		// create the Options
		Options options = new Options();
		
		options.addOption( Option.builder("o").argName("FILE").required(true)
								.longOpt("output-file").desc("the name of the output file.")
								.hasArg().build() );
		options.addOption( Option.builder("r").argName("NUM").required(false)
								.longOpt("router-depth").desc("the number of pipeline stages in a router.")
								.hasArg(true).type(Integer.class).build());
		options.addOption( Option.builder("p").argName("NAME").required(false).longOpt("printer-name")
								.desc("the name of the printer. Choose between the following:\n" +
													"- Source-Phase-text\n" +
													"- Source-Phase-ASIC-text\n" +
													"- Source-text\n" +
													"- Source-java\n" +
													"- Aegean-c\n" +
													"- Aegean-phase-c\n" +
													"- Argo2-c\n" +
													"- Aegean-ASIC-phase-c\n" +
													"- Dist-c\n"+
													"- NetworkCalculus-constraints\n")
								.hasArg().build() );
		options.addOption( Option.builder("s").required(false).longOpt("stats")
								.desc("Show stats of the converted schedule.")
								.hasArg(false).build());
		options.addOption( Option.builder("m").required(false).longOpt("min-stats")
								.desc("Show a minimal number of stats for the converted schedule.")
								.hasArg(false).build());
		options.addOption( Option.builder("v").required(false).longOpt("verbose")
								.desc("Make converter output verbose.")
								.hasArg(false).build());

		try {
			// parse the command line arguments
			CommandLine line = cmdParser.parse( options, args );
			if( line.hasOption( "v" ) ) {
				verbose = true;
			}
			if( line.hasOption( "r" ) ) {
				routerDepth = Integer.parseInt(line.getOptionValue( "r" ));
				if (verbose) {
					System.out.println( "routerDepth: " + routerDepth );
				}
			}
			if( line.hasOption( "p" ) ) {
				printerName = line.getOptionValue( "p" );
				if (verbose) {
					System.out.println( "printerName: " + printerName );
				}
			}
			if( line.hasOption( "s" ) ) {
				showStats = true;
			}
			if( line.hasOption( "m" ) ) {
				showMinStats = true;
			}
			
			outFile = line.getOptionValue( "o" );
			inFiles = line.getArgs();
			if (verbose) {
				System.out.println("inFiles:");
				for(String i : inFiles){
					System.out.println("\t"+i);
				}
				System.out.println( "outFile: " + outFile );
			}
			
			numModes = inFiles.length;

		} catch( ParseException exp ) {
			System.out.println( exp.getMessage() );
			String header = "";
			String footer = "\nPlease report issues to rasmus@rbscloud.dk";

			HelpFormatter formatter = new HelpFormatter();
			formatter.setArgName("schedules");
			formatter.printHelp("poseidon-conv", header, options, footer, true);
			return;
		}
		if (numModes != 1 && !"Argo2-c".equals(printerName)) {
			System.out.println(printerName + " does not support multiple modes.");
			return;
		}

		if ("Source-Phase-text".equals(printerName)){
			printer = new SourcePhaseTextPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Source-Phase-ASIC-text".equals(printerName)){
			printer = new SourcePhaseTextPrinter();
			parser = new ASICSourceParser(routerDepth);
		} else if ("Source-text".equals(printerName)){
			printer = new SourceTextPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Source-java".equals(printerName)){
			printer = new JOPDMAPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-c".equals(printerName)){
			printer = new AegeanPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-phase-c".equals(printerName)){
			printer = new AegeanAsyncPrinter();
			parser = new SourceParser(routerDepth);
		} else if ("Aegean-ASIC-phase-c".equals(printerName)){
			printer = new AegeanAsyncPrinter();
			parser = new ASICSourceParser(routerDepth);
		} else if ("Argo2-c".equals(printerName)){
			if (verbose) {
				System.out.print("Printing configuration files...");
			}
			parser = new Argo2Parser(inFiles,outFile,new Argo2Printer(),showStats,showMinStats);
			if (verbose) {
				System.out.println("Done");
			}
			return;
		} else if ("Dist-c".equals(printerName)) {
			throw new UnsupportedOperationException("Dist-c: Not supported yet.");
		} else if ("NetworkCalculus-constraints".equals(printerName)){
			printer = null;
			parser = new NCParser();
		} else {
			System.out.println("No printer specified...");
			return;
		}

		if (verbose) {
			System.out.print("Printing configuration files...");
		}

		parser.start(inFiles[0],outFile,printer);

		if (verbose) {
			System.out.println("Done");
		}
	}
}
