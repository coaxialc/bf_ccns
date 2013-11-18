/*
 * Trie.h
 *
 *  Created on: Sep 17, 2013
 *      Author: Coaxial
 */

#ifndef TRIE_H_
#define TRIE_H_

#include "ns3/TrieNode.h"
#include "ns3/CcnModule.h"
#include "ns3/Receivers.h"


	class Trie  : public ns3::Object
	{
		public:
		Trie(ns3::Ptr<CcnModule> m);

		~Trie();
		ns3::Ptr<TrieNode> r;
		ns3::Ptr<CcnModule> m;

		void put(CCN_Name name,ns3::Ptr<Receivers> re);
		ns3::Ptr<TrieNode> prefix(CCN_Name name);

		std::vector < ns3::Ptr < TrieNode > > leaves;



	};




#endif /* TRIE_H_ */
