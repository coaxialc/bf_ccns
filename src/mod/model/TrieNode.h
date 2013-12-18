/*
 * TrieNode.h
 *
 *  Created on: Sep 17, 2013
 *      Author: Coaxial
 */

#ifndef TRIENODE_H_
#define TRIENODE_H_

#include "ns3/core-module.h"
#include "ns3/Receivers.h"

	class TrieNode  : public ns3::Object
	{
		public:
		TrieNode(std::string* incoming_word, ns3::Ptr<Receivers> re);
		~TrieNode();

		std::map<std::string,ns3::Ptr<TrieNode> >* getChildren();
		bool checkEnd();

		void setEnd(bool end);
		std::string word;
		bool end;
		ns3::Ptr<Receivers> re;

		private:
		std::map<std::string,ns3::Ptr<TrieNode> >* children;


	};

#endif /* TRIENODE_H_ */
