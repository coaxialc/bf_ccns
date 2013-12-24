
#ifndef CCN_INTEREST_H_
#define CCN_INTEREST_H_


#include "ns3/mod-module.h"

using namespace std;

class CCN_Interest : public ns3::Object
{
    public:
	void decreaseHopCounter();
	ns3::Ptr<Bloomfilter> getBloomfilter();
	int getHopCounter();
	CCN_Interest(ns3::Ptr<const ns3::Packet> p,int filterLength);
	CCN_Interest(ns3::Ptr<CCN_Name> name,int ttl,int filterLength);
	void setTtl(int ttl);
	ns3::Ptr<CCN_Name> getName();
	//void resetHC();
	//void resetBF();
	ns3::Ptr<ns3::Packet> serializeToPacket();
	std::string stringForm(int hc);
	~CCN_Interest();

    private:
	int hc;
	ns3::Ptr<Bloomfilter> bf;
	ns3::Ptr<CCN_Name> name;
};

#endif
