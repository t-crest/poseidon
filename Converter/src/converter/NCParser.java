package converter;

import org.w3c.dom.*;
import java.util.*;
import java.lang.System;
import java.io.PrintWriter;

public class NCParser extends Parser {
    
    static final List<Character> PORTS = new ArrayList<Character>();
    static {
        PORTS.add('L');
        PORTS.add('N');
        PORTS.add('E');
        PORTS.add('S');
        PORTS.add('W');
    }

    private static class Route extends ArrayList<Node> {
        @Override
        public String toString() {
            StringBuilder b = new StringBuilder("[local->");
            for (Node n : this) {
                b.append(getTileCoord(n).toString());
                b.append("->");
            }
            b.append("local]");
            return b.toString();
        }
    }

    private static class Link {
        public final Node source;
        public final Node dest;

        public Link(Node source, Node dest) {
            this.source = source;
            this.dest = dest;
        }

        @Override
        public int hashCode() {
            int srcHash = source == null ? 0 : source.hashCode();
            int dstHash = dest == null ? 0 : dest.hashCode();
            return srcHash*31 + dstHash;
        }

        @Override
        public boolean equals(Object o) {
            if (o instanceof Link) {
                Link l = (Link)o;
                boolean srcEq = source == null ? l.source == null : source.equals(l.source);
                boolean dstEq = dest == null ? l.dest == null : dest.equals(l.dest);
                return srcEq && dstEq;
            }
            return false;
        }

        @Override
        public String toString() {
            String srcStr = source == null ? "local" : getTileCoord(source).toString();
            String dstStr = dest == null ? "local" : getTileCoord(dest).toString();
            return srcStr + "->" + dstStr;
        }
    }

    public NCParser() {
    }

    private PrintWriter out;

    @Override
	public void start(String inFile, String outFile, Printer printer) {
		parseXml(inFile);
		parse();
        printConstraints(outFile);
	}

    private int schedLength;
    // the channels/flows
    private Map<String, Route> channels = new LinkedHashMap<String, Route>();
    // the channel rates/bandwidths
    private Map<String, Double> rates = new LinkedHashMap<String, Double>();
    // map to record which channels use a link
    private Map<Link, Set<String>> linkChannels = new LinkedHashMap<Link, Set<String>>();
    // maps to record which channels use which input/output ports a router
    private Map<Node, Map<Character, Set<String>>> routerInChannels = new LinkedHashMap<Node, Map<Character, Set<String>>>();
    private Map<Node, Map<Character, Set<String>>> routerOutChannels = new LinkedHashMap<Node, Map<Character, Set<String>>>();

