#include "ns3/Initializer.h"
#include <sstream>
#include <vector>

#include "ns3/CcnModule.h"

using std::stringstream;
using std::vector;
using std::queue;

namespace ns3
{

class CcnModule;

		Initializer::Initializer(vector < Ptr < CcnModule > >* module,Ptr<Parser> parser,int dataOwner,uint32_t dataNum,map <uint32_t , uint32_t> nodeToModule)
		{
			this->module=*module;
			this->parser=parser;
			this->dataOwner=dataOwner;
			this->dataNum=dataNum;
			this->nodeToModule=nodeToModule;

			for(uint32_t i=0;i<module->size();i++)
			{
				visited[module->at(i)]=false;
			}
		}

		Initializer::~Initializer()
		{
			//delete connection;
		}

		void Initializer::doesEveryModuleHaveANode()
		{
			for(uint32_t i=0;i<module.size();i++)
			{
				if(module.at(i)->getNode()==0)
				{
					std::cout<<"Module "<<i<<" does not have a node."<<std::endl;
				}
			}
		}

		void Initializer::initializeFIBs()
		{
		//	std::cout<<"-------------BFS---------------"<<std::endl;
			unsigned i=this->dataOwner;
			queue < Ptr < CcnModule > >* q=new queue < Ptr < CcnModule > >();
			q->push(module.at(i));

			visited.find(module.at(i))->second=true;

			while(q->size()!=0)
			{

				Ptr<CcnModule> handle=q->front();
				q->pop();

				Ptr<CcnModule> c=0;
				while((c=firstUnvisitedChild(handle))!=0)
				{
			//		std::cout<<"Handle: "<<handle->getNode()->GetId()<<" \n First unvisited child: "<<c->getNode()->GetId()<<std::endl;
					visited.find(c)->second=true;

					for(uint32_t k=1;k<=this->dataNum;k++)
					{
						vector <string>* nameVector=new vector<string>();
						nameVector->push_back("domain1");
						nameVector->push_back("domain2");
						nameVector->push_back("domain3");

						stringstream sstream;
						sstream << k;

						nameVector->push_back(sstream.str());
						Ptr<CCN_Name> name=CreateObject<CCN_Name>(*nameVector);

				//		std::cout<<"Initializer - Module "<<c->getNode()->GetId()<<":"<<"To reach name "<<name->toString()<<" go through "<<ndfinder(handle->getNode(),c->getNode())->GetAddress()<<std::endl;
						c->getFIB()->put(name,ndfinder(handle->getNode(),c->getNode()));
					}
					q->push(c);
				}
			}
		//	std::cout<<"-------------BFS---------------"<<std::endl;
		}

		Ptr<CcnModule> Initializer::firstUnvisitedChild(Ptr<CcnModule> ccn)
		{
			for(unsigned i=0;i<ccn->getNode()->GetNDevices();i++)
			{
				if(!ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second->getNode()==0)//checking if its a real module (explained in BootstrappingHelper.cc)
				{
					if(!(this->visited.find(ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second)->second))
					{
						return ccn->getNeighborModules().find(ccn->getNode()->GetDevice(i))->second;
					}
				}
			}

			return 0;
		}


		Ptr<NetDevice> Initializer::ndfinder(Ptr<Node> n1,Ptr<Node> n2)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
		{
		//	std::cout<<"ndfinder: "<<n1->GetId()<<" "<<n2->GetId()<<std::endl;

			if(n1==0) {std::cout<<"In ndfinder in initializer n1 is null"<<std::endl;}

			if(n2==0) {std::cout<<"In ndfinder in initializer n2 is null"<<std::endl;}

			for(unsigned i=0;i<n2->GetNDevices();i++)
			{
					if(n2->GetDevice(i)->GetChannel()->GetDevice(0)->GetNode()==n1)
					{
							return n2->GetDevice(i);
					}

					if(n2->GetDevice(i)->GetChannel()->GetDevice(1)->GetNode()==n1)
					{
							return n2->GetDevice(i);
					}
			}

			return 0;
		}
}
