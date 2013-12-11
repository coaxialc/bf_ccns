
#include "ns3/CCN_Name.h"

CCN_Name::CCN_Name(std::vector< std::string* > name)
{
	this->name=name;
}

CCN_Name::~CCN_Name()
{

}

std::string CCN_Name::getValue()
{
	std::string result="";

	for(unsigned i=0;i<name.size();i++)
	{
		result=result+"/"+*(name.at(i));
	}

	result=result+"*";

	//std::cout<<"CCN_Name.getValue: "<<result<<std::endl;

	return result;
}
