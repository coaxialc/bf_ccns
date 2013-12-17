
#ifndef CCNMODULE_H
#define CCNMODULE_H

#include "ns3/Text.h"
#include "ns3/Sender.h"
#include "ns3/Receiver.h"
#include "ns3/Trie.h"
#include "ns3/Bloomfilter.h"
#include "ns3/PIT.h"
#include "ns3/md5.h"
#include "ns3/sha1.h"
#include <bitset>

class Trie;
class Sender;
class Receiver;
class Bloomfilter;
class PIT;

	class CcnModule  : public ns3::Object
	{
		public:

		static int interestCount;
		static int dataCount;
		int data;

		CcnModule(int length,int d,int switchh,ns3::Ptr<ns3::UniformRandomVariable> rv);

		~CcnModule();

		int d;

		bool visited;

		ns3::Ptr<ns3::UniformRandomVariable> rv;

		int switchh;

		int node;

		int length;

		ns3::Ptr<Text> text;

		ns3::Ptr<Trie> FIB;

		long size(ns3::Ptr<ns3::Packet> p);

		std::vector < ns3::Ptr<CCN_Name> >* DATA;

		void reInit();

		std::string stringtobinarystring(std::string s);

		//std::bitset<128> stringtobitset1(std::string s);

		//std::bitset<160> stringtobitset2(std::string s);

		ns3::Ptr<PIT> p_i_t;

		ns3::Ptr<ns3::Node> n;

		ns3::Ptr<Sender> s;
		ns3::Ptr<Receiver> r;

		std::map < int, ns3::Ptr < CcnModule > > map;

		ns3::Ptr<Bloomfilter> add(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s);

		bool equals(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s);

		void sendThroughDevice(ns3::Ptr<ns3::Packet> p,ns3::Ptr<ns3::NetDevice> nd);

		void sendInterest(ns3::Ptr<CCN_Name> name,int h,ns3::Ptr < Bloomfilter > bf,ns3::Ptr < ns3::NetDevice > nd);

	//	std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > >* ltd;
		std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >* dtl;

		void sendData(ns3::Ptr<CCN_Name>,char *buff, int bufflen,ns3::Ptr < Bloomfilter > bf,int ttl,ns3::Ptr<ns3::NetDevice> excluded);

		/*optional*/
		void announceName(ns3::Ptr<CCN_Name> name, ns3::Ptr<Sender> app);

		ns3::Ptr<Bloomfilter> orbf(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s);

		void setNode(ns3::Ptr<ns3::Node>);

		void takeCareOfHashes();

		//void send(ns3::Ptr<ns3::Packet> p,ns3::Ptr<Bloomfilter> bf,ns3::Ptr<ns3::NetDevice> excluded,std::string calledby);

		bool receiveabc(ns3::Ptr<ns3::NetDevice> nd,ns3::Ptr<const ns3::Packet> p,uint16_t a,const ns3::Address& ad);
	};

#endif

