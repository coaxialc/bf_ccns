#ifndef PIT_H_
#define PIT_H_


#include <map>
#include "ns3/CCN_Name.h"
#include "PTuple.h"
class PTuple;
using namespace std;

class Bloomfilter;

class PIT : public ns3::Object
{
    public:
	std::map < ns3::Ptr<CCN_Name> ,ns3::Ptr < PTuple > >* p;
	PIT();
	~PIT();
	void update(ns3::Ptr<CCN_Name> name,ns3::Ptr < PTuple >);
	ns3::Ptr<PTuple> check(ns3::Ptr<CCN_Name> name);
	void erase(ns3::Ptr<CCN_Name> name);



};



#endif
