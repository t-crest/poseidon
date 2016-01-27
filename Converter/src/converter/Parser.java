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
		parseXml(inFile);
		printer.open(outFile);
		parse();
		printer.close();
	}

	protected abstract void parse();

	protected static void parseXml(String inputFile){
		try{
			File fXmlFile = new File(inputFile);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			doc = dBuilder.parse(fXmlFile);
			int width = Integer.parseInt(doc.getDocumentElement().getAttribute("width"));
			int height = Integer.parseInt(doc.getDocumentElement().getAttribute("height"));
			new TileCoord(0,0,width,height); // Initializing the static size variables in TileCoord
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

	protected static NodeList getTimeslots(Node tile) {
		if (tile.getNodeType() != Node.ELEMENT_NODE) {throw new Error("Tile is not element node");}
		Element tileE = (Element) tile;
		TileCoord tileCoord = getTileCoord(tile);

		return tileE.getElementsByTagName("timeslot");
	}

	protected static TileCoord getDestCoord(Element slotE) {
		String[] coord = slotE.getElementsByTagName("na").item(0).getAttributes().getNamedItem("tx").getNodeValue().split("\\D");
		TileCoord destCoord = new TileCoord(Integer.parseInt(coord[1]), Integer.parseInt(coord[2]));
		return destCoord;
	}

	protected static int getChanId(Element slotE) {
		int chanId = -1;
		try {
			String id = slotE.getElementsByTagName("na").item(0).getAttributes().getNamedItem("chan-id").getNodeValue();
			chanId = Integer.parseInt(id);
		} catch (Exception e){
			chanId = -1;
		}
		return chanId;
	}

	protected static int getPktId(Element slotE) {
		int pktId = -1;
		try {
			String id = slotE.getElementsByTagName("na").item(0).getAttributes().getNamedItem("pkt-id").getNodeValue();
			pktId = Integer.parseInt(id);
		} catch (Exception e){
			pktId = -1;
		}
		return pktId;
	}

	protected static String getRoute(Element slotE) {
		String route = "";
		try {
			route = slotE.getElementsByTagName("na").item(0).getAttributes().getNamedItem("route").getNodeValue();
		} catch (Exception e) {
			route = "";
		}
		return route;
	}

	protected static int getNumOfNodes(){
		return tList.getLength();
	}

	protected TileCoord nextTile(TileCoord tileCoord, char outPort){
		switch (outPort) {
		case 'N': return tileCoord.moveNorth();
		case 'E': return tileCoord.moveEast();
		case 'S': return tileCoord.moveSouth();
		case 'W': return tileCoord.moveWest();
		}
		// If local port do nothing
		return tileCoord;
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

	protected static char oppositePort(char p){
		switch (p) {
		case 'N': return 'S';
		case 'E': return 'W';
		case 'S': return 'N';
		case 'W': return 'E';
		default: return 'L';
		}
	}
}
