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
import java.util.ArrayList;
import java.util.List;
import org.w3c.dom.*;
import java.lang.System;
import java.nio.file.*;

/**
* A converter from xml format to Setup of DMA tables
* @author Rasmus Bo Sorensen
*/
public class Argo2Parser extends Parser {
  private static List<List<List<List<Integer> > > > initArray;
  private static final int SCHED_TBL = 0;
  private static final int CH_ID_TBL = 1;

  private static int numOfModes;
  private static int numOfNodes;
  private static List<NodeList> tListArray;
  private static boolean showStats;
  private static boolean showMinStats;

  private static final int TIME2NEXT_WIDTH = 5;
  private static final int PKTLEN_WIDTH = 3;
  private static final int DMANUM_WIDTH = 8;
  private static final int ROUTE_WIDTH = 16;


  @Override
  public void parse(){
    return;
  }

  public Argo2Parser(String[] inFiles, String outFile, Argo2Printer printer, boolean showStats, boolean showMinStats){
    numOfModes = inFiles.length;
    this.showStats = showStats;
    this.showMinStats = showMinStats;
    tListArray = new ArrayList<NodeList> (numOfModes);
    int width = 0;
    int firstWidth = 0;
    int height = 0;
    int firstHeight = 0;
    for (String file : inFiles) {
      try{
        File fXmlFile = new File(file);
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
        doc = dBuilder.parse(fXmlFile);
        width = Integer.parseInt(doc.getDocumentElement().getAttribute("width"));
        height = Integer.parseInt(doc.getDocumentElement().getAttribute("height"));
        //Check that the width and height is the same for all the modes.
        if (firstWidth != 0) {
          if (firstWidth != width) {
            System.out.println("Error: Modes belong to different platforms. The widths of the platforms do not match.");
            return;
          }
        } else {
          firstWidth = width;
        }
        if (firstHeight != 0) {
          if (firstHeight != height) {
            System.out.println("Error: Modes belong to different platforms. The heights of the platforms do not match.");
            return;
          }
        } else {
          firstHeight = height;
        }
        new TileCoord(0,0,width,height); // Initializing the static size variables in TileCoord
        doc.getDocumentElement().normalize();
        NodeList nList = doc.getElementsByTagName("tile");
        tListArray.add(nList);
        //numOfNodes = nList.getLength();
        numOfNodes = width * height;
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    initializeArray(numOfModes,numOfNodes);
    if (showMinStats) {
      System.out.println("Filename\t\tEntries\tBits\tComp Entries\tComp Bits\tGain\tMax Entries");
    }
    int modeId = 0;
    for (NodeList tList : tListArray) {
      for_each_tile_timeslot(tList,modeId,inFiles[modeId]); 
      modeId++;
    }
    printer.open(outFile);
    printer.printMCData(initArray);
    printer.close();
  }

  private void for_each_tile_timeslot(NodeList tList, int modeId, String inFile){
    int nodes = tList.getLength();
    int timeslots = 0;
    int schedTblEntries = 0;
    int compressedSchedTblEntries = 0;
    int blindCompressedSchedTblEntries = 0;
    int minCompTblEntries = Integer.MAX_VALUE;
    int maxCompTblEntries = 0;
    for (int tileIdx = 0; tileIdx < tList.getLength(); tileIdx++) {
      
      //System.out.println("Core: " + tileIdx);
      Node tile = tList.item(tileIdx);
      NodeList slotList = getTimeslots(tile);
      TileCoord tileCoord = getTileCoord(tile);
      int schedIdx = -1;
      int prevChIdx = -1;
      int prevPktIdx = -1;
      int pktLen = 0;
      int time2Next = 0;
      boolean pktEnded = true;
      String prevRouteStr = "";
      /* For each time slot, schedule table and channel index table is written. */
      for (int slotIdx = 0; slotIdx < slotList.getLength(); slotIdx++) {
        timeslots = slotList.getLength();
        schedTblEntries++;
        Node slot = slotList.item(slotIdx);
        if (slot.getNodeType() != Node.ELEMENT_NODE) { continue; }
        Element slotE = (Element) slot;
        int chIdx = getChanId(slotE);
        int pktIdx = getPktId(slotE);
        TileCoord destCoord = getDestCoord(slotE);
        List<Integer> schedTbl = initArray.get(modeId).get(tileCoord.getTileId()).get(SCHED_TBL);
        int slotVal = 0;
        String routeStr = getRoute(slotE);
        //System.out.print("CONDITION: chIdx: " + chIdx + " prevChIdx: " + prevChIdx + " \tpktIdx: " + pktIdx + " prevPktIdx: " + prevPktIdx + " \tpktEnded: " + pktEnded);
        //if (chIdx != -1 && (pktEnded || (chIdx != prevChIdx || !routeStr.equals(prevRouteStr)))) {
        if (chIdx != -1 && (pktEnded || (chIdx != prevChIdx || pktIdx != prevPktIdx))) {
          //System.out.println(" Entered");
          // First time slot of a new channel
          // Set Route, DMA number, Packet length, and time to next
          // Set the DMA num in the channel id table
          pktEnded = false;
          schedIdx++;
          int route = route2bin(routeStr);
          int dmaNum = destCoord.getTileId();
          pktLen = 0;
          time2Next = 0;
          slotVal = route << (TIME2NEXT_WIDTH + PKTLEN_WIDTH + DMANUM_WIDTH) |
                    dmaNum << (TIME2NEXT_WIDTH + PKTLEN_WIDTH) |
                    pktLen << TIME2NEXT_WIDTH |
                    time2Next;
          schedTbl.add(schedIdx,slotVal);
          //System.out.println("Start of packet: slotIdx: " + slotIdx + " schedIdx: " + schedIdx + " route: " + route);
          time2Next++;
        } else if (chIdx != -1 && chIdx == prevChIdx && pktIdx == prevPktIdx && !pktEnded) {
          // A later time slot of the current channel
          // Update Packet length and time to next
          pktLen++;
          slotVal = schedTbl.get(schedIdx);
          slotVal &= ~((int)Math.pow(2,TIME2NEXT_WIDTH + PKTLEN_WIDTH)-1); // set the lower 8 bits to 0
          slotVal = slotVal | (pktLen << TIME2NEXT_WIDTH) | time2Next;
          schedTbl.set(schedIdx,slotVal);
          //System.out.println("\t\t slotIdx: " + slotIdx + " schedIdx: " + schedIdx + " pktLen: " + pktLen + " time2Next: " + time2Next);
          time2Next++;
          //System.out.println("");
        } else if (chIdx == -1 && prevChIdx != -1) {
          // An in between channels time slot
          // Update time to next of previous channel
          pktEnded = true;
          slotVal = schedTbl.get(schedIdx);
          slotVal &= ~((int)Math.pow(2,TIME2NEXT_WIDTH)-1); // set the lower 5 bits to 0
          slotVal = slotVal | time2Next;
          schedTbl.set(schedIdx,slotVal);
          //System.out.println("Between packets: slotIdx: " + slotIdx + " schedIdx: " + schedIdx + " pktLen: " + pktLen + " time2Next: " + time2Next);
          time2Next++;
          //System.out.println("");
        } else if (chIdx == -1 && prevChIdx == -1) {
          // No previous channel
          // Insert entry with DMA number equal all '1's
          // and update its time to next
          if (time2Next == 0) {
            int dmaNum = (1 << DMANUM_WIDTH) - 1;
            schedIdx++;
            //System.out.println(" Empty");
            blindCompressedSchedTblEntries++;
            pktLen = 0;
            slotVal = dmaNum << (TIME2NEXT_WIDTH + PKTLEN_WIDTH) |
                      pktLen << TIME2NEXT_WIDTH |
                      time2Next;
            schedTbl.add(schedIdx,slotVal);
          } else {
            int dmaNum = (1 << DMANUM_WIDTH) - 1;
            pktLen = 0;
            slotVal = dmaNum << (TIME2NEXT_WIDTH + PKTLEN_WIDTH) |
                      pktLen << TIME2NEXT_WIDTH |
                      time2Next;
            schedTbl.set(schedIdx,slotVal);
            //System.out.println("");
          }
          //System.out.println("No packet:\t slotIdx: " + slotIdx + " schedIdx: " + schedIdx + " pktLen: " + pktLen + " time2Next: " + time2Next);
          time2Next++;
          pktEnded = true;
        }
        if (chIdx != -1) {
          prevChIdx = chIdx; 
        }
        if (pktIdx != -1) {
          prevPktIdx = pktIdx; 
        }
        //if (time2Next == 32) {
        if (time2Next == (int)Math.pow(2,TIME2NEXT_WIDTH)) {
          time2Next = 0;
          prevChIdx = -1;
          prevPktIdx = -1;
        }
        prevRouteStr = routeStr;
        //initArray.get(tileCoord.getTileId()).get(CH_ID_TBL).set(destCoord.getTileId(), route);
      }
      // Add the index plus one, because we want to count the number and not the index.
      int numEntries = (schedIdx+1);
      compressedSchedTblEntries+=numEntries;
      if (numEntries > maxCompTblEntries) {
        maxCompTblEntries = numEntries;
      }
      if (numEntries < minCompTblEntries) {
        minCompTblEntries = numEntries;
      }
    }
    if (showStats || showMinStats) {
      int tableWidth = DMANUM_WIDTH+ROUTE_WIDTH;
      int baseTableWidth = tableWidth;
      int compressedTableWidth = TIME2NEXT_WIDTH+PKTLEN_WIDTH+tableWidth;
      int schedTblBits = tableWidth*schedTblEntries;
      int baseTblBits = baseTableWidth*(schedTblEntries/3);
      int compressedTblBits = compressedTableWidth*compressedSchedTblEntries;
      double naiveGain = ((schedTblBits-compressedTblBits)*100.0)/schedTblBits;
      double roundedNaiveGain = Math.round(naiveGain*100.0)/100.0;
      double baseGain = ((baseTblBits-compressedTblBits)*100.0)/baseTblBits;
      double roundedBaseGain = Math.round(baseGain*100.0)/100.0;
      if (showStats) {
        System.out.println("Mode " + modeId + " contains:\n"
                          +"\tnodes:\t\t\t" + nodes + "\n"
                          +"\ttimeslots:\t\t" + timeslots + "\n"
                          +"\tSchedule table:\n"
                          +"\t\tentries:\t" + schedTblEntries + "\n"
                          +"\t\twidth:\t\t" + tableWidth + "\n"
                          +"\t\tbits:\t\t" + schedTblBits + "\n"
                          +"\tBase schedule table:\n"
                          +"\t\tentries:\t" + (schedTblEntries/3) + "\n"
                          +"\t\twidth:\t\t" + baseTableWidth + "\n"
                          +"\t\tbits:\t\t" + baseTblBits + "\n"
                          +"\tCompressed schedule table:\n"
                          +"\t\tentries:\t" + compressedSchedTblEntries + "\n"
                          +"\t\tnull entries:\t" + blindCompressedSchedTblEntries + "\n"
                          +"\t\tmin entries:\t" + minCompTblEntries + "\n"
                          +"\t\tmax entries:\t" + maxCompTblEntries + "\n"
                          +"\t\twidth:\t\t" + compressedTableWidth + "\n"
                          +"\t\tbits:\t\t" + compressedTblBits + "\n"
                          +"\tTotal gain of compression over naive:\t" + roundedNaiveGain + "%\n"
                          +"\tTotal gain of compression over base:\t" + roundedBaseGain + "%\n");
      } else if (showMinStats) {
        Path p = Paths.get(inFile);
        String fileName = p.getFileName().toString();
        String name = String.format("%-24s", fileName);
        int len = name.length();
        //System.out.println("Filename: "+fileName+" length: "+len+" name: "+name.replace(' ', '*')+" length name: "+name.length());
        System.out.print(name.substring(len-23,len)+"\t");
        //System.out.println((schedTblEntries/3)+"\t"+baseTblBits+"\t"+compressedSchedTblEntries
        //                    +"\t\t"+compressedTblBits+"\t\t"+roundedBaseGain+"%\t" + maxCompTblEntries);
        System.out.println((schedTblEntries/3)+"\t"+baseTblBits+"\t"+compressedSchedTblEntries
                            +"\t\t"+compressedTblBits+"\t\t"+roundedBaseGain+"%\t" + maxCompTblEntries);
      }
    }
  }

  int route2bin(String route) {
    /* For each transmission slot write an entry in the route table */
    String binRoute = "";
    char prevTurn = 'L';
    for(int i = 0; i < route.length(); i++) {
      char c = route.charAt(i);
      if (c != 'L') {
        binRoute = port2bin(c) + binRoute;  
      } else {
        char lastTurn = oppositePort(prevTurn);
        binRoute = port2bin(lastTurn) + binRoute;
      }
      prevTurn = c;
    }
    return Integer.parseInt("0" + binRoute, 2);
  }

  protected String port2bin(char p){
    String bin;
    if(p == 'N'){bin = "00";}
    else if(p == 'E'){bin = "01";}
    else if(p == 'S'){bin = "10";}
    else if(p == 'W'){bin = "11";}
    else{bin = "00";}
    return bin;
  }

  private void initializeArray(int nrModes,int nrCpu){
    initArray = new ArrayList<List<List<List<Integer> > > > (nrModes);
    for (int j = 0; j < nrModes; j++) {
      initArray.add(new ArrayList<List<List<Integer> > > (nrCpu));
      for (int i = 0; i < nrCpu; i++) {
        initArray.get(j).add(new ArrayList<List<Integer> >(2));
        initArray.get(j).get(i).add(new ArrayList<Integer>());
        initArray.get(j).get(i).add(new ArrayList<Integer>());
  //      for(int j = 0; j < nrCpu; j++){
  //        initArray.get(i).get(ROUTE_TABLE).add(0);
  //      }
      }  
    }
  }
}





