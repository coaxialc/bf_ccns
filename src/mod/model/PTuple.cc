#include "ns3/PTuple.h"

PTuple::PTuple(ns3::Ptr<Bloomfilter> bf,int ttl)
{
	this->bf=bf;
	this->ttl=ttl;
	r= new std::vector < ns3::Ptr< Receiver > >();
}

PTuple::~PTuple()
{
	bf=0;
}
