/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ns3/Initializer.h"

using namespace ns3;

		Initializer::Initializer(std::vector < Ptr < CcnModule > >* module,std::vector< std::vector < int > >* connection,int dataOwner)
		{
			this->module=*module;
			this->connection=connection;
			this->dataOwner=dataOwner;
		}

		Initializer::~Initializer()
		{
			//delete connection;
		}

		void Initializer::initializeFIBs()
		{
		//	std::cout<<"-----------------BFS-----------------"<<std::endl;
			unsigned i=this->dataOwner;
			/*for(unsigned i=0;i<this->module.size();i++)
			{*/
			//	std::cout<<"Starting from node "<<i<<"."<<std::endl;
				/*for(unsigned j=0;j<this->module.size();j++)
				{
					module.at(j)->visited=false;
				}*/

				std::queue < Ptr < CcnModule > >* queue=new std::queue < Ptr < CcnModule > >();
				queue->push(module.at(i));

				module.at(i)->visited=true;

				while(queue->size()!=0)
				{
					Ptr<CcnModule> handle=queue->front();
					queue->pop();

					Ptr<CcnModule> c=0;
					while((c=firstUnvisitedChild(handle))!=0)
					{
						c->visited=true;

						//std::cout<<"Updating FIB ,DATA size: "<<module.at(i)->DATA->size()<<std::endl;
						for(unsigned k=0;k<module.at(i)->DATA->size();k++)
						{
							std::vector < Ptr < Object > >* vec=new std::vector < Ptr < Object > >();
							vec->push_back(ndfinder(handle->n,c->n,handle->node,c->node));
							Ptr<Receivers> rec=CreateObject<Receivers>(vec);
						//	std::cout<<"Updating FIB of node "<<c->node<<"."<<std::endl;
							c->FIB->put(*(module.at(i)->DATA->at(k)),rec);
						}

						//-----------------------------------------------------------------
					/*	for(unsigned k=0;k<module.at(i)->DATA->size();k++)
						{
							std::cout<<"prefix "<<k<<": "<<std::endl;
							std::cout<<module.at(i)->DATA->at(k)->getValue()<<std::endl;
							Ptr<TrieNode> t=c->FIB->prefix(*(module.at(i)->DATA->at(k)));
							if(t==0)
							{
								std::cout<<"not ok"<<std::endl;
							}
							else
							{
								std::cout<<"ok"<<std::endl;
							}

							if(Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(t->re->receivers->at(0))))))
							{
								std::cout<<Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(t->re->receivers->at(0)))))->GetAddress()<<std::endl;
							}
						}
*/
						//-----------------------------------------------------------------

						queue->push(c);
					}
				}
		//	}
		//	std::cout<<"-----------------BFS-----------------"<<std::endl;
		}

		Ptr<NetDevice> Initializer::ndfinder(Ptr<Node> n1,Ptr<Node> n2,int i,int j)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
		{

			for(unsigned i=0;i<module.at(j)->n->GetNDevices();i++)
			{
				if(module.at(j)->n->GetDevice(i)->GetChannel()->GetDevice(0)->GetNode()==n1)
				{
					return module.at(j)->n->GetDevice(i);
				}

				if(module.at(j)->n->GetDevice(i)->GetChannel()->GetDevice(1)->GetNode()==n1)
				{
					return module.at(j)->n->GetDevice(i);
				}
			}

			return 0;
		}

		Ptr<CcnModule> Initializer::firstUnvisitedChild(Ptr<CcnModule> ccn)
		{
			for(unsigned i=0;i<ccn->map.size();i++)
			{
				if(!ccn->map.find(i)->second->visited)
				{
			//		std::cout<<"First unvisited child of node "<<ccn->node<<" is "<<ccn->map.find(i)->second->node<<std::endl;
					return ccn->map.find(i)->second;
				}
			}

			return 0;
		}
