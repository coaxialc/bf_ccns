/*
 * BootstrappingHelper.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Coaxial
 */

#ifndef BOOTSTRAPPINGHELPER_H_
#define BOOTSTRAPPINGHELPER_H_

#include "ns3/mod-module.h"
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "ns3/CcnModule.h"
#include "ns3/Initializer.h"



using std::string;
using std::vector;

namespace ns3
{
class Text;
class Initializer;
class CCN_Name;

class BootstrappingHelper : public Object
{
	public:
	Ptr<UniformRandomVariable> r;
	//int pitEntryForExp;

	//Ptr<Text> text;
	Ptr<Parser> p;
	vector < Ptr < CcnModule > > * module;
	Ptr<CCN_Name> name2;
	Ptr<Initializer> i;
	Ptr<NetDevice> ndfinder(Ptr<Node> n1,Ptr<Node> n2,uint32_t,uint32_t);
	vector< Ptr < Object > >* vec3;
//	Ptr<Receivers> rec3;
	vector <vertex> select(vector <vertex>,unsigned gs);

	vertex selectOwner(vector <vertex>,vector <vertex>);
	void PITCheck(int gs,int exp);
	unsigned gs;
	void specificData();
	std::string filename;

	int length,switchh,d;

	typedef boost::property_map<graph, boost::vertex_index_t>::type im;
	im in;
	//void staticStart(unsigned int s,string filename,int gsize,int length,int d,int switchh);



	BootstrappingHelper(unsigned int s,std::string filename,int gsize,int length,int d,int switchh);
	~BootstrappingHelper();
	void parseTopology();

	void startExperiment();
};
}


#endif
