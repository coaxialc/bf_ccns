
#ifndef CCN_Data_H_
#define CCN_Data_H_


#include "ns3/mod-module.h"

using namespace std;

class CCN_Data : public ns3::Object
{
    public:
	void decreaseHopCounter();
	ns3::Ptr<Bloomfilter> getBloomfilter();
	void setBloomfilter(ns3::Ptr<Bloomfilter> bf);
	void setTtl(int ttl);
	uint8_t* getData();
	int getHopCounter();
	CCN_Data(ns3::Ptr<const ns3::Packet> p,int filterLength);
	ns3::Ptr<CCN_Name> getName();
	~CCN_Data();
	int getLength();
	ns3::Ptr<ns3::Packet> serializeToPacket();
	std::string stringForm(int hc);

    private:
	int hc;
	ns3::Ptr<Bloomfilter> bf;
	ns3::Ptr<CCN_Name> name;
	uint8_t * data;
	int dataLength;
};

#endif
