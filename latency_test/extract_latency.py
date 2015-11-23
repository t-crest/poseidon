#! /usr/bin/env python3
import sys
import util
from lxml import etree
import subprocess

applications = ["RS-32_28_8_dec", "RS-32_28_8_enc", "FFT-1024_complex", "Sparse", "Robot", "Fpppp", "H264-720p_dec", "H264-1080p_dec"]
relative_mcsl_path = '../MCSL/xml/torus/torus_4x4/'

def parseXML(filename):
	parser = etree.XMLParser(remove_comments=True)
	tree = etree.parse(filename, parser=parser)
	tree.xinclude()
	return tree.getroot()


def extract_from_sched(filename):
	schedule = parseXML(filename)
	tdm_lenght = schedule.get("length")
	average_WCTT = 0
	min_WCTT = sys.maxsize
	max_WCTT = 0
	count = 0
	average_rate = 0
	min_rate = sys.maxsize
	max_rate = 0

	tiles = list(schedule)
	for i in range(0,len(tiles)):
		timeslot = list(tiles[i])
		out_rate = 0
		for j in range(0,len(timeslot)):
			if timeslot[j].tag == "latency":
				dest = list(timeslot[j])
				count += len(dest)
				for k in range(0,len(dest)):
					chan_id = dest[k].get("chan-id")
					slt_wait = dest[k].get("slotwaittime")
					chan_lat = dest[k].get("channellatency")
					WCTT = int(slt_wait) + int(chan_lat)
					rate = float(dest[k].get("rate"))
					out_rate += rate
					#print(str(chan_id) + " " + str(WCTT) + " " +  str(rate))
					if WCTT < min_WCTT:
						min_WCTT = WCTT
					if WCTT > max_WCTT:
						max_WCTT = WCTT
					average_WCTT += WCTT
					if rate < min_rate:
						min_rate = rate
					if rate > max_rate:
						max_rate = rate
					average_rate += rate
		if out_rate > 1:
			raise SystemExit(__file__ +': Error: rate out of node is higher than one.')
					
	average_WCTT = average_WCTT/count
	average_rate = average_rate/count
	#print("length\tmax_rate\tmax_WCTT\taverage_WCTT\tmin_WCTT")
	print(tdm_lenght+"\t"+str(count)+"\t"+str(min_WCTT)+"\t\t"+str(round(average_WCTT,1))+"\t\t"+str(max_WCTT)+"\t\t"+str(round(min_rate,4))+"\t\t"+str(round(average_rate,4))+"\t\t"+str(round(max_rate,4)))

def poseidon(infile,outfile):
	Poseidon = ['poseidon']
	Poseidon+= ['-p',infile]  # Platform specification
	Poseidon+= ['-s',outfile] # XML Schedule output
	Poseidon+= ['-m','GRASP']       # Optimization algorithm
	Poseidon+= ['-b','0.02']        # Beta value
	Poseidon+= ['-t','200']         # Time to run
	Poseidon+= ['-a']               # Draw the topology
	Poseidon+= ['-v','2']           # Use poseidon for version 2
	ret = subprocess.call(Poseidon)
	if ret != 0:
		raise SystemExit(__file__ +': Error: poseidon: ' + str(ret))

extract_from_sched(sys.argv[1])

# Generate schedules with poseidon
#for x in range(0,len(applications)):
#	print(applications[x])
#	poseidon(relative_mcsl_path+applications[x]+"_torus_4x4_max_rate.stp.xml",applications[x]+"_sched.xml")

# Extract latency information from the generated schedules
#for x in range(0,len(applications)):
#	print(applications[x])
#	extract_from_sched(applications[x]+"_sched.xml")