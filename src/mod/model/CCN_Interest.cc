#include "ns3/CCN_Interest.h"
#include <string>
#include <cstdlib>

using namespace ns3;

void CCN_Interest::decreaseHopCounter()
{
	this->hc--;
}

Ptr<Bloomfilter> CCN_Interest::getBloomfilter()
{
	return this->bf;
}

CCN_Interest::CCN_Interest(ns3::Ptr<CCN_Name> name,int ttl)
{
	this->name=name;
	bf=CreateObject<Bloomfilter>();
	hc=ttl;
}

void CCN_Interest::setTtl(int ttl)
{
	hc=ttl;
}

Ptr<CCN_Name> CCN_Interest::getName()
{
	return this->name;
}

CCN_Interest::CCN_Interest(ns3::Ptr<ns3::Packet> p,int filterLength)
{
	uint8_t* b2=new uint8_t[p->GetSize()];
	p->CopyData(b2,p->GetSize());
	std::string dt(b2, b2+p->GetSize());

	//extract bloom filter (variable length)
	//-----------------------------------
	std::string filter_string=dt.substr(0,filterLength);

	bf=CreateObject<Bloomfilter>(filterLength,filter_string);
	//-----------------------------------

	//extract hop counter (2 characters)
	//-----------------------------------
	std::string hopcounter=dt.substr(filterLength,2);
	this->hc=std::atoi(hopcounter.c_str());
	//-----------------------------------

	dt=dt.substr(filterLength+2);

	int pos=dt.find("*");
	std::string prename=dt.substr(0,pos);//pairnoume to onoma xoris *

	std::string name_value=prename;

	static std::string* name2=&name_value;
	this->name=Text::getPtr()->giveText(name2);
	(this->name)->name.erase(name->name.begin());
	//ola ta onomata ksekinane me / alla den exoune / sto telos
}

CCN_Interest::~CCN_Interest()
{
	bf=0;
	name=0;
}

int CCN_Interest::getHopCounter()
{
	return this->hc;
}
