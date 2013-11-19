#ifndef PIT_H_
#define PIT_H_


#include <map>
#include "ns3/Bloomfilter.h"
#include "ns3/CCN_Name.h"

using namespace std;

class PIT : public ns3::Object
{
    public:
	std::map < ns3::Ptr<CCN_Name> ,ns3::Ptr < Bloomfilter > >* p;
	PIT();
	~PIT();
	void update(ns3::Ptr<CCN_Name> name,ns3::Ptr < Bloomfilter >);
	ns3::Ptr<Bloomfilter> check(ns3::Ptr<CCN_Name> name);
	void erase(ns3::Ptr<CCN_Name> name);



};



#endif
