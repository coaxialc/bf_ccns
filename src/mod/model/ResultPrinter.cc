#include "ns3/ResultPrinter.h"

using namespace ns3;

ResultPrinter::ResultPrinter(std::vector < Ptr < CcnModule > > * module,int gs,int exp,ns3::Ptr<Sender> s)
{
	this->module=module;

	std::cout<<"-----------------Interest and data count-----------------"<<std::endl;

	std::cout<<CcnModule::interestCount<<" interests appeared."<<std::endl<<CcnModule::dataCount<<" data appeared."<<std::endl;

	std::cout<<"-----------------Interest and data count-----------------"<<std::endl;

	for(unsigned i=0;i<module->size();i++)
		{
			stringstream st;
			st << gs;

			stringstream st2;
			st2 << exp;

			ofstream file;

			file.open (("./data_stats/gs-"+st.str()+"-experiment-"+st2.str()+".txt").c_str(),ios::app);
			file<<"router "<<module->at(i)->node<<" data_count "<<module->at(i)->data<<std::endl;
			file.close();
		}

	for(unsigned i=0;i<module->size();i++)
	{
		if(module->at(i)->r!=0)
		{
			//std::cout<<"receiver found"<<std::endl;
			if(module->at(i)->r->returned!=module->at(i)->r->askedfor)
			{
				std::cout<<"A Receiver did not get everything it asked for!"<<std::endl;
			}
			std::cout<<"Receiver got "<<module->at(i)->r->returned<<".Asked for "<<module->at(i)->r->askedfor<<std::endl;
		}
	}

	std::cout<<"Sender got "<<s->interests<<" interests."<<std::endl;
}

ResultPrinter::~ResultPrinter()
{

}
