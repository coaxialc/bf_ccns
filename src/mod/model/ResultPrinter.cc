#include "ns3/ResultPrinter.h"

using namespace ns3;

ResultPrinter::ResultPrinter(std::vector < Ptr < CcnModule > > * module,int gs,int exp)
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
}

ResultPrinter::~ResultPrinter()
{

}
