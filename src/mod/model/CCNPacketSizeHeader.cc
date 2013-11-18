
#include <ns3/CCNPacketSizeHeader.h>


ns3::TypeId CCNPacketSizeHeader::GetTypeId(void)
{
	static ns3::TypeId t=ns3::TypeId("SIZE");
	t.SetParent<Header>();
	//t.AddConstructor<CCNPacketSizeHeader>();

	return t;
}

ns3::TypeId CCNPacketSizeHeader::GetInstanceTypeId(void) const
{
	return GetTypeId();
}

uint32_t CCNPacketSizeHeader::GetSerializedSize(void) const
{
	return 4;
}

void CCNPacketSizeHeader::Serialize(ns3::Buffer::Iterator start) const
{
	start.WriteHtonU32(data);
}

uint32_t CCNPacketSizeHeader::Deserialize(ns3::Buffer::Iterator start)
{
	data=start.ReadNtohU32();

	return 4;
}

void CCNPacketSizeHeader::Print(std::ostream &os) const
{
	os << "Header: " << data;
}

