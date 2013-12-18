
#ifndef CCN_Data_H_
#define CCN_Data_H_


#include "ns3/mod-module.h"

using namespace std;

class CCN_Data : public ns3::Object
{
    public:
	void decreaseHopCounter();
	Ptr<Bloomfilter> getBloomfilter();
	int getHopCounter();
	CCN_Data(ns3::Ptr<ns3::Packet> p,int filterLength);
	Ptr<CCN_Name> getName();
	~CCN_Data();

    private:
	int hc;
	Ptr<Bloomfilter> bf;
	Ptr<CCN_Name> name;
	uint8_t * data;
	int dataLength;
};

#endif
