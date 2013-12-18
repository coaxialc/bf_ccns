#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mod-module.h"

using namespace ns3;

int main(int argc ,char *argv[])
{


	/* Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue (0));
	  Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));
*/
	  /*Config::SetDefault ("ns3::ListErrorModel::ErrorRate", DoubleValue (0));
	  	  Config::SetDefault ("ns3::ListErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));*/

	  /*	Config::SetDefault ("ns3::ReceiveListErrorModel::ErrorRate", DoubleValue (0));
	  		  	  Config::SetDefault ("ns3::ReceiveListErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));*/

	 /* Config::SetDefault ("ns3::BurstErrorModel::ErrorRate", DoubleValue (0));
	  Config::SetDefault ("ns3::BurstErrorModel::BurstSize", StringValue ("ns3::UniformRandomVariable[Min=0|Max=1]"));*/
//	  Config::SetDefault("ns3::ErrorModel::IsEnabled",BooleanValue("false"));

	/*  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(10000000));
	  Config::SetDefault("ns3::DropTailQueue::MaxBytes", UintegerValue(10000000));*/


	/*Ptr<ErrorModel> em = ErrorModel::CreateDefault ();
	Ptr<RateErrorModel> rem = em->QueryInterface<RateErrorModel>(RateErrorModel::iid);

	     if (rem)
	    {
	       rem->SetRandomVariable (UniformVariable ());

	       rem->SetRate (0);

	       }

	      nd3->AddReceiveErrorModel (em);*/


	CommandLine c;
	c.Parse (argc, argv);
	//std::cout<<argv[1]<<std::endl;

	//std::string temp="topology.txt";
	Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>(std::atoi(argv[3]),argv[1],std::atoi(argv[2]),std::atoi(argv[4]),std::atoi(argv[5]),std::atoi(argv[6]));
//	Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>(129,"1221.txt",12,128,4,0);

	bh->parseTopology();

	bh->startExperiment();

	//bh=0;
	//delete Text::getPtr();

	Simulator::Destroy();
	return 0;
}
