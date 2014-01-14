#include "PTuple.h"

namespace ns3 {

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn){
	bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());
	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
}

PTuple::PTuple(Ptr<Bloomfilter> bf, int ttlIn, vector<Ptr<LocalApp> >* apps) {
	bf = CreateObject<Bloomfilter>(bf->getLength(), bf->getBuffer());
	ttl = ttlIn;
	r = vector<Ptr<LocalApp> >();
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=apps->begin(); iter!=apps->end(); iter++){
		r.push_back(*iter);
	}
}

PTuple::~PTuple() {
	bf = 0;
}

void PTuple::DoDispose(void) {
	bf = 0;
}

bool PTuple::addLocalApp(Ptr<LocalApp> app){
	bool added = false;
	if (find(app) == r.end()){
		r.push_back(app);
		added = true;
	}
	return added;
}

bool PTuple::removeLocalApp(Ptr<LocalApp> app){
	bool found = false;
	vector<Ptr<LocalApp> >::iterator iter = find(app);
	if(iter != r.end()){
		r.erase(iter);
		found = true;
	}

	return found;
}

vector<Ptr<LocalApp> >::iterator PTuple::find(Ptr<LocalApp> app){
	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=r.begin(); iter!=r.end(); iter++){
		if (*iter == app){
			break;
		}
	}
	return iter;
}

}
