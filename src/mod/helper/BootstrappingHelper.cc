#include "BootstrappingHelper.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <boost/graph/graphviz.hpp>

using std::ofstream;
using std::stringstream;
using std::vector;
using std::string;

using std::endl;

using namespace ns3;

BootstrappingHelper::BootstrappingHelper(string filename,int gsize,int length,int d,int switchh)
{
	std::cout<<"Constructing BootstrappingHelper"<<std::endl;


	this->filename=filename;
	vec3=new std::vector< Ptr < Object > >();

	this->d=d;
	this->length=length;
	this->gs=gsize;
//	r=CreateObject<UniformRandomVariable>();
	this->switchh=switchh;
	nodeToModule=map<uint32_t, uint32_t >();//to find module index in the vector that is connected to specific node (by its id)
}

BootstrappingHelper::~BootstrappingHelper()
{
	std::cout<<"Destructing BootstrappingHelper"<<std::endl;
	delete vec3;
	p=0;
}

void BootstrappingHelper::parseTopology()
{
	//std::cout<<"Parsing topology"<<std::endl;

	p=CreateObject<Parser>();

	p->parse(filename);

	module=new vector < Ptr < CcnModule > >();

	/*map<uint32_t, uint32_t >::iterator it;
	uint32_t count=0;
	for(it=p->nodeToId.begin();it!=p->nodeToId.end();it++)
	{
		count++;
	}

	std::cout<<"nodeToId size "<<count<<std::endl;*/

	map<uint32_t, Ptr<Node> >::iterator iter;

	for (iter=p->idToNode.begin(); iter!=p->idToNode.end(); iter++ ){//iterating using iterator ,not size and counters ,because in the middle there are numbers we do not want to use
		Ptr<CcnModule> m = CreateObject<CcnModule>(iter->second,this->switchh);
		nodeToModule[iter->first]=module->size();//if there are x already inside ,then the next will be the (x+1)th ,with index x
		module->push_back(m);
	}

	for(uint32_t i=0;i<module->size();i++)
	{
	//	std::cout<<"Node "<<i<<" has ns3 id "<<module->at(i)->getNode()->GetId()<<std::endl;
	}

	for(unsigned i=0;i<module->size();i++)//here we use the size and counter because we want to refer to vector indexes (module's)
	{
		module->at(i)->installLIDs();
	}

	map<uint32_t, Ptr<Node> >::iterator iter2;

	//here we set the neighbors of every CcnModule
	for (iter2=p->idToNode.begin(); iter2!=p->idToNode.end(); iter2++ )//here we want an iterator again ,because we must not try to refer to nodes that do not exist
	{
		if(iter2->first!=0)
		{
			for(unsigned j=0;j<p->getNeighbors(iter2->first).size();j++)//for every neighbor
				{
					Ptr<Node> n1=p->getNeighbors(iter2->first).at(j);//this is the neighbor

					uint32_t u1=p->nodeToId[n1->GetId()];//neighbors id (the id we use ,not the system's)

					Ptr<CcnModule> ccn_module=module->at(nodeToModule.find(u1)->second);//neighbor's module

					Ptr<Node> n2=iter2->second;//this node

					uint32_t u2=iter2->first;//this node's id

					Ptr<NetDevice> net_device = ndfinder(n1,n2);

					if(net_device==0)
					{
						std::cout<<"net_device about to be inserted in the map is null"<<std::endl;
					}

					if(ccn_module==0)
					{
						std::cout<<"ccn_module about to be inserted in the map is null"<<std::endl;
					}

					//std::cout<<"Setting neighbor of module "<<nodeToModule.find(u2)->second<<" to be "<<nodeToModule.find(u1)->second<<std::endl;
					module->at(nodeToModule.find(u2)->second)->setNeighbor(net_device, ccn_module);
				}
		}
	}

	std::cout<<"Module vector has size: "<<module->size()<<std::endl;
	std::cout<<"According to the maps:"<<std::endl;
	for(uint32_t i=0;i<module->size();i++)
	{
		for(uint32_t j=0;j<module->at(i)->getNode()->GetNDevices();j++)//yparxoune netdevices poy den einai syndedemenes poyhtena ,etsi fainetai apo edo ,opote prepei na prosexoume na min asxoloumaste mazi tous
		{
			if(module->at(i)->getNeighborModules().find(module->at(i)->getNode()->GetDevice(j))->second->getNode()!=0)
			{
			//	std::cout<<"Node "<<module->at(i)->getNode()->GetId()<<" is connected through device "<<module->at(i)->getNode()->GetDevice(j)->GetAddress()<<" to node "<<module->at(i)->getNeighborModules().find(module->at(i)->getNode()->GetDevice(j))->second->getNode()->GetId()<<std::endl;
			}

		}
	}
}

