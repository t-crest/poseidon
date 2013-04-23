
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
		System.out.println("slotIdx:" + slotIdx);
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
