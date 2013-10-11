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
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 *
 * @author Rasmus Bo Sorensen
 */
public abstract class Parser {
	protected enum Port {N, E, S, W, L, D}
	protected static Document doc;
	protected static NodeList tList;
	protected Printer printer;

	public void start(String inFile, String outFile, Printer __printer){
		printer = __printer;
		printer.open(outFile);
		parseXml(inFile);
		new TileCoord(0,0,(int)Math.sqrt(getNumOfNodes())); // Initializing the static sideLength variable in TileCoord
		parse();

		printer.close();
	}

	protected abstract void parse();

	private static void parseXml(String inputFile){
		try{
			File fXmlFile = new File(inputFile);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			doc = dBuilder.parse(fXmlFile);
			doc.getDocumentElement().normalize();
			tList = doc.getElementsByTagName("tile");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	protected static NodeList getPorts(TileCoord tileCoord, int slotIdx){
		Element tileE = (Element) getTile(tileCoord);
		NodeList sList = tileE.getElementsByTagName("timeslot");
		slotIdx = slotIdx % sList.getLength();
		//slotIdx = (slotIdx + 2) % sList.getLength(); // The schedule takes pipelining into acount.
													 // Shifting by 2 gets rid of the pipelining.
		//System.out.println("slotIdx:" + slotIdx);
		Element slotE = (Element) sList.item(slotIdx);
		NodeList rList = slotE.getElementsByTagName("router");
		Node router = rList.item(0);
		Element routerE = (Element) router;
		return routerE.getElementsByTagName("output");
	}

	protected static Node getTile(TileCoord tileCoord){
		for (int tileIdx = 0; tileIdx < tList.getLength(); tileIdx++) { // For each tile
			Node tile = tList.item(tileIdx);
			if (tile.getNodeType() == Node.ELEMENT_NODE) {
				if(tileCoord.getTileId() == getTileCoord(tile).getTileId()){
					return tile;
				}
			}
		}
		return tList.item(0);
	}

	protected static Node getTile(int tileIdx){
		return tList.item(tileIdx);
	}

	protected static TileCoord getTileCoord(Node tile){
		String[] tileCoord = tile.getAttributes().getNamedItem("id").getNodeValue().split("\\D");
		TileCoord tileId = new TileCoord(Integer.parseInt(tileCoord[1]), Integer.parseInt(tileCoord[2]));
		return tileId;
	}

	protected static TileCoord getDestCoord(Element slotE) {
		String[] coord = slotE.getElementsByTagName("na").item(0).getAttributes().getNamedItem("tx").getNodeValue().split("\\D");
		TileCoord destCoord = new TileCoord(Integer.parseInt(coord[1]), Integer.parseInt(coord[2]));
		return destCoord;
	}

	protected static int getNumOfNodes(){
		return tList.getLength();
	}

	protected static char findOutputPort(NodeList ports, char inPort){
		char outPort = ' ';
		for(int nodeIdx = 0; nodeIdx < ports.getLength(); nodeIdx++){
			if(ports.item(nodeIdx).getAttributes().getNamedItem("input").getNodeValue().charAt(0) == inPort){
				outPort = ports.item(nodeIdx).getAttributes().getNamedItem("id").getNodeValue().charAt(0);
			}
		}
		return outPort;
	}

	protected static char oppositPort(char p){
		if(p == 'N'){p = 'S';}
		else if(p == 'E'){p = 'W';}
		else if(p == 'S'){p = 'N';}
		else if(p == 'W'){p = 'E';}
		else{p = 'L';}
		return p;
	}
}
