/*
 * PTuple.h
 *
 *  Created on: Nov 26, 2013
 *      Author: Coaxial
 */

#ifndef PTUPLE_H_
#define PTUPLE_H_


#include "ns3/Bloomfilter.h"
class Bloomfilter;
using namespace std;


class PTuple : public ns3::Object
{
	public:
	ns3::Ptr<Bloomfilter> bf;
	int ttl;
	PTuple(ns3::Ptr<Bloomfilter> bf,int ttl);
	~PTuple();
};


#endif /* PTUPLE_H_ */
