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
			std::cout<<"wrong bits: "<<bits.at(i)<<std::endl;
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
