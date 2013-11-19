#include "ns3/Bloomfilter.h"

Bloomfilter::Bloomfilter(int length,bool [])
{
	this->length=length;
	filter=new bool[length];
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

Ptr<Bloomfilter> operator+(const ns3::Ptr<Bloomfilter>& f,const ns3::Ptr<Bloomfilter>& s)
{
	if(f->length!=s->length)
	{
		return 0;
	}

	bool* result=new bool [f->length];

	for(int i=0;i<f->length;i++)
	{
		if(f->filter[i]==1&&s->filter[i]==1)
		{
			result[i]=1;
		}
		else
		{
			result[i]=0;
		}
	}

	return Ptr<Bloomfilter>(new Bloomfilter(f->length,result));
}

