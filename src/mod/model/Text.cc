#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mod-module.h"


using namespace ns3;

Ptr<Text> Text::text=0;

	Text::Text()
	{
		textContainer=new std::map<std::string,std::string*>();
	}

	Text::~Text()
	{
		delete textContainer;
	}

	Ptr<Text> Text::getPtr()
	{
		if(!text)
		{
			text=new Text;
		}

		return text;
	}

	Ptr<CCN_Name> Text::giveText(std::string* text)
	{
		//std::cout<<"getting value "<<*text<<std::endl;
		std::vector<std::string*>* res=new std::vector<std::string*>();

		std::vector < std::string >* name=new std::vector < std::string >();
		std::istringstream s(*text);
		std::string piece;

		while(std::getline(s,piece,'/'))
		{
			name->push_back(piece);
		}

		for(unsigned i=0;i<name->size();i++)
		{
			if(textContainer->find(name->at(i))!=textContainer->end())
			{
				std::string* r=textContainer->find(name->at(i))->second;
				res->push_back(r);
			}
			else
			{
				std::pair<std::string,std::string* > pa (name->at(i),&(name->at(i)));//to & edo einai ok?
				textContainer->insert(pa);

				std::string* r2=textContainer->find(name->at(i))->second;
				res->push_back(r2);
			}
		}

		Ptr<CCN_Name> result=CreateObject<CCN_Name>(*res);
		//std::cout<<"QWEQWE"<<result->getValue()<<std::endl;
		return (result);

		return 0;
	}

	void Text::removeText(std::string* text)
	{
		(*textContainer).erase(*text);
	}




