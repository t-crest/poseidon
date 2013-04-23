
package converter;

import java.util.ArrayList;
import java.util.List;
import org.w3c.dom.*;

/**
*	A converter from xml format to Setup of DMA tables
*	@author Rasmus Bo Sorensen
*/
public class SourceParser extends Parser {
	private static List<List<List<Integer> > > initArray;
	private static final int SLOT_TABLE = 0;
	private static final int ROUTE_TABLE = 1;
	private static final int ROUTER_DEPTH = 1;

	public SourceParser(){
		
	}
	
	@Override
	public void parse() {
		try {
			int numOfNodes = getNumOfNodes();
			initializeArray(numOfNodes);
			System.out.println("numOfNodes:" + numOfNodes);
			for_each_tile_timeslot();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Printing data...");
		printer.printData(initArray);
	}
	
	private static void for_each_tile_timeslot(){
		int slotTableWidth = (int)Math.ceil(Math.log(getNumOfNodes())/Math.log(2));
		System.out.println("slotTableWidth: " + slotTableWidth);
		for (int tileIdx = 0; tileIdx < getNumOfNodes(); tileIdx++) {
			Node tile = getTile(tileIdx);
			NodeList slotList = getTimeslots(tile);
			TileCoord tileCoord = getTileCoord(tile);
			System.out.println("Tile coordinate: " + tileCoord.toString());
			/* For each time slot, slot table and route table is written. */
			for (int slotIdx = 0; slotIdx < slotList.getLength(); slotIdx++) {
				Node slot = slotList.item(slotIdx);
				if (slot.getNodeType() != Node.ELEMENT_NODE) { continue; }
				Element slotE = (Element) slot;
				// Get the coordinates of the receiver for this timeslot
				TileCoord destCoord = getDestCoord(slotE);
				System.out.println("DestTile coordinate: " + destCoord.toString());
				// Write the destination ID in the slot table.
				//int slotVal = destCoord.getTileId() | (1 << slotTableWidth);
				int slotVal = destCoord.getTileId();
				System.out.println("slotVal: " + slotVal);
				initArray.get(tileCoord.getTileId()).get(SLOT_TABLE).add(slotIdx,slotVal);
				System.out.println("Finding route...");
				int route = find_route(destCoord, tileCoord, slotIdx);
				// Write the route to the route table in the initArray.
				initArray.get(tileCoord.getTileId()).get(ROUTE_TABLE).set(destCoord.getTileId(), route);
			}
		}
	}
	
	private static int find_route(TileCoord destCoord, TileCoord tileCoord, int slotIdx) throws NumberFormatException {
		/* For each transmission slot write an entry in the route table */
		String binRoute = "";
		if (destCoord.getTileId() != tileCoord.getTileId()){
			TileCoord tempTileCoord = new TileCoord(tileCoord.x,tileCoord.y); // Make temporary tile
			char inPort = 'L';
			for(int i = 0; tempTileCoord.getTileId() != destCoord.getTileId(); i++){ // Increment i with the router depth, plus the link depth on the given link
				NodeList ports = getPorts(tempTileCoord,slotIdx+(i*ROUTER_DEPTH));
				char outPort = findOutputPort(ports,inPort);

				System.out.println("Outport:" + outPort);
				binRoute = port2bin(outPort) + binRoute; // Must not be changed to binRoute += port2bin(outport), this is string concatenation
				inPort = oppositPort(outPort);
				nextTile(tempTileCoord,outPort);
			}
			// Must not be changed to binRoute += port2bin(outport), this is string concatenation
			binRoute = port2bin(inPort) + binRoute; // Route to local port
		}
		System.out.println("binRoute:" + binRoute);
		return Integer.parseInt("0" + binRoute, 2);
	}
	
	private static NodeList getTimeslots(Node tile) {
		if (tile.getNodeType() != Node.ELEMENT_NODE) {throw new Error("Tile is not element node");}
		Element tileE = (Element) tile;
		TileCoord tileCoord = getTileCoord(tile);

		return tileE.getElementsByTagName("timeslot");
	}
	
	private static void nextTile(TileCoord tileCoord, char outPort){
		if(outPort == 'N'){tileCoord.moveNorth();}
		else if(outPort == 'E'){tileCoord.moveEast();}
		else if(outPort == 'S'){tileCoord.moveSouth();}
		else if(outPort == 'W'){tileCoord.moveWest();}
		// If local port do nothing
	}
	
	private static String port2bin(char p){
		String bin;
		if(p == 'N'){bin = "00";}
		else if(p == 'E'){bin = "01";}
		else if(p == 'S'){bin = "10";}
		else if(p == 'W'){bin = "11";}
		else{bin = "00";}
		return bin;
	}
	
	private static void initializeArray(int nrCpu){
		initArray = new ArrayList<List<List<Integer> > >(nrCpu);
		for (int i = 0; i < nrCpu; i++) {
			initArray.add(new ArrayList<List<Integer> >(2));
			initArray.get(i).add(new ArrayList<Integer>());
			initArray.get(i).add(new ArrayList<Integer>(nrCpu));
			for(int j = 0; j < nrCpu; j++){
				initArray.get(i).get(ROUTE_TABLE).add(0);
			}
		}
	}
}



		
 
