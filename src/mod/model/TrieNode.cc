#include <string>
#include "ns3/TrieNode.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"


using namespace ns3;

		TrieNode::TrieNode(std::string* incoming_word,Ptr<Receivers> re)
		{
			end=false;
			children =new std::map<std::string,Ptr<TrieNode> >();
			this->word= *incoming_word;
			this->re=re;
		}

		TrieNode::~TrieNode()
		{
			children->clear();
		}

		std::map<std::string, Ptr<TrieNode> >* TrieNode::getChildren()
		{
			return children;
		}

		bool TrieNode::checkEnd()
		{
			return this->end;
		}

		void TrieNode::setEnd(bool end)
		{
			this->end=end;
		}









