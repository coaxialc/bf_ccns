/*
 * TrieNode.h
 *
 *  Created on: Sep 17, 2013
 *      Author: Coaxial
 */

#ifndef TRIENODE_H_
#define TRIENODE_H_

#include <map>
#include <vector>
#include <string>
#include "ns3/core-module.h"
#include "ns3/net-device.h"
#include "ns3/local_app.h"
#include "CCN_Name.h"

using std::map;
using std::vector;
using std::string;

namespace ns3 {
class LocalApp;

class TrieNode: public Object {
public:
	TrieNode(Ptr<PtrString> incoming_word);
	~TrieNode();
	virtual void DoDispose(void);

	map<Ptr<PtrString>, Ptr<TrieNode> > getChildren(){ return children; }

	Ptr<TrieNode> setAndGetChildren(Ptr<PtrString>);
	Ptr<TrieNode> getChild(Ptr<PtrString>);
	bool hasDevices(){return devices != 0;}
	bool hasLocalApps(){ return localApps != 0;}
	bool hasData();

	vector<Ptr<LocalApp> >* getLocalApps(){return localApps;}
	vector<Ptr<NetDevice> >* getDevices(){return devices;}

	bool isLeaf() { return children.size() == 0; }
	bool addDevice(Ptr<NetDevice>);
	bool addLocalApp(Ptr<LocalApp>);

private:

	Ptr<PtrString> word;

	vector<Ptr<NetDevice> > *devices;
	vector<Ptr<LocalApp> > *localApps;
	map<Ptr<PtrString>, Ptr<TrieNode> > children;

};
}
#endif /* TRIENODE_H_ */
