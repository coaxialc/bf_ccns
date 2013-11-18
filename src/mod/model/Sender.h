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

class CcnModule;

class Sender : public ns3::Application
{

public:
	ns3::Ptr<CcnModule> ccnm;


	static ns3::TypeId GetTypeId(void);
	virtual ns3::TypeId GetInstanceTypeId (void) const;
	~Sender();
	Sender(ns3::Ptr<CcnModule> ccnm,int waitingTime);
	//std::vector<char*> data;
	//std::vector<int> length;
	int waitingTime;
	std::map < ns3::Ptr < CCN_Name > , char*> data;
	std::map < ns3::Ptr < CCN_Name > , int> length;


	void SendData(ns3::Ptr<CCN_Name> data, char* buff, int bufflen);
	virtual void InterestReceived(ns3::Ptr<CCN_Name> ccnn);


	void AnnounceName(ns3::Ptr<CCN_Name> n);
};

#endif
