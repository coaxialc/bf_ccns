
#ifndef CCNMODULE_H
#define CCNMODULE_H

#include "ns3/Text.h"
#include "ns3/Sender.h"
#include "ns3/Receiver.h"
#include "ns3/Trie.h"
#include "ns3/Bloomfilter.h"
#include "ns3/md5.h"
#include "ns3/sha1.h"

class Trie;
class Sender;
class Receiver;
class Bloomfilter;

	class CcnModule  : public ns3::Object
	{
		public:

		static int interestCount;
		static int dataCount;
		int data;

		CcnModule(int length,int d);

		~CcnModule();

		int d;

		bool visited;

		int node;

		int length;

		ns3::Ptr<Text> text;

		ns3::Ptr<Trie> FIB;

		long size(ns3::Ptr<ns3::Packet> p);

		std::vector < ns3::Ptr<CCN_Name> >* DATA;

		void reInit();

		ns3::Ptr<PIT> p_i_t;

		ns3::Ptr<ns3::Node> n;

		std::map < int, ns3::Ptr < CcnModule > > map;

		void sendInterest(ns3::Ptr<CCN_Name> name, ns3::Ptr<Receiver> ba);

		std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > >* ltd;
		std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >* dtl;

		void sendData(ns3::Ptr<CCN_Name>, char *buff, int bufflen);

		/*optional*/
		void announceName(ns3::Ptr<CCN_Name> name, ns3::Ptr<Sender> app);

		void setNode(ns3::Ptr<ns3::Node>);

		void takeCareOfHashes();

		void send(ns3::Ptr<ns3::Packet> p,ns3::Ptr<ns3::NetDevice> nd);

		bool receiveabc(ns3::Ptr<ns3::NetDevice> nd,ns3::Ptr<const ns3::Packet> p,uint16_t a,const ns3::Address& ad);
	};

#endif

