
#include "ns3/Trie.h"
#include <vector>

using namespace ns3;

		Trie::Trie(Ptr<CcnModule> m)
		{
			r=CreateObject<TrieNode>(new std::string(),CreateObject<Receivers>(new std::vector< Ptr < Object > >()));
			this->m=m;
		}

		Trie::~Trie()
		{	
			r = 0;
			//this->m = 0;
		}

		void Trie::put(CCN_Name name,Ptr<Receivers> re)
		{
		//	std::cout<<"receivers size: "<<re->receivers->size()<<std::endl;
		    Ptr<TrieNode> handle=r;

			for(unsigned i=0;i<name.name.size();i++)
			{

				std::map<std::string,ns3::Ptr < TrieNode > >* m=handle->getChildren();
				std::string* w=name.name.at(i);

				if(m->count(*w)==0)
				{
					std::map<std::string,ns3::Ptr < TrieNode > >* m2=handle->getChildren();

					std::vector < ns3::Ptr<Object > > * receivers=new std::vector < ns3::Ptr < Object > > ();

					receivers->reserve(re->receivers->size());
					receivers->insert(receivers->end(),re->receivers->begin(),re->receivers->end());

					Ptr<Receivers> rec=CreateObject<Receivers>(receivers);
					std::pair < std::string,ns3::Ptr < TrieNode > > pa (*w,ns3::CreateObject<TrieNode>(w,rec));

					m2->insert(pa);
				}

				handle=(m->find(*w))->second;
			}

			handle->end=true;
		}

		Ptr<TrieNode> Trie::prefix(CCN_Name name)
		{
			Ptr<TrieNode> handle=r;

			for(unsigned i=0;i<name.name.size()&&handle!=0;i++)
			{
				std::string* c=name.name.at(i);

				std::map<std::string,ns3::Ptr < TrieNode > >* m=handle->getChildren();

				if(m==0||m->find(*c)==m->end())
				{
					return handle;
				}
				else
				{
					handle=m->find(*c)->second;
				}
			}

			return handle;
		}
