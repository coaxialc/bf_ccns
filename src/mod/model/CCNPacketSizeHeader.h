/*
 * CCNPacketSizeHeader.h
 *
 *  Created on: Sep 29, 2013
 *      Author: Coaxial
 */

#ifndef CCNPACKETSIZEHEADER_H_
#define CCNPACKETSIZEHEADER_H_


#include <ns3/header.h>

class CCNPacketSizeHeader : public ns3::Header
{
	public:
	static ns3::TypeId GetTypeId(void);
	virtual ns3::TypeId GetInstanceTypeId (void) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize (ns3::Buffer::Iterator start) const;
	virtual uint32_t Deserialize (ns3::Buffer::Iterator start);
	virtual void Print (std::ostream &os) const;
	uint32_t data;
};


#endif /* CCNPACKETSIZEHEADER_H_ */
