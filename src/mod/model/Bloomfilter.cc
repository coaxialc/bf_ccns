#include "ns3/Bloomfilter.h"

Bloomfilter::Bloomfilter(int length,bool bits [])
{
	this->length=length;
	filter=new bool[length];

	for(int i=0;i<length;i++)
	{
		if(bits[i]==0)
		{
			filter[i]=0;
		}
		else
		{
			filter[i]=1;
		}
	}
}

Bloomfilter::Bloomfilter(int length,std::string bits)
{
	this->length=length;
	filter=new bool[length];

	for(int i=0;i<length;i++)
	{
		if(bits[i]=='0')
		{
			filter[i]=0;
		}
		else if(bits[i]=='1')
		{
			filter[i]=1;
		}
		else
		{
			std::cout<<"wrong bits: "<<bits[i]<<std::endl;
		}
	}

	//std::cout<<"o kataskeuastis:   "<<this->getstring()<<std::endl;
}

Bloomfilter::Bloomfilter(int length)
{
	this->length=length;
	filter=new bool[length];

	for(int i=0;i<length;i++)
	{
		filter[i]=0;
	}
}

Bloomfilter::~Bloomfilter()
{
	delete filter;
}

std::string Bloomfilter::getstring()
{
	std::string result="";

	for(int i=0;i<length;i++)
	{
		if(filter[i]==0)
		{
			result=result+"0";
		}
		else
		{
			result=result+"1";
		}
	}

	return result;
}

bool operator<(const ns3::Ptr<Bloomfilter>& f,const ns3::Ptr<Bloomfilter>& s)
{
	if(f->length<s->length)
	{
		return true;
	}
	else if(f->length>s->length)
	{
		return false;
	}
	else
	{
		for(int i=0;i<f->length;i++)
		{
			if(f->filter[i]==1&&s->filter[i]==0)
			{
				return false;
			}
			else if(f->filter[i]==0&&s->filter[i]==1)
			{
				return true;
			}
		}

		return false;
	}
}

bool operator<(const Bloomfilter& f,const Bloomfilter& s)
{
	if(f.length<s.length)
	{
		return true;
	}
	else if(f.length>s.length)
	{
		return false;
	}
	else
	{
		for(int i=0;i<f.length;i++)
		{
			if(f.filter[i]==1&&s.filter[i]==0)
			{
				return false;
			}
			else if(f.filter[i]==0&&s.filter[i]==1)
			{
				return true;
			}
		}

		return false;
	}
}

ns3::Ptr<Bloomfilter> Bloomfilter::AND(ns3::Ptr<Bloomfilter> f)
{
	if(f->length!=this->length)
	{
		 return 0;
	}

	bool* result=new bool [f->length];

	for(int i=0;i<f->length;i++)
	{
		 if(f->filter[i]==1&&this->filter[i]==1)
		 {
			 result[i]=1;
		 }
		 else
		 {
			 result[i]=0;
		 }
	}

	/*std::cout<<"--------------------------------------------------------------------"<<endl;
	std::cout<<"adding "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
	std::cout<<"result: "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;
	std::cout<<"--------------------------------------------------------------------"<<endl;*/

	return CreateObject<Bloomfilter>(f->length,result);
}

void Bloomfilter::OR(ns3::Ptr<Bloomfilter> f)
{
	if(f==0)
	{
		 std::cout<<"orbf: f is null"<<std::endl;
	}
	/*else if(this->filter==0)
	{
		 std::cout<<"orbf: s is null"<<std::endl;
	}*/


	if(f->length!=this->length)
	{
		 std::cout<<"orbf: different sizes"<<std::endl;
	}

	for(int i=0;i<f->length;i++)
	{
		 if(f->filter[i]==1||this->filter[i]==1)
		 {
			 this->filter[i]=1;
		 }
	}

//	std::cout<<"OR between "<<f->getstring()<<" and "<<std::endl<<s->getstring()<<std::endl<<"gives "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;

	/*std::cout<<"--------------------------------------------------------------------"<<endl;
	std::cout<<"or: "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
	std::cout<<"result: "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;
	std::cout<<"--------------------------------------------------------------------"<<endl;*/
}
