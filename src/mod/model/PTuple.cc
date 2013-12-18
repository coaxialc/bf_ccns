#include "ns3/PTuple.h"

PTuple::PTuple(ns3::Ptr<Bloomfilter> bf,int ttl)
{
	this->bf=bf;
	this->ttl=ttl;
}

PTuple::~PTuple()
{
	bf=0;
}
