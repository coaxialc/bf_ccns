/*
 * PTuple.h
 *
 *  Created on: Nov 26, 2013
 *      Author: Coaxial
 */

#ifndef PTUPLE_H_
#define PTUPLE_H_


#include "ns3/Bloomfilter.h"
#include "ns3/Receiver.h"

class Receiver;
class Bloomfilter;
using namespace std;


class PTuple : public ns3::Object
{
	public:
	ns3::Ptr<Bloomfilter> bf;
	int ttl;
	std::vector < ns3::Ptr < Receiver > >* r;
	PTuple(ns3::Ptr<Bloomfilter> bf,int ttl);
	~PTuple();
};


#endif /* PTUPLE_H_ */
