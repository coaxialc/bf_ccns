#ifndef CCNMODULE_H
#define CCNMODULE_H

#include <map>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/Trie.h"
#include "ns3/Bloomfilter.h"
#include "ns3/PIT.h"
#include "ns3/CCN_Name.h"
#include "ns3/ccn-packets.h"

using std::string;
using std::map;


namespace ns3 {

class Trie;
class Bloomfilter;
class PIT;
class CCN_Data;
class CCN_Interest;
class CCN_Name;


class CcnModule: public Object {
public:

	static uint32_t RX_INTERESTS;
	static uint32_t RX_DATA;

	CcnModule(Ptr<Node>, int switchh);
	~CcnModule();
	virtual void DoDispose(void);

	uint8_t extract_packet_type(Ptr<const Packet> p);
	void reInit();
	string stringtobinarystring(string s);

	uint32_t bfForward(Ptr<Bloomfilter>, Ptr<CCN_Data>,	Ptr<NetDevice> );
	void sendThroughDevice(Ptr<const Packet> p, Ptr<NetDevice> nd);
	int decideTtl();

	uint32_t getTXData(); //const {return p_RX_Data;}
	uint32_t getNodeId() ;//const {return nodePtr->GetId(); }
	Ptr<Trie> getFIB();
	Ptr<PIT> getPIT();
	Ptr<Node> getNode();
	map<Ptr<NetDevice>, Ptr<CcnModule> > getNeighborModules();

	//for LocalApp
	void sendInterest(Ptr<CCN_Name> name, Ptr<LocalApp>);
	void sendData(Ptr<CCN_Name>, uint8_t *buff, uint32_t bufflen);

	bool handlePacket(Ptr<NetDevice> nd, Ptr<const Packet> p, uint16_t a, const Address& ad);
	void handleIncomingData(Ptr<const Packet> p, Ptr<NetDevice> nd);
	void handleIncomingInterest(Ptr<const Packet> p, Ptr<NetDevice> nd);

	void installLIDs();

	friend bool operator< (const Ptr<NetDevice>&, const Ptr<NetDevice>&);

private:
	uint32_t p_RX_Data;
	uint32_t switchh;

	Ptr<Trie> FIB;
	Ptr<PIT> thePIT;
	Ptr<Node> nodePtr;
	map<Ptr<NetDevice>, Ptr<CcnModule> > neighborModules;
	map<Ptr<NetDevice>, Ptr<Bloomfilter> > deviceToLid;
	map<Ptr<NetDevice>, Address> addresses;

	void doSendInterest(Ptr<CCN_Name> name, Ptr<LocalApp>);
	void doSendData(Ptr<CCN_Name>, uint8_t *buff, uint32_t bufflen);

	static Time ONE_NS;

};
}
#endif

