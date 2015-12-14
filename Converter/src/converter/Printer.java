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
	protected static int indent = 0;
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


	protected String ind(){
		String str = "\n";
		if (indent < 0) {
			return "";
		}
		for (int i = 0; i < indent; i++) {
			str += "\t";
		}
		return str;
	}

	protected String openBrac(){
		indent++;
		return "{";
	}

	protected String closeBrac(int b){
		String str = "";
		for (int i = 0 ; i < b; i++) {
			str += closeBrac();
		}
		return str;
	}

	protected String closeBrac(){
		if (indent > 0) {
			indent--;
		}
		String str = ind();
		return str + "}";
	}
	
}