    @Override
	protected void parse() {
		try {
            // length of schedule
			schedLength = Integer.parseInt(doc.getDocumentElement().getAttribute("length"));
            
            // find all tiles and create routers
            Set<Node> tiles = new LinkedHashSet<Node>();
            for (int i = 0; i < getNumOfNodes(); i++) {
                Node tile = getTile(i);
                tiles.add(tile);
                initRouterChannels(tile, routerInChannels);
                initRouterChannels(tile, routerOutChannels);
            }            

            // find all channels originating from a tile
            for (Node t : tiles) {
                Map<String, String> rawChannels = new LinkedHashMap<String, String>();

                // check all timeslots for channels/routes
                NodeList slots = ((Element)t).getElementsByTagName("timeslot");
                for (int i = 0; i < slots.getLength(); i++) {
                    Node na = ((Element)slots.item(i)).getElementsByTagName("na").item(0);
                    NamedNodeMap naAttrs = na.getAttributes();
                    Node idNode = naAttrs.getNamedItem("chan-id");
                    Node routeNode = naAttrs.getNamedItem("route");
                    if (idNode != null && routeNode != null) {
                        rawChannels.put(idNode.getNodeValue(), routeNode.getNodeValue());
                    }
                }

                // convert route strings to a list of nodes
                for (String id : rawChannels.keySet()) {
                    String routeString = rawChannels.get(id);

                    // create a new route
                    Route route = new Route();
                    route.add(t);
                    // record usage of link
                    addLinkChannel(linkChannels, new Link(null, t), id);
                    // record use of router input port
                    routerInChannels.get(t).get('L').add(id);

                    TileCoord thisCoords = getTileCoord(t);
                    for (int i = 0; i < routeString.length(); i++) {
                        // move according to route
                        char outPort = routeString.charAt(i);
                        TileCoord lastCoords = thisCoords;
                        thisCoords = nextTile(thisCoords, outPort);
                        // add next router to route
                        if (outPort != 'L') {
                            route.add(getTile(thisCoords));
                        }
                        // record usage of link
                        Link l = new Link(getTile(lastCoords), outPort == 'L' ? null : getTile(thisCoords));
                        addLinkChannel(linkChannels, l, id);
                        // record use of output port of old router
                        routerOutChannels.get(getTile(lastCoords)).get(outPort).add(id);
                        // record use of input port of next router
                        if (outPort != 'L') {
                            char inPort = oppositePort(routeString.charAt(i));
                            routerInChannels.get(getTile(thisCoords)).get(inPort).add(id);
                        }
                    }

                    // map route to channel id
                    channels.put(id, route);
                }

                // get channel rates
                Node latency = ((Element)t).getElementsByTagName("latency").item(0);
                if (latency != null) {
                    NodeList chans = ((Element)latency).getElementsByTagName("destination");
                    for (int i = 0; i < chans.getLength(); i++) {
                        NamedNodeMap chanAttrs = chans.item(i).getAttributes();
                        Node idNode = chanAttrs.getNamedItem("chan-id");
                        Node rateNode = chanAttrs.getNamedItem("rate");
                        if (idNode != null && rateNode != null) {
                            rates.put(idNode.getNodeValue(),
                                      Double.parseDouble(rateNode.getNodeValue()));
                        }
                    }
                }
            }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

    protected void printConstraints(String outFile) {
        try {
            PrintWriter out = new PrintWriter(outFile);

            // print constraints
            out.println("using CP;");
            out.println();
            out.println("int Tmin = 1;");
            out.println("int Tmax = 512;");
            out.println("int Lmax = 3;");
            out.println("int Qsize = 401;");
            out.println();
            out.println("range idRange = 0.."+(channels.size()-1)+";");
            out.println("range rateRange = 1..Tmax div Lmax;");
            out.println();
            out.println("dvar int Tw in Tmin..Tmax;");
            out.println("dvar int r[idRange] in rateRange;");
            out.println("dexpr float rho[i in idRange] = r[i]*Lmax/Tw;");
            out.println();
            out.println("dexpr float minRho = min(i in idRange) rho[i];");
            out.println("dexpr float avgRho = sum(i in idRange) rho[i] / "+channels.size()+";");
            out.println("dexpr float maxRho = max(i in idRange) rho[i];");
            out.println();
            out.print("int hops [idRange] = [");
            for (int i = 0; i < channels.size(); i++) {
                Route r = channels.get(Integer.toString(i));
                out.print((r.size()+1));
                if (i < channels.size()-1) {
                    out.print(", ");
                }
            }
            out.println("];");
            out.println();
            out.println("dexpr float latency[i in idRange] = (1-rho[i])*Tw + (hops[i]-1)*Lmax/rho[i] + hops[i]*Lmax;");
            out.println();
            out.println("dexpr float minLat = min(i in idRange) latency[i];");
            out.println("dexpr float avgLat = sum(i in idRange) latency[i] / "+channels.size()+";");
            out.println("dexpr float maxLat = max(i in idRange) latency[i];");
            out.println("execute { cp.param.Workers = 10; cp.param.TimeLimit = 600; }");
            out.println();

            out.print("maximize ");
            for (String id : rates.keySet()) {
                Double rate = rates.get(id);
                out.print("log(rho["+id+"])*"+rate+"+");
            }            
            out.println("0;");
            out.println();

            out.println("constraints {");

            out.println("// Minimum rates");
            for (String id : rates.keySet()) {
                Double rate = rates.get(id);
                out.println("rho["+id+"] >= "+rate+";");
            }

            out.println("// Link constraints");
            for (Link l : linkChannels.keySet()) {
                for (String id : linkChannels.get(l)) {
                    out.print("rho["+id+"]+");
                }
                out.println("0 <= 1;\t// "+l);
            }
            out.println("// Queue backlog constraints");
            for (Node router : routerOutChannels.keySet()) {
                Map<Character, Set<String>> outPorts = routerOutChannels.get(router);
                Set<String> seen = new LinkedHashSet<String>();
                for (Character port : PORTS) {
                    // all channels that share the output port
                    Set<String> portChannels = outPorts.get(port);
                    // skip if only one channel uses the port, nothing to merge
                    if (portChannels.size() <= 1) continue;
                    for (String id : portChannels) {
                        // skip if constraint containing channels already has been generated
                        if (seen.contains(id)) continue;
                        // find channels that share the same input port
                        Set<String> sharedInChannels = new LinkedHashSet<String>(portChannels);
                        Character inPort = null;
                        for (Character p : PORTS) {
                            Set<String> inChannels = routerInChannels.get(router).get(p);
                            if (inChannels.contains(id)) {
                                sharedInChannels.retainAll(inChannels);
                                inPort = p;
                            }
                        }
                        // skip if input channels are the same as the output channels, nothing to merge
                        if (portChannels.equals(sharedInChannels)) break;
                        // generate constraint
                        for (String chan : sharedInChannels) {
                            int n = mergeCount(chan, channels.get(chan), router);
                            out.print(n+"*rho["+chan+"]+");
                            seen.add(chan);
                        }
                        int nb = sharedInChannels.size();
                        out.println("0 <= (Qsize - "+(nb-1)+"/"+nb+"*Tw)/Lmax;"+
                                    " // "+getTileCoord(router)+" "+inPort+"->"+port);
                    }
                }
            }            
            out.println("}");
            out.println();
            out.println("execute {");
            out.println("  writeln(\"Tw   = \", Tw);");
            out.println("  writeln(\"rate = \", r);");
            out.println("  for (i in idRange) {");
            out.println("    writeln(rho[i], \" \", latency[i]);");
            out.println("  }");
            out.println("  writeln(\"bandwidth (min,avg,max) = \", minRho, \" \", avgRho, \" \", maxRho);");
            out.println("  writeln(\"latency (min,avg,max) = \", minLat, \" \", avgLat, \" \", maxLat);");
            out.println("}");

            out.close();
        } catch (Exception e) {
			e.printStackTrace();
		}
    }

    private int mergeCount(String chan, Route route, Node last) {
        int n = 0;
        // iterate over route
        for (Node r : route) {
            // find all channels that merge into the same link
            Set<String> mergedChans = null;
            for (Character p : PORTS) {
                Set<String> m = routerOutChannels.get(r).get(p);
                if (m.contains(chan)) {
                    mergedChans = m;
                    break;
                }
            }
            // find how many ports merge into the same link
            for (Character p : PORTS) {
                Set<String> ins = routerInChannels.get(r).get(p);
                if (ins.contains(chan)) {
                    continue;
                }
                for (String m : mergedChans) {
                    if (ins.contains(m)) {
                        n++;
                        break;
                    }
                }
            }
            // stop when last router is reached
            if (r.equals(last)) {
                break;
            }
        }
        return n;
    }

    private void initRouterChannels(Node router, Map<Node, Map<Character, Set<String>>> routerChannels) {
        routerChannels.put(router, new LinkedHashMap<Character, Set<String>>());
        for (Character port : PORTS) {
            routerChannels.get(router).put(port, new LinkedHashSet<String>());
        }
    }

    private void addLinkChannel(Map<Link, Set<String>> linkChannels, Link link, String id) {
        if (!linkChannels.containsKey(link)) {
            linkChannels.put(link, new LinkedHashSet<String>());            
        }
        linkChannels.get(link).add(id);
    }
 
}