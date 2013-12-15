/*
 * Receiver.h
 *
 *  Created on: Oct 21, 2013
 *      Author: Coaxial
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_


#include "ns3/CcnModule.h"
#include "ns3/CCN_Name.h"
#include <string>
#include <vector>


class CcnModule;

class Receiver : public ns3::Application
{

public:
	ns3::Ptr<CcnModule> ccnm;


	static ns3::TypeId GetTypeId(void);
	virtual ns3::TypeId GetInstanceTypeId (void) const;

	Receiver(ns3::Ptr<CcnModule> ccnm);
	~Receiver();
	char* data;
	int length;
	ns3::Ptr<CCN_Name> dataName;
	void SendInterest(ns3::Ptr<CCN_Name> n,int num);
	int askedfor;
	int returned;
	//std::vector < ns3::Ptr<CCN_Name> >* d;

	virtual void DataArrived(ns3::Ptr<CCN_Name> data, char* buff, int bufflen);
};

#endif
