/*
 * Initializer.h
 *
 *  Created on: Sep 17, 2013
 *      Author: Coaxial
 */

#ifndef INITIALIZER_H_
#define INITIALIZER_H_

#include "ns3/core-module.h"
#include "ns3/CcnModule.h"

class CcnModule;
	class Initializer  : public ns3::Object
	{
		public:
		Initializer(std::vector < ns3::Ptr < CcnModule > >* module,std::vector< std::vector < int > >* connection,int dataOwner);

		virtual ~Initializer();

		void initializeFIBs();
		int dataOwner;
		ns3::Ptr<CcnModule> firstUnvisitedChild(ns3::Ptr<CcnModule> c);
		std::vector < ns3::Ptr < CcnModule > > module;
		std::vector< std::vector < int > >* connection;
		ns3::Ptr<ns3::NetDevice> ndfinder(ns3::Ptr<ns3::Node> n1,ns3::Ptr<ns3::Node> n2,int i,int j);
	};




#endif /* INITIALIZER_H_ */
