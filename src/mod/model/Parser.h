
#ifndef PARSER_H_
#define PARSER_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> graph;
typedef boost::graph_traits<graph>::vertex_descriptor vertex;

	class Parser : public ns3::Object
	{
		public:
		std::vector <  ns3::Ptr<ns3::Node>  >* kombos;
		std::vector< std::vector < int > >* connection;
		ns3::NetDeviceContainer ndc;
		std::vector <unsigned>* v1;
		std::vector <unsigned>* v2;
		std::string filename;

		Parser(std::string filename);

		~Parser();


		void parse();
		graph topology;
		graph getGraph();
	};




#endif /* PARSER_H_ */
