
#ifndef CCN_INTEREST_H_
#define CCN_INTEREST_H_


#include "ns3/mod-module.h"

using namespace std;

class CCN_Interest : public ns3::Object
{
    public:
	void decreaseHopCounter();
	Ptr<Bloomfilter> getBloomfilter();
	int getHopCounter();
	CCN_Interest(ns3::Ptr<ns3::Packet> p,int filterLength);
	CCN_Interest(ns3::Ptr<CCN_Name> name,int ttl);
	void setTtl(int ttl);
	Ptr<CCN_Name> getName();
	~CCN_Interest();

    private:
	int hc;
	Ptr<Bloomfilter> bf;
	Ptr<CCN_Name> name;
};

#endif
