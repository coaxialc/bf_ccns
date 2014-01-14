#include "TrieNode.h"

namespace ns3 {

TrieNode::TrieNode(Ptr<PtrString> incoming_word) {
	children = map<Ptr<PtrString>, Ptr<TrieNode> >();
	this->word = incoming_word;

	devices = 0;
	localApps = 0;
}

TrieNode::~TrieNode() {
	children.clear();
	if (devices){
		devices->clear();
	}

	if (localApps){
		localApps->clear();
	}
}

void TrieNode::DoDispose(void){
	children.clear();
	if (devices){
		devices->clear();
	}

	if (localApps){
		localApps->clear();
	}
}

bool TrieNode::addDevice(Ptr<NetDevice> d){
	if (!devices){
		devices = new vector<Ptr<NetDevice> >();
	}

	vector<Ptr<NetDevice> >::iterator iter;
	for(iter=devices->begin(); iter!=devices->end(); iter++){
		if ( *iter == d){
			return false;
		}
	}

	devices->push_back(d);
	return true;
}

bool TrieNode::addLocalApp(Ptr<LocalApp> app){
	if (!localApps){
		localApps = new vector<Ptr<LocalApp> >();
	}

	vector<Ptr<LocalApp> >::iterator iter;
	for(iter=localApps->begin(); iter!=localApps->end(); iter++){
		if ( *iter == app){
			return false;
		}
	}

	localApps->push_back(app);
	return true;
}

Ptr<TrieNode> TrieNode::setAndGetChildren(Ptr<PtrString> word){
	Ptr<TrieNode> retValue = 0;
	map<Ptr<PtrString>, Ptr<TrieNode> >::iterator find = children.find(word);
	if (find != children.end()){
		retValue = find->second;
	}else{
		Ptr<TrieNode> newTrieNode = CreateObject<TrieNode>(word);
		children[word] = newTrieNode;
		retValue = newTrieNode;
	}

	return retValue;
}

Ptr<TrieNode> TrieNode::getChild(Ptr<PtrString> word){
	map<Ptr<PtrString>, Ptr<TrieNode> >::iterator find = children.find(word);
	if (find == children.end()){
		return 0;
	}else{
		return find->second;
	}
}

bool TrieNode::hasData(){
	return (devices != 0 && devices->size() > 0)
			|| (localApps != 0 && localApps->size() > 0);
}

}
