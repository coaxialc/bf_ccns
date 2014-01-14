#ifndef PIT_H_
#define PIT_H_

#include <map>
#include "CCN_Name.h"
#include "PTuple.h"
#include "local_app.h"

using std::map;

namespace ns3 {
class PTuple;
class Bloomfilter;

class PIT: public Object {
public:

	PIT();
	~PIT();
	virtual void DoDispose(void);

	void update(Ptr<CCN_Name> name, Ptr<PTuple>);
	Ptr<PTuple> check(Ptr<CCN_Name> name);
	void erase(Ptr<CCN_Name> name);
	uint32_t getSize();
	bool addRecord(Ptr<CCN_Name>, Ptr<Bloomfilter>,  uint32_t);

private:
	map<Ptr<CCN_Name>, Ptr<PTuple> > table;
};
}

#endif