Ptr<NetDevice> BootstrappingHelper::ndfinder(Ptr<Node> n1,Ptr<Node> n2)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
{
	//std::cout<<"Bootstrapping ndfinder"<<std::endl;
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

	std::cout<<"Bootstrapping helper ndfinder returning 0"<<std::endl;
	return 0;
}

void BootstrappingHelper::startExperiment()
{
	std::cout<<"startExperiment"<<std::endl;

	/*for(unsigned i=0;i<p->kombos->size();i++)
	{
		std::cout<<"O kombos "<<i<<" exei "<<p->kombos->at(i)->GetNDevices()<<" devices."<<std::endl;
		for(unsigned j=0;j<p->kombos->at(i)->GetNDevices();j++)
		{
			std::cout<<"Device "<<j<<":"<<std::endl;
			std::cout<<"Address: "<<p->kombos->at(i)->GetDevice(j)->GetAddress()<<std::endl;
		}
	}*/

	std::vector <vertex> v1;
	//std::vector <vertex> v0;
	//std::set <uint32_t> unwanted;

	graph topology=p->getGraph();



	in = boost::get(boost::vertex_index, topology);

	typedef boost::graph_traits<graph>::vertex_iterator vi;

	std::pair<vi, vi> v;

	uint32_t boostNodeCounter=0;
	//uint32_t degree0=0;

	for(v=boost::vertices(topology);v.first!=v.second;++v.first)
	{
		boostNodeCounter++;
		if(boost::degree((*v.first),topology)==1)
		{
			v1.push_back((*v.first));
		}

		/*if(boost::degree((*v.first),topology)==0)
		{
		//	degree0++;
		//	std::cout<<"degree 0: "<<in[(*v.first)]<<std::endl;
			v0.push_back((*v.first));
			//unwanted.insert(in[*v.first]);
		}*/
	}

	//std::cout<<"degree 0 exoun : "<<degree0<<std::endl;

	/*std::cout<<"Deleting nodes with degree 0..."<<std::endl;
	for(uint32_t i=0;i<v0.size();i++)
	{
		boost::remove_vertex(v0.at(i),topology);
	}*/

	write_graphviz(std::cout,topology);

	/*std::pair<vi, vi> v2;
	uint32_t boostNodeCounter2=0;
	for(v2=boost::vertices(topology);v2.first!=v2.second;++v2.first)
	{
		boostNodeCounter2++;
	}
*/
	in = boost::get(boost::vertex_index, topology);

	std::cout<<"Plithos kombon: "<<boostNodeCounter<<std::endl;
	//std::cout<<"Plithos kombon meta ti diagrafi: "<<boostNodeCounter2<<std::endl;

	std::cout<<"degree 1 exoun : "<<v1.size()<<std::endl;

	if(this->gs>v1.size()-1)
	{
		exit(EXIT_FAILURE);
	}

	{
		uint32_t numOfReq=1;

		std::cout<<"******************************************************"<<std::endl;
		std::cout<<"                  Group size: "<<gs<<std::endl;
		std::cout<<"******************************************************"<<std::endl<<std::endl<<std::endl;

		for(unsigned c=0;c<10;c++)
		{
			vector <vertex> group=select(v1,gs);
			vertex dataOwner=selectOwner(v1,group);
			std::cout<<"Module vector size "<<module->size()<<std::endl;
			//set owner app and data object
			//----------------------------------------------------
			std::cout<<"ready to ask for position: "<<in[dataOwner]<<std::endl;
			Ptr<Sender> sa1=CreateObject<Sender>(module->at(in[dataOwner]),60);

			std::cout<<"Data owner: "<<module->at(in[dataOwner])->getNode()->GetId()<<std::endl;

			char d2 []={'h','e','l','l','o'};

			vector < Ptr < CCN_Name > >* nameVector=new vector < Ptr < CCN_Name > >();
			vector<string>* nv=new vector<string>();

			for(uint32_t i=1;i<=numOfReq;i++)
			{
				nv=new vector<string>();
				nv->push_back("domain1");
				nv->push_back("domain2");
				nv->push_back("domain3");

				stringstream sstream;
				sstream << i;

				nv->push_back(sstream.str());
				Ptr<CCN_Name> name=CreateObject<CCN_Name>(*nv);
				nameVector->push_back(name);
			}

			for(uint32_t i=0;i<numOfReq;i++)
			{				
				Ptr<Packet> packet = Create<Packet>((uint8_t*)d2, (uint32_t)strlen(d2));
				sa1->insertData(nameVector->at(i), packet);

				const pair < ns3::Ptr< CCN_Name >, int > pa2 (nameVector->at(i),5);
			}



			for(uint32_t i=0;i<numOfReq;i++)
			{
				//std::cout<<"putting: "<<nameVector->at(i)->toString()<<std::endl;

				if(sa1->getLocalApp()==0)
				{
					std::cout<<"XONO NULL STON OWNER"<<std::endl;
				}

				module->at(in[dataOwner])->getFIB()->put(nameVector->at(i),sa1->getLocalApp());
			}

			//----------------------------------------------------


			//set group apps and request data
			//----------------------------------------------------
			vector < Ptr < Receiver > >* vec=new vector < Ptr < Receiver > >();

			vector<string>* name3=new vector <string> ();
			name3->push_back("domain1");
			name3->push_back("domain2");
			name3->push_back("domain3");


			Ptr<CCN_Name> name4=CreateObject<CCN_Name>(*name3);

			//std::cout<<"module size: "<<module->size()<<std::endl;
			for(unsigned i=0;i<gs;i++)
			{   //std::cout<<"accessing: "<<in[group.at(i)]<<std::endl;
				vec->push_back(CreateObject<Receiver>(module->at(nodeToModule.find(in[group.at(i)])->second)));
			}

			this->i=CreateObject<Initializer>(module,p,module->at(in[dataOwner])->getNode()->GetId(),numOfReq,nodeToModule);
			this->i->initializeFIBs();

			for(unsigned i=0;i<gs;i++)
			{
				vec->at(i)->SendInterest(name4,numOfReq);
			}
			//----------------------------------------------------

			//PIT check
			//----------------------------------------------------
			Time t=ns3::Seconds(30);
			Simulator::Schedule(t,&BootstrappingHelper::PITCheck,this,gs,c);
			//----------------------------------------------------


			Simulator::Run();


			//print results and initialize structures again
			//----------------------------------------------------
			Ptr<ResultPrinter> rp=CreateObject<ResultPrinter>(*module,gs,c,sa1,*vec);

			//print to file
			//----------------------------------------------------
			ofstream file;
			file.open ("results.txt",std::ios::app);

			file << "experiment "<<c<<" groupsize "<<gs<<" participants ["<<module->at(in[dataOwner])->getNodeId()<<",";  //prota emfanizetai i pigi

			for(unsigned i=0;i<gs;i++)
			{
				file<<vec->at(i)->getModule()->getNodeId();

				if(i!=gs-1)
				{
					file<<",";
				}
			}

			file<<"] interests "<<CcnModule::RX_INTERESTS<<" data "<<CcnModule::RX_DATA<<"\n";

			file.close();

			//----------------------------------------------------

			CcnModule::RX_INTERESTS=0;
			CcnModule::RX_DATA=0;
			delete vec;

			for(unsigned i=0;i<module->size();i++)
			{
				module->at(i)->reInit();
			}

			//----------------------------------------------------

			Simulator::Stop();
		}
	}
}

