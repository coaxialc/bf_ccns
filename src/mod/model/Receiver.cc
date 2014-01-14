#include "Receiver.h"
#include <vector>
#include <string>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include "boost/lexical_cast.hpp"

using std::stringstream;

namespace ns3 {

Receiver::Receiver(Ptr<CcnModule> ccnmIn) {
	askedfor = 0;
	asked = set<Ptr<CCN_Name> >();

	returned = 0;
	ccnm = ccnmIn;
	localApp = CreateObject<LocalApp>();

	Callback<void, Ptr<CCN_Name> > interestCb = MakeCallback(&Receiver::handleInterest, this);
	localApp->setInterestCallback(interestCb);

	Callback<void, Ptr<CCN_Name>, uint8_t*, uint32_t> dataCb = MakeCallback(&Receiver::handleData, this);
	localApp->setDataCallback(dataCb);
}

Receiver::~Receiver() {
	ccnm = 0;
	localApp = 0;
	dataName = 0;
	asked.clear();
}

void Receiver::DoDispose(void){
	ccnm = 0;
	localApp = 0;
	dataName = 0;
	asked.clear();
}

TypeId Receiver::GetTypeId(void) {
	static TypeId t = TypeId("RECEIVER");
	t.SetParent<Object>();
	//t.AddConstructor<CCNPacketSizeHeader>();

	return t;
}

TypeId Receiver::GetInstanceTypeId(void) const {
	return GetTypeId();
}

uint32_t Receiver::getReturned()
{
	return returned;
}

uint32_t Receiver::getAskedFor()
{
	return askedfor;
}

Ptr<CcnModule> Receiver::getModule()
{
	return ccnm;
}


void Receiver::handleInterest(Ptr<CCN_Name>){
	NS_ASSERT_MSG(false, "Receiver should not receive any Interests");
}

void Receiver::handleData(Ptr<CCN_Name> name, uint8_t*, uint32_t){
	if (asked.find(name) == asked.end()){
		NS_ASSERT_MSG(false, "Got a Data for interest not asked " << name->toString());
	}else{
		returned++;
	}
}

void Receiver::SendInterest(Ptr<CCN_Name> name, uint32_t num) {

	for (uint32_t i=1; i<=num; i++){
		Time millis = MilliSeconds(i);
		stringstream sstream;
		sstream << num;
		string numstr = sstream.str();

		vector<string> tokens = name->getTokens();
		tokens.push_back(numstr);
		Ptr<CCN_Name> theName = CreateObject<CCN_Name>(tokens);

		Simulator::Schedule(millis, &Receiver::doSendInterest, this, theName);
	}
}

void Receiver::doSendInterest(Ptr<CCN_Name> name){
	askedfor++;
	asked.insert(name);
	this->ccnm->sendInterest(name, localApp);
}

}
