
#ifndef BLOOMFILTER_H_
#define BLOOMFILTER_H_

#include "ns3/mod-module.h"

using namespace std;

class Bloomfilter : public ns3::Object
{
    public:
	std::string getstring();
	Bloomfilter(int length,bool bits []);
	Bloomfilter(int length,std::string bits);
	Bloomfilter(int length);
	~Bloomfilter();

	bool * filter;
	int length;
};

#endif
