/*
 * ResultPrinter.h
 *
 *  Created on: Oct 8, 2013
 *      Author: Coaxial
 */

#ifndef RESULTPRINTER_H_
#define RESULTPRINTER_H_

#include "ns3/core-module.h"
#include "ns3/CcnModule.h"
#include "ns3/Sender.h"

class Sender;

class ResultPrinter : public ns3::Object
{
	public:
		ResultPrinter(std::vector < ns3::Ptr < CcnModule > > * module,int gs,int exp,ns3::Ptr<Sender> s);
		std::vector < ns3::Ptr < CcnModule > > * module;
		~ResultPrinter();

};

#endif /* RESULTPRINTER_H_ */
