#ifndef INITIALIZER_H_
#define INITIALIZER_H_

#include "ns3/core-module.h"
#include "ns3/CcnModule.h"
#include "ns3/Parser.h"
#include <ns3/net-device.h>

using std::map;
using std::vector;



namespace ns3
{
	class CcnModule;

	class Initializer  : public Object
	{
		public:
		Initializer(std::vector < Ptr < CcnModule > >* ,Ptr<Parser> ,int ,uint32_t);

		~Initializer();

		void initializeFIBs();
		int dataOwner;
		uint32_t dataNum;
		Ptr<CcnModule> firstUnvisitedChild(Ptr<CcnModule> );
		vector < Ptr < CcnModule > > module;
		Ptr<Parser> parser;
		map<Ptr<CcnModule>, bool > visited;
		Ptr<NetDevice> ndfinder(Ptr<Node> n1,Ptr<Node> n2,int i,int j);
	};
}




#endif
