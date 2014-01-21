#include "CcnModule.h"
#include "experiment_globals.h"

#include <iostream>
#include <sstream>
#include <string>
#include <bitset>
#include <list>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::bitset;
using std::list;

namespace ns3 {

uint32_t CcnModule::RX_INTERESTS = 0;
uint32_t CcnModule::RX_DATA = 0;

Time CcnModule::ONE_NS = NanoSeconds(1);

CcnModule::CcnModule(Ptr<Node> node, int switchh,Ptr<UniformRandomVariable> urv) {
	nodePtr = node;
	this->urv=urv;
	this->switchh = switchh; //if zero ,hop counters start randomly ,otherwise they all start at max ,which is d

	thePIT = CreateObject<PIT>();
	FIB = CreateObject<Trie>();
	p_RX_Data = 0;
	addresses = map<Ptr<NetDevice>, Address>();

	//std::cout<<"this has "<<nodePtr->GetNDevices()<<" devices"<<std::endl;

	for (uint32_t i = 0; i < nodePtr->GetNDevices(); i++) {

		/*if(nodePtr->GetDevice(0)==0)
		{
			std::cout<<"i 0 einai null"<<std::endl;
		}

		if(nodePtr->GetDevice(1)==0)
		{
			std::cout<<"i 1 einai null"<<std::endl;
		}*/

	//	std::cout<<"trying to access device "<<i<<std::endl;

		Ptr<NetDevice> device = nodePtr->GetDevice(i);
		device->SetReceiveCallback(
				MakeCallback(&CcnModule::handlePacket, this));

		Ptr<Channel> channel = device->GetChannel();
		Address adr;
		if (device == channel->GetDevice(0)) {
			adr = channel->GetDevice(1)->GetAddress();
		} else {
			adr = channel->GetDevice(0)->GetAddress();
		}
		addresses[device] = adr;
	}

	deviceToLid = map<Ptr<NetDevice>, Ptr<Bloomfilter> >();
}

CcnModule::~CcnModule() {
	thePIT = 0;
	FIB = 0;
	nodePtr = 0;
	deviceToLid.clear();
	addresses.clear();
}

void CcnModule::DoDispose(void) {
	thePIT = 0;
	FIB = 0;
	nodePtr = 0;
	deviceToLid.clear();
	addresses.clear();
}

void CcnModule::reInit() {
	thePIT = 0;
	thePIT = CreateObject<PIT>();
	p_RX_Data = 0;

	FIB = 0;
	FIB = CreateObject<Trie>();
}

void CcnModule::sendInterest(Ptr<CCN_Name> name, Ptr<LocalApp> localApp){
	Simulator::Schedule(CcnModule::ONE_NS, &CcnModule::doSendInterest, this, name, localApp);
}

void CcnModule::sendData(Ptr<CCN_Name> name, uint8_t *buff, uint32_t bufflen){
	uint8_t *newBuff = (uint8_t*)malloc(bufflen);
	memcpy(newBuff, buff, bufflen);
	Simulator::Schedule(CcnModule::ONE_NS, &CcnModule::doSendData, this, name, newBuff, bufflen);
}


void CcnModule::sendThroughDevice(Ptr<const Packet> p, Ptr<NetDevice> nd) {
	uint8_t* b = new uint8_t[p->GetSize()];
	p->CopyData(b, p->GetSize());
	Ptr<Packet> p2 = Create<Packet>(b, p->GetSize());

	if(nd==0)
	{
		std::cout << "device null" << std::endl;
	}

	bool sent = nd->Send(p2, addresses[nd], 0x88DD);

	if (!sent) {
		std::cout << "bytes dropped" << std::endl;
		std::cout << "packets dropped" << std::endl;
	}
}

bool CcnModule::handlePacket(Ptr<NetDevice> nd, Ptr<const Packet> p, uint16_t a,
		const Address& ad) {
	uint8_t type = extract_packet_type(p);
	if (type == CCN_Packets::INTEREST) {
		RX_INTERESTS++;
		handleIncomingInterest(p, nd);
	} else if (type == CCN_Packets::DATA) {
		RX_DATA++;
		p_RX_Data++;
		handleIncomingData(p, nd);
	}

	return true;
}

uint8_t CcnModule::extract_packet_type(Ptr<const Packet> p) {
	uint8_t b2 = 0;
	p->CopyData(&b2, sizeof(uint8_t));
	return b2;
}

void CcnModule::handleIncomingInterest(Ptr<const Packet> p, Ptr<NetDevice> nd) {
	Ptr<CCN_Interest> interest = CCN_Interest::deserializeFromPacket(p->Copy());

	interest->decreaseHopCounter();
	Ptr<Bloomfilter> outgoingLID = this->deviceToLid.find(nd)->second;
	interest->getBloomfilter()->OR(outgoingLID); //update BF in interest packet

	Ptr<PTuple> ptuple = thePIT->check(interest->getName());
	if (ptuple) {
		ptuple->getBF()->OR(interest->getBloomfilter());

		uint8_t new_ttl = ExperimentGlobals::D - interest->getInitialHopCounter();
		if (new_ttl > ptuple->getTTL()) {
			ptuple->setTTL(new_ttl);
		}

		return;
	}

	Ptr<TrieNode> tn = this->FIB->longestPrefixMatch(interest->getName());

	if(tn==0)
	{
		std::cout<<"null tn"<<std::endl;
	}

	if (tn->hasLocalApps()) {
		uint8_t new_ttl = ExperimentGlobals::D - interest->getInitialHopCounter();

		thePIT->addRecord(interest->getName(), interest->getBloomfilter(), new_ttl);

		Ptr<LocalApp> pubisher = tn->getLocalApps()->at(0);
		pubisher->deliverInterest(interest->getName());
		return;
	}

	NS_ASSERT_MSG(tn->hasDevices(),
			"router " + nodePtr->GetId() << "does not know how to forward " << interest->getName()->toString());

	if (interest->getHopCounter() == 0) { //must store in PIT
		uint8_t new_ttl = ExperimentGlobals::D - interest->getInitialHopCounter();

		Ptr<PTuple> ptuple = CreateObject<PTuple>(interest->getBloomfilter(),
				new_ttl);
		thePIT->update(interest->getName(), ptuple);

		//reset interest status
		interest->setTtl(ExperimentGlobals::D);
		interest->setinitialHopCounter(0);
		Ptr<Bloomfilter> newBF = CreateObject<Bloomfilter>(Bloomfilters::BF_LENGTH);
		interest->setBloomfilter(newBF);
	}

	Ptr<NetDevice> device = tn->getDevices()->at(0);
	Ptr<Packet> packet = interest->serializeToPacket();
	sendThroughDevice(packet, device);

}

void CcnModule::handleIncomingData(Ptr<const Packet> p, Ptr<NetDevice> nd) {
	Ptr<CCN_Data> data = CCN_Data::deserializeFromPacket(p->Copy());
	data->decreaseHopCounter();

	//always check PIT
	Ptr<PTuple> pt = this->thePIT->check(data->getName());

	//watch this carefully, we 'll discuss this closely on Tuesday
	if (pt != 0) { //interest has been tracked at PIT
		thePIT->erase(data->getName());

		//give data to any local app
		vector<Ptr<LocalApp> >& localApps = pt->getLocalApps();
		for (uint32_t i = 0; i < localApps.size(); i++) {
			localApps[i]->deliverData(data->getName(), data->getData(),
					data->getLength());
		}

		Ptr<Bloomfilter> tmpBF = pt->getBF();
		Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(tmpBF->getLength(),
				tmpBF->getBuffer());
		uint32_t ttl = pt->getTTL();

		if (data->getHopCounter() == 0) {
			//replace bf and ttl in packet
			data->setBloomfilter(bf);
			data->setTtl(ttl);
		} else {
			//THIS IS TRICKY,
			//Data packet does not terminate here, eithere send two packets OR
			//aggregate the two BFs and transmit once
			//Must see what causes less false positives
			data->getBloomfilter()->OR(bf);
			if (data->getHopCounter() < ttl) {
				data->setTtl(ttl);
			}
		}
	}

	if (data->getHopCounter() != 0) {
		bfForward(data->getBloomfilter(), data, nd);
	}
}

uint32_t CcnModule::bfForward(Ptr<Bloomfilter> bf, Ptr<CCN_Data> data,
		Ptr<NetDevice> excluded) {
	uint32_t fwded = 0;
	for (unsigned i = 0; i < this->nodePtr->GetNDevices(); i++) {
		Ptr<NetDevice> device = this->nodePtr->GetDevice(i);
		if (device == excluded) {
			continue;
		}

		Ptr<Bloomfilter> lid = deviceToLid[device];
		if (bf->contains(lid)) {
			fwded++;
			Ptr<Packet> p = data->serializeToPacket();
			Address address = addresses[device];
			bool sent = device->Send(p, address, 0x88DD);

			if (!sent) {
				std::cout << "bytes dropped" << std::endl;
				std::cout << "packets dropped" << std::endl;
			}
		}
	}
	return fwded;
}

void CcnModule::doSendInterest(Ptr<CCN_Name> name, Ptr<LocalApp> localApp) { //std::cout << "doSendInterest called" << std::endl;
	Ptr<PTuple> pt = thePIT->check(name);
	if (pt != 0) {
		bool added = pt->addLocalApp(localApp);
		if (!added) {
			cout << "local app has already requested " << name->toString()
					<< endl;
		}
		return;
	}

	Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(Bloomfilters::BF_LENGTH);
	//std::cout<<"na kai to filtro: "<<bf->toString()<<std::endl;
	uint32_t initTTL = decideTtl();
	Ptr<CCN_Interest> interest = CreateObject<CCN_Interest>(name, initTTL,
			initTTL, bf);
	Ptr<TrieNode> fibLookup = this->FIB->longestPrefixMatch(name);
	if (fibLookup == 0) {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "cannot forward Interests for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(fibLookup != 0, mesg);
	}

	pt = CreateObject<PTuple>(bf, 0);
	//std::cout<<"na kai to filtro: "<<bf->toString()<<std::endl;
	pt->addLocalApp(localApp);
	//std::cout<<"na kai to filtro: "<<pt->getBF()->toString()<<std::endl;
	thePIT->update(name, pt);

	if (fibLookup->hasLocalApps()) {
		Ptr<LocalApp> publisher = fibLookup->getLocalApps()->at(0);
		publisher->deliverInterest(name);
	} else if (fibLookup->hasDevices()) {
		Ptr<NetDevice> netdevice = fibLookup->getDevices()->at(0);
		Ptr<Packet> p = interest->serializeToPacket();

		/*if(netdevice==0)
		{
			std::cout << "device null" << std::endl;
		}

		for(uint32_t i=0;i<fibLookup->getDevices()->size();i++)
		{
			if(fibLookup->getDevices()->at(i)==0)
			{
				std::cout << "position: "<<i<<" is null "<< std::endl;
			}
			else
			{
				std::cout << "position: "<<i<<" is not null "<< std::endl;
			}
		}
*/
		sendThroughDevice(p, netdevice);
	} else {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "cannot forward Interests for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(fibLookup != 0, mesg);
		NS_ASSERT_MSG(false, mesg);
	}
}

int CcnModule::decideTtl() {
	if (switchh == 0) { //switchh 0 means using a random value
		/*ExperimentGlobals::RANDOM_VAR->SetAttribute ("Min", DoubleValue (1));
		ExperimentGlobals::RANDOM_VAR->SetAttribute ("Max", DoubleValue (ExperimentGlobals::D));*/
		//uint32_t d=ExperimentGlobals::RANDOM_VAR->GetInteger(1,ExperimentGlobals::D);

		uint32_t d=urv->GetInteger(1,ExperimentGlobals::D);

		return d;
	} else {
		return ExperimentGlobals::D;
	}
}

Ptr<Trie> CcnModule::getFIB() {
	return FIB;
}

Ptr<Node> CcnModule::getNode() {
	return nodePtr;
}

map<Ptr<NetDevice>, Ptr<CcnModule> > CcnModule::getNeighborModules() {
	return neighborModules;
}

Ptr<PIT> CcnModule::getPIT() {
	return thePIT;
}

void CcnModule::doSendData(Ptr<CCN_Name> name, uint8_t* buff, uint32_t bufflen) {

	Ptr<PTuple> ptuple = thePIT->check(name);
	if (ptuple == 0) {
		stringstream sstr;
		sstr << "router " << nodePtr->GetId() << "has no pit record for "
				<< name->toString();
		string mesg = sstr.str();
		NS_ASSERT_MSG(ptuple != 0, mesg);
	}

	uint32_t deliverd = 0;
	vector<Ptr<LocalApp> >::iterator iter;
	for (iter = ptuple->getLocalApps().begin();
			iter != ptuple->getLocalApps().end(); iter++) {
		deliverd++;
		(*iter)->deliverData(name, buff, bufflen);
	}

	Ptr<CCN_Data> data = CreateObject<CCN_Data>(ptuple->getBF(),
			ptuple->getTTL(), name, buff, bufflen);
	uint32_t fwded = bfForward(data->getBloomfilter(), data, 0);
	NS_ASSERT_MSG(fwded > 0 || deliverd > 0, "data went nowhere");

	free(buff);
}

void CcnModule::installLIDs() {
	for (uint32_t i = 0; i < this->nodePtr->GetNDevices(); i++) {
		Ptr<NetDevice> device = this->nodePtr->GetDevice(i);
		std::stringstream s;
		s << device->GetAddress();
		std::string result1 = md5(s.str());
		std::string result2 = sha1(s.str());

		uint32_t integer_result1 = (bitset<32>(
				stringtobinarystring(result1).substr(96))).to_ulong(); //we only keep the last 32 bits
		uint32_t integer_result2 = (bitset<32>(
				stringtobinarystring(result2).substr(96))).to_ulong(); //we only keep the last 32 bits

		Ptr<Bloomfilter> lid = CreateObject<Bloomfilter>(
				Bloomfilters::BF_LENGTH);

		//Double hashing scheme
		for (uint32_t j = 0; j < Bloomfilters::K_HASHES; j++) {
			uint32_t index = (integer_result1 + j * j * integer_result2)
					% (Bloomfilters::BF_LENGTH);
			lid->setBit(index);
		}
		deviceToLid[device] = lid;
	}

	/*std::cout<<"diagnostic for the maps-----------------------"<<std::endl;
	 for(unsigned i=0;i<this->n->GetNDevices();i++)
	 {
	 std::cout<<"device "<<i<<" gives "<<dtl->find(this->n->GetDevice(i))->second->getstring()<<std::endl;
	 std::cout<<"Bloom filter "<<i<<" gives device with address"<<ltd->find(dtl->find(this->n->GetDevice(i))->second)->second->GetAddress()<<std::endl;
	 }
	 std::cout<<"diagnostic for the maps-----------------------"<<std::endl;*/
}

string CcnModule::stringtobinarystring(std::string s) {
	string result = "";

	for (std::size_t i = 0; i < s.size(); ++i) {
		result = result + (bitset<8>(s.c_str()[i])).to_string();
	}

	return result;
}

/*
 * bool operator<(const Ptr<NetDevice>& f, const Ptr<NetDevice>& s) {
 std::stringstream stream;
 stream << f->GetAddress();

 std::stringstream stream2;
 stream2 << s->GetAddress();

 if (stream.str() < stream2.str()) {
 return true;
 } else if (stream.str() > stream2.str()) {
 return false;
 } else {
 return false;
 }
 }*/

bool operator<(const Ptr<NetDevice>& lhs, const Ptr<NetDevice>& rhs) {
	if(lhs==0)
	{
		std::cout<<"first was null"<<std::endl;
	}

	if(rhs==0)
	{
		std::cout<<"second was null"<<std::endl;
	}
	return lhs->GetAddress() < rhs->GetAddress();
}

uint32_t CcnModule::getNodeId()
{
	return nodePtr->GetId();
}

uint32_t CcnModule::getTXData()
{
	return p_RX_Data;
}

}
