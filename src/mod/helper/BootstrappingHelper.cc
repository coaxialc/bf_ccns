#include "BootstrappingHelper.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

using std::ofstream;
using std::stringstream;
using std::vector;
using std::string;

using std::endl;

using namespace ns3;

BootstrappingHelper::BootstrappingHelper(unsigned int s,string filename,int gsize,int length,int d,int switchh)
{
	this->filename=filename;
	vec3=new std::vector< Ptr < Object > >();

	RngSeedManager::SetSeed (s);
	this->d=d;
	this->length=length;
	this->gs=gsize;
	//std::cout<<"group size"<<gs<<std::endl;
	r=CreateObject<UniformRandomVariable>();
	this->switchh=switchh;

}

/*void BootstrappingHelper::staticStart(unsigned int s,string filename,int gsize,int length,int d,int switchh)
{
	this->filename=filename;
	vec3=new std::vector< Ptr < Object > >();

	RngSeedManager::SetSeed (s);
	this->d=d;
	this->length=length;
	this->gs=gsize;
	//std::cout<<"group size"<<gs<<std::endl;
	r=CreateObject<UniformRandomVariable>();
	this->switchh=switchh;
}*/

BootstrappingHelper::~BootstrappingHelper()
{
	r=0;
	delete vec3;
	p=0;
}


void BootstrappingHelper::parseTopology()
{
	//text=Text::getPtr();

	p=CreateObject<Parser>();

	p->parse(filename);

	module=new vector < Ptr < CcnModule > >();

	for(unsigned i=0;i<p->idToNode.size();i++)
	{
		module->push_back(CreateObject<CcnModule>(p->idToNode.find(i)->second,this->switchh,r));
	}

	for(unsigned i=0;i<p->idToNode.size();i++)
	{
		module->at(i)->installLIDs();
	}

	for(unsigned i=0;i<p->idToNode.size();i++)
	{
		for(unsigned j=0;j<p->getNeighbors(i).size();j++)
		{
			/*for(uint32_t k=0;k<module->at(i)->nodePtr->GetNDevices();k++)
			{
				module->at(i)->nodePtr->GetDevice(k)->GetChannel()
			}*/

			const std::pair< Ptr<NetDevice> , Ptr < CcnModule > > pa
			(
					ndfinder(   module->at(p->nodeToId(p->getNeighbors(i).at(j)))->getNode() ,   module->at(i)->getNode() ,   module->at((p->nodeToId(p->getNeighbors(i).at(j))->getNodeId()))   ,   module->at(i)->getNodeId())
					,
					module->at((int)(p->nodeToId(p->getNeighbors(i).at(j))))
		    );

			module->at(i)->getNeighborModules().insert(pa);
		}
	}
	//std::cout<<"MEGETHOS:   "<<p->kombos->size()<<std::endl;
}

Ptr<NetDevice> BootstrappingHelper::ndfinder(Ptr<Node> n1,Ptr<Node> n2,uint32_t i,uint32_t j)//epistrefei to net device tou deksiou me to opoio o deksis syndeetai ston aristero
{

	for(unsigned i=0;i<module->at(j)->getNode()->GetNDevices();i++)
	{
			if(module->at(j)->getNode()->GetDevice(i)->GetChannel()->GetDevice(0)->GetNode()==n1)
			{
					return module->at(j)->getNode()->GetDevice(i);
			}

			if(module->at(j)->getNode()->GetDevice(i)->GetChannel()->GetDevice(1)->GetNode()==n1)
			{
					return module->at(j)->getNode()->GetDevice(i);
			}
	}

	return 0;
}

void BootstrappingHelper::startExperiment()
{


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

	graph topology=p->getGraph();

	in = boost::get(boost::vertex_index, topology);

	typedef boost::graph_traits<graph>::vertex_iterator vi;

	std::pair<vi, vi> v;

	for(v=boost::vertices(topology);v.first!=v.second;++v.first)
	{
		if(boost::degree((*v.first),topology)==1)
		{
			v1.push_back((*v.first));
		}
	}

	std::cout<<"degree 1 exoun : "<<v1.size()<<std::endl;

	if(this->gs>v1.size()-1)
	{
		exit(EXIT_FAILURE);
	}

/*
	for(unsigned i=0;i<v1.size();i++)
	{
		std::cout<<in[v1.at(i)]<<std::endl;
	}
*/

	{
		uint32_t numOfReq=200;

		std::cout<<"******************************************************"<<std::endl;
		std::cout<<"                  Group size: "<<gs<<std::endl;
		std::cout<<"******************************************************"<<std::endl<<std::endl<<std::endl;

		for(unsigned c=0;c<10;c++)
		{
		//	std::cout<<"Experiment: "<<c<<std::endl<<std::endl;
			vector <vertex> group=select(v1,gs);
			vertex dataOwner=selectOwner(v1,group);

			//set owner app and data object
			//----------------------------------------------------
			Ptr<Sender> sa1=CreateObject<Sender>(module->at(in[dataOwner]),60);

		//	static vector <std::string*>* sv=new vector <string*>();
			char d2 []={'h','e','l','l','o'};

			/*for(int i=0;i<numOfReq;i++)
			{
				stringstream st;
				st << i;
				sv->push_back(new std::string("domain1/domain2/domain3/"+st.str()));
			}*/

			vector < Ptr < CCN_Name > >* nameVector=new vector < Ptr < CCN_Name > >();
			vector<string>* nv=new vector<string>();

			for(uint32_t i=0;i<numOfReq;i++)
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
				const pair < Ptr< CCN_Name >,char* > pa (nameVector->at(i),d2);

				sa1->insertData(pa);

				const pair < ns3::Ptr< CCN_Name >, int > pa2 (nameVector->at(i),5);

			//	sa1->length.insert(pa2);

			//	module->at(in[dataOwner])->DATA->push_back(nameVector->at(i));
			}
/*
			vec3=new vector < Ptr < Object > >();
			vec3->push_back(sa1);

			rec3=CreateObject<Receivers>(vec3);*/

			for(uint32_t i=0;i<numOfReq;i++)
			{
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

		//	static string* name3=new string("domain1/domain2/domain3/");
			Ptr<CCN_Name> name4=CreateObject<CCN_Name>(*name3);
		//	std::cout<<"ererer:      "<<name4->getValue()<<std::endl;

			for(unsigned i=0;i<gs;i++)
			{
				vec->push_back(CreateObject<Receiver>(module->at(in[group.at(i)])));
			}

			this->i=CreateObject<Initializer>(module,p,in[dataOwner],numOfReq);
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

	//		ns3::Time t2=ns3::Seconds(180);
	//		ns3::Simulator::Schedule(t2,&BootstrappingHelper::specificData,this);
			Simulator::Run();


			//print results and initialize structures again
			//----------------------------------------------------
			Ptr<ResultPrinter> rp=CreateObject<ResultPrinter>(module,gs,c,sa1);

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
		r->SetAttribute ("Min", DoubleValue (i+1));
		r->SetAttribute ("Max", DoubleValue (v.size()-1));
		int pos=r->GetInteger();

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

	r->SetAttribute ("Min", DoubleValue (0));
	r->SetAttribute ("Max", DoubleValue (d.size()-1));
	int pos=r->GetInteger();

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

//void BootstrappingHelper::specificData()
//{
//	for(unsigned i=0;i<module->size();i++)
//			{
//				std::cout<<module->at(i)->data<<std::endl;
//			}
//}
