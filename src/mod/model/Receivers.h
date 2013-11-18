
#ifndef RECEIVERS_H_
#define RECEIVERS_H_

#include <vector>
#include <ns3/core-module.h>

class Receivers : public ns3::Object
{
	public:
	Receivers(std::vector < ns3::Ptr < Object > > * receivers);

	~Receivers();

	std::vector < ns3::Ptr < Object > > * receivers;
};

#endif
