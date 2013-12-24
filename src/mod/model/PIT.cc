
#include <ns3/PIT.h>

using namespace std;

PIT::PIT()
{
	this->p=new std::map < ns3::Ptr<CCN_Name> ,ns3::Ptr < PTuple > >();
}

PIT::~PIT()
{
	delete p;
}

void PIT::update(ns3::Ptr<CCN_Name> name,ns3::Ptr < PTuple > re)
{
	//std::cout<<"mpainei sto pit to onoma:"<<name->getValue()<<std::endl;
	const std::pair < ns3::Ptr< CCN_Name >, ns3::Ptr< PTuple > > pa (name,re);
	this->p->insert(pa);
}

void PIT::erase(ns3::Ptr<CCN_Name> name)
{
	this->p->erase(name);
}

ns3::Ptr<PTuple> PIT::check(ns3::Ptr<CCN_Name> name)
{
	if(p->find(name)!=p->end())
	{
		return p->find(name)->second;
	}
	else
	{
		return 0;
	}

	return 0;
}


bool operator<(const ns3::Ptr<CCN_Name>& f,const ns3::Ptr<CCN_Name>& s)
{
	if(f->name.size()<s->name.size())
	{
		return true;
	}

	if(f->name.size()>s->name.size())
	{
		return false;
	}

	if(f->name.size()==s->name.size())
	{
		for(unsigned i=0;i<f->name.size();i++)
		{
			if((f->name.at(i))<(s->name.at(i)))
			{
				return true;
			}
			else if((f->name.at(i))>(s->name.at(i)))
			{
				return false;
			}
		}
	}

	return false;
}

/*bool PIT::addRecord(ns3::Ptr<CCN_Name> name,ns3::Ptr<Bloomfilter> f,int ttl)
{
	Ptr<PTuple> tuple=this->check(name);

	if (tuple!=0)
	{
		tuple->bf=f;
		tuple->ttl=ttl;

		return true;//existing record found
	}
	else
	{
		Ptr<PTuple> newTuple=CreateObject<PTuple>(f,ttl);
		this->update(name,newTuple);
		return false;
	}
}*/

