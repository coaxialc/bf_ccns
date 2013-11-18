/*
 * BootstrappingHelper.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Coaxial
 */

#ifndef BOOTSTRAPPINGHELPER_H_
#define BOOTSTRAPPINGHELPER_H_

#include "ns3/mod-module.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

class Text;
class CcnModule;
class Initializer;


using namespace ns3;


class BootstrappingHelper : public Object
{
	public:
	Ptr<UniformRandomVariable> r;
	//int pitEntryForExp;

	Ptr<Text> text;
	Ptr<Parser> p;
	std::vector < Ptr < CcnModule > > * module;
	Ptr<CCN_Name> name2;
	Ptr<Initializer> i;
	std::vector< Ptr < Object > >* vec3;
	Ptr<Receivers> rec3;
	std::vector <vertex> select(std::vector <vertex>,unsigned gs);
	vertex selectOwner(std::vector <vertex>,std::vector <vertex>);
	void PITCheck(int gs,int exp);
	unsigned gs;
	void specificData();
	std::string filename;

	typedef boost::property_map<graph, boost::vertex_index_t>::type im;
	im in;



	BootstrappingHelper(unsigned int s,std::string filename,int gsize);
	~BootstrappingHelper();
	void parseTopology();

	void startExperiment();
};


#endif
