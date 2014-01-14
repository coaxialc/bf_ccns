/*
 * PTuple.h
 *
 *  Created on: Nov 26, 2013
 *      Author: Coaxial
 */

#ifndef PTUPLE_H_
#define PTUPLE_H_

#include <vector>
#include "ns3/core-module.h"
#include "ns3/Bloomfilter.h"
#include "ns3/local_app.h"

using std::vector;

namespace ns3 {

class LocalApp;
class Bloomfilter;

class PTuple : public Object {
public:
	PTuple(Ptr<Bloomfilter> bf, int ttl);
	PTuple(Ptr<Bloomfilter> bf, int ttl, vector<Ptr<LocalApp> >*);
	virtual ~PTuple();
	virtual void DoDispose(void);

	bool addLocalApp(Ptr<LocalApp> app);
	bool removeLocalApp(Ptr<LocalApp>);

	Ptr<Bloomfilter> getBF(){ return bf; }
	uint32_t getTTL(){ return ttl;}
	void setTTL(uint32_t _ttl){ ttl = _ttl;}
	vector<Ptr<LocalApp> >& getLocalApps() {return r;}

private:
	Ptr<Bloomfilter> bf;
	uint32_t ttl;
	vector<Ptr<LocalApp> > r;

	vector<Ptr<LocalApp> >::iterator find(Ptr<LocalApp>);
};

}
#endif /* PTUPLE_H_ */
