#include <ns3/Receivers.h>

Receivers::Receivers(std::vector < ns3::Ptr<Object > > * receivers)
{
	this->receivers=receivers;
}

Receivers::~Receivers()
{
	//delete receivers;
}


