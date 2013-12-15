#include "ns3/BootstrappingHelper.h"
#include <vector>
#include <sstream>

using namespace ns3;

BootstrappingHelper::BootstrappingHelper(unsigned int s,std::string filename,int gsize,int length,int d,int switchh)
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

BootstrappingHelper::~BootstrappingHelper()
{
	r=0;
	delete vec3;
	p=0;
}


void BootstrappingHelper::parseTopology()
{
	text=Text::getPtr();

	p=CreateObject<Parser>(filename);

	p->parse();

	module=new std::vector < Ptr < CcnModule > >();

	for(unsigned i=0;i<p->kombos->size();i++)
	{
		module->push_back(CreateObject<CcnModule>(this->length,this->d,this->switchh,r));
	}

	for(unsigned i=0;i<p->kombos->size();i++)
	{
		module->at(i)->setNode((p->kombos->at(i)));
		module->at(i)->node=i;
		module->at(i)->takeCareOfHashes();


	/*	for(unsigned j=0;j<module->at(i)->n->GetNDevices();j++)
		{
			module->at(i0->dtl->find(module->at(i)->n->GetDevice(j))->second
		}*/

		/*for(unsigned j=0;j<module->at(i)->n->GetNDevices();j++)
				{
					Ptr<PointToPointNetDevice> pd = module->at(i)->n->GetDevice(j)->GetObject<PointToPointNetDevice> ();
					Ptr<DropTailQueue> dtq=pd->GetQueue()->GetObject<DropTailQueue>();
					//DropTailQueue* dtq= dynamic_cast<DropTailQueue*> (pd->GetQueue());
					dtq->SetMode(ns3::Queue::QUEUE_MODE_PACKETS);
				}*/



	}

	for(unsigned i=0;i<p->kombos->size();i++)
	{
		for(unsigned j=0;j<p->connection->at(i).size();j++)
		{
			const std::pair< int , Ptr < CcnModule > > pa (j,module->at(p->connection->at(i).at(j)));
			module->at(i)->map.insert(pa);
		}
	}
	std::cout<<"MEGETHOS:   "<<p->kombos->size()<<std::endl;
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
		int numOfReq=20;

		std::cout<<"******************************************************"<<std::endl;
		std::cout<<"                  Group size: "<<gs<<std::endl;
		std::cout<<"******************************************************"<<std::endl<<std::endl<<std::endl;

		for(unsigned c=0;c<10;c++)
		{
		//	std::cout<<"Experiment: "<<c<<std::endl<<std::endl;
			std::vector <vertex> group=select(v1,gs);
			vertex dataOwner=selectOwner(v1,group);

			//set owner app and data object
			//----------------------------------------------------
			Ptr<Sender> sa1=CreateObject<Sender>(module->at(in[dataOwner]),60);

			static std::vector <std::string*>* sv=new std::vector <std::string*>();
			char d2 []={'h','e','l','l','o'};

			for(int i=0;i<numOfReq;i++)
			{
				stringstream st;
				st << i;
				sv->push_back(new std::string("domain1/domain2/domain3/"+st.str()));
			}

			std::vector < ns3::Ptr < CCN_Name > >* nameVector=new std::vector < ns3::Ptr < CCN_Name > >();

			for(int i=0;i<numOfReq;i++)
			{
				nameVector->push_back(text->giveText(sv->at(i)));

			}

			for(int i=0;i<numOfReq;i++)
			{
				const std::pair < ns3::Ptr< CCN_Name >,char* > pa (nameVector->at(i),d2);

				sa1->data.insert(pa);

				const std::pair < ns3::Ptr< CCN_Name >, int > pa2 (nameVector->at(i),5);

				sa1->length.insert(pa2);

				module->at(in[dataOwner])->DATA->push_back(nameVector->at(i));
			}

			vec3=new std::vector < Ptr < Object > >();
			vec3->push_back(sa1);

			rec3=CreateObject<Receivers>(vec3);

			for(int i=0;i<numOfReq;i++)
			{
				module->at(in[dataOwner])->FIB->put(*(nameVector->at(i)),rec3);
			}

			//----------------------------------------------------


			//set group apps and request data
			//----------------------------------------------------
			std::vector < Ptr < Receiver > >* vec=new std::vector < Ptr < Receiver > >();

			static std::string* name3=new std::string("domain1/domain2/domain3/");
			Ptr<CCN_Name> name4=text->giveText(name3);
		//	std::cout<<"ererer:      "<<name4->getValue()<<std::endl;

			for(unsigned i=0;i<gs;i++)
			{
				vec->push_back(CreateObject<Receiver>(module->at(in[group.at(i)])));
			}

			this->i=CreateObject<Initializer>(module,p->connection,in[dataOwner]);
			this->i->initializeFIBs();

			for(unsigned i=0;i<gs;i++)
			{
				vec->at(i)->SendInterest(name4,numOfReq);
			}
			//----------------------------------------------------

			//PIT check
			//----------------------------------------------------
			ns3::Time t=ns3::Seconds(30);
			ns3::Simulator::Schedule(t,&BootstrappingHelper::PITCheck,this,gs,c);
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
			file.open ("results.txt",ios::app);

			file << "experiment "<<c<<" groupsize "<<gs<<" participants ["<<module->at(in[dataOwner])->node<<",";  //prota emfanizetai i pigi

			for(unsigned i=0;i<gs;i++)
			{
				file<<vec->at(i)->ccnm->node;

				if(i!=gs-1)
				{
					file<<",";
				}
			}

			file<<"] interests "<<CcnModule::interestCount<<" data "<<CcnModule::dataCount<<"\n";

			file.close();

			//----------------------------------------------------

			CcnModule::interestCount=0;
			CcnModule::dataCount=0;
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

		file.open (("./pit_stats/gs-"+st.str()+"-experiment-"+st2.str()+".txt").c_str(),ios::app);
		file<<"router "<<module->at(i)->node<<" pit_entries "<<module->at(i)->p_i_t->p->size()<<std::endl;
		file.close();
	}
}

void BootstrappingHelper::specificData()
{
	for(unsigned i=0;i<module->size();i++)
			{
				std::cout<<module->at(i)->data<<std::endl;
			}
}