std::vector <vertex> BootstrappingHelper::select(std::vector <vertex> v,unsigned gs)
{
	std::vector <vertex>* group=new std::vector <vertex>();

	for(unsigned i=0;i<gs;i++)
	{

		int pos=ExperimentGlobals::RANDOM_VAR->GetInteger(i+1,v.size()-1);
		//int pos=urv->GetInteger(i+1,v.size()-1);

	/*	urv->SetAttribute ("Min", DoubleValue (i+1));
		urv->SetAttribute ("Max", DoubleValue (v.size()-1));
		int pos=urv->GetInteger();*/


		std::swap(v[i],v[pos]);
	}

	for(unsigned i=0;i<gs;i++)
	{
		group->push_back(v.at(i));
	}

	return *group;
}

vertex BootstrappingHelper::selectOwner(std::vector <vertex> v1,std::vector <vertex> group)
{
	std::vector <vertex> d;

	for(unsigned i=0;i<v1.size();i++)
	{
		bool outOfGroup=true;

		for(unsigned j=0;j<group.size();j++)
		{
			if(in[v1.at(i)]==in[group.at(j)])
			{
				outOfGroup=false;
			}
		}

		if(outOfGroup)
		{
			d.push_back(v1.at(i));
		}
	}

	int pos=ExperimentGlobals::RANDOM_VAR->GetInteger(0,d.size()-1);
	//int pos=urv->GetInteger(0,d.size()-1);

	/*urv->SetAttribute ("Min", DoubleValue (0));
	urv->SetAttribute ("Max", DoubleValue (d.size()-1));
	int pos=urv->GetInteger();
*/
	std::cout<<"selectOwner: pos is "<<pos<<std::endl;
	return d.at(pos);
}

void BootstrappingHelper::PITCheck(int gs,int exp)
{//sto router tis pigis tha mou bgalei allo ena entry

	for(unsigned i=0;i<module->size();i++)
	{
		stringstream st;
		st << gs;

		stringstream st2;
		st2 << exp;

		ofstream file;

		file.open (("./pit_stats/gs-"+st.str()+"-experiment-"+st2.str()+".txt").c_str(),std::ios::app);
		file<<"router "<<module->at(i)->getNodeId()<<" pit_entries "<<module->at(i)->getPIT()->getSize()<<endl;
		file.close();
	}
}
