#include <map>
#include "PIT.h"

using std::pair;

namespace ns3 {

PIT::PIT() {
	table = map<Ptr<CCN_Name>, Ptr<PTuple> >();
}

PIT::~PIT() {
	table.clear();
}

void PIT::DoDispose(void) {
	table.clear();
}

void PIT::update(Ptr<CCN_Name> name, Ptr<PTuple> re) {
	table[name] = re;
}

void PIT::erase(Ptr<CCN_Name> name) {
	table.erase(name);
}

Ptr<PTuple> PIT::check(Ptr<CCN_Name> name) {
	map<Ptr<CCN_Name>, Ptr<PTuple> >::iterator find = table.find(name);
	if (find != table.end()){
		return find->second;
	}else{
		return 0;
	}
}

/*
map<Ptr<CCN_Name>, Ptr<PTuple> > PIT::getTable() {
	return table;
}
*/

uint32_t PIT::getSize() {
	return table.size();
}

bool PIT::addRecord(Ptr<CCN_Name> name, Ptr<Bloomfilter> f, uint32_t ttl){
	Ptr<PTuple> tuple = check(name);
	if (tuple == 0){
		tuple = CreateObject<PTuple>(f, ttl);
		update(name, tuple);
		return true;
	}else{
		tuple->getBF()->OR(f);
		if (ttl > tuple->getTTL()){
			tuple->setTTL(ttl);
		}
		return false;
	}
}

}
