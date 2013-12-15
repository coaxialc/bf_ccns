/*
 * Sender.h
 *
 *  Created on: Oct 21, 2013
 *      Author: Coaxial
 */

#ifndef SENDER_H_
#define SENDER_H_


#include "ns3/CcnModule.h"
#include "ns3/CCN_Name.h"
#include <string>
#include "ns3/Bloomfilter.h"

class CcnModule;
class Bloomfilter;

class Sender : public ns3::Application
{

public:
	ns3::Ptr<CcnModule> ccnm;


	static ns3::TypeId GetTypeId(void);
	virtual ns3::TypeId GetInstanceTypeId (void) const;
	~Sender();
	Sender(ns3::Ptr<CcnModule> ccnm,int waitingTime);
	//void SendData2(ns3::Ptr<CCN_Name> data, char* buff, int bufflen,ns3::Ptr<Bloomfilter> bf,std::string hopc);
	// void InterestReceived2(ns3::Ptr<CCN_Name> ccnn,ns3::Ptr<Bloomfilter> bf,std::string hopc);
	//std::vector<char*> data;
	//std::vector<int> length;
	int waitingTime;
	int interests;
	std::map < ns3::Ptr < CCN_Name > ,char*> data;
	std::map < ns3::Ptr < CCN_Name > , int> length;
	void MapTest(int l);

	void SendData(ns3::Ptr<CCN_Name> data,char* buff, int bufflen);
	virtual void InterestReceived(ns3::Ptr<CCN_Name> ccnn);


	void AnnounceName(ns3::Ptr<CCN_Name> n);
};

#endif
