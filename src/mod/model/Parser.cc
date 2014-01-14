#include "Parser.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using std::ifstream;
using std::istringstream;

namespace ns3 {


Parser::Parser() {
	matrix_map = map<uint32_t, set<uint32_t> >();
	idToNode = map<uint32_t, Ptr<Node> >() ;
	nodeToId = map<uint32_t, uint32_t >();
}

Parser::~Parser() {
	//TODO implement correctly
}

void Parser::DoDispose(void){
	//TODO implement correctly
}


Ptr<Node> Parser::getNodeById(uint32_t nodeid){
	return idToNode[nodeid];
}

uint32_t Parser::findId(Ptr<Node> node){
	return nodeToId[node->GetId()];
}

vector<Ptr<Node> > Parser::getNeighbors(uint32_t graphNodeId){
	set<uint32_t> neighbors = matrix_map[graphNodeId];
	vector<Ptr<Node> > v;

	set<uint32_t>::iterator iter;
	for (iter=neighbors.begin(); iter!=neighbors.end(); iter++ ){
		uint32_t n_id = *iter;
		Ptr<Node> nodePtr = idToNode[n_id];
		v.push_back(nodePtr);
	}

	return v;
}

//new one
void Parser::parse(string& filepath) {
	ifstream f;

	string line; //std::cout<<this->filename<<std::endl;
	f.open(filepath.c_str());

	f.open(filepath.c_str());
	if (f.is_open()) {
		while (getline(f, line)) {
			istringstream s(line);
			string t;

			uint32_t position = 0;
			uint32_t sourceNode = 0;
			set<uint32_t> neighbors;
			while (getline(s, t, '\t')) {
				if (t.at(0) != '<') {
					position = atoi(t.c_str());
					position--;
					sourceNode = position;
				} else if (t.at(0) == '<') {
					//  	std::cout<<"pushing at pos: "<<position<<std::endl;
					uint32_t neighbor = atoi((t.substr(1, t.length() - 2)).c_str())	- 1;
					neighbors.insert(neighbor);
				}

				matrix_map[sourceNode] = neighbors;
			}
		}

		f.close();
	}
	map<uint32_t, set<uint32_t> >::iterator iter;
	NodeContainer n;
	for (iter=matrix_map.begin(); iter!=matrix_map.end(); iter++ ){
		uint32_t node = iter->first;
		Ptr<Node> nodePtr = CreateObject<Node>();
		n.Add(nodePtr);
		idToNode[node] = nodePtr;
		nodeToId[nodePtr->GetId()] = node;
	}

	map<uint32_t, set<uint32_t> > alreadyConnected;
	for (iter=matrix_map.begin(); iter!=matrix_map.end(); iter++ ){
		Ptr<Node> sourceNode = getNodeById(iter->first);
		vector<Ptr<Node> > neighbors = getNeighbors(iter->first);
		vector<Ptr<Node> >::iterator neighborsIter;
		for (neighborsIter=neighbors.begin(); neighborsIter!=neighbors.end(); neighborsIter++ ){
			uint32_t sourceNodeGraphId = nodeToId[sourceNode->GetId()];
			uint32_t neighborNodeGraphId = nodeToId[(*neighborsIter)->GetId()];
			uint32_t min = sourceNodeGraphId < neighborNodeGraphId? sourceNodeGraphId : neighborNodeGraphId;
			uint32_t max = sourceNodeGraphId >= neighborNodeGraphId? sourceNodeGraphId : neighborNodeGraphId;

			map<uint32_t, set<uint32_t> >::iterator find =alreadyConnected.find(min);
			if (find != alreadyConnected.end()){
				set<uint32_t> links = find->second;
				if (links.find(max) != links.end()){
					continue;
				}
			}

			NodeContainer n;
			n.Add(sourceNode);
			n.Add(*neighborsIter);

			PointToPointHelper pph;
			pph.SetQueue(string("ns3::DropTailQueue"),
					string("MaxPackets"),
					ns3::UintegerValue(10000), string("MaxBytes"),
					ns3::UintegerValue(10000));
			pph.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
			pph.SetChannelAttribute("Delay", StringValue("2ms"));
			NetDeviceContainer ndc = pph.Install(n);

			boost::add_edge(sourceNodeGraphId, neighborNodeGraphId, topology);
			alreadyConnected[min].insert(max);
		}
	}
}

graph Parser::getGraph() {
	return topology;
}

}
