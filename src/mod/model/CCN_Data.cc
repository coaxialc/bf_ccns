#include "ns3/CCN_Data.h"
#include <string>
#include <cstdlib>

using namespace ns3;

void CCN_Data::decreaseHopCounter()
{
	this->hc--;
}

int CCN_Data::getLength()
{
	return dataLength;
}

uint8_t* CCN_Data::getData()
{
	return data;
}

Ptr<CCN_Name> CCN_Data::getName()
{
	return this->name;
}

Ptr<Bloomfilter> CCN_Data::getBloomfilter()
{
	return this->bf;
}

void CCN_Data::setBloomfilter(ns3::Ptr<Bloomfilter> bf)
{
	this->bf=bf;
}

void CCN_Data::setTtl(int ttl)
{
	this->hc=ttl;
}

CCN_Data::CCN_Data(ns3::Ptr<const ns3::Packet> p,int filterLength)
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

	/*std::string value=dt.substr(pos+1,dt.length()-pos-2);
	uint8_t* v=&value[0];*/

	data=&b2[filterLength+3+pos];
	dataLength=p->GetSize()-3-pos-filterLength;
}

ns3::Ptr<ns3::Packet> CCN_Data::serializeToPacket()
{
	std::string temp(reinterpret_cast<char*>(data),dataLength);
	std::string payload=bf->getstring()+stringForm(hc)+name->getValue()+temp+"d";
	ns3::Ptr<ns3::Packet> p=Create<ns3::Packet>(reinterpret_cast<const uint8_t *>(&payload[0]),payload.length());
	return p;
}

std::string CCN_Data::stringForm(int hc)
{
	if(hc>9)
	{
		return static_cast<ostringstream*>( &(ostringstream() << hc) )->str();
	}
	else
	{
		return "0"+static_cast<ostringstream*>( &(ostringstream() << hc) )->str();
	}
}

CCN_Data::~CCN_Data()
{
	bf=0;
	name=0;
	delete data;
}

int CCN_Data::getHopCounter()
{
	return this->hc;
}
