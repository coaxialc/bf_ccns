#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mod-module.h"
#include "ns3/BootstrappingHelper.h"


using namespace ns3;

int main(int argc ,char *argv[])
{
	CommandLine c;
	c.Parse (argc, argv);

	//RngSeedManager::SetSeed (std::atoi(argv[3]));
	RngSeedManager::SetSeed (1000);


	/*Ptr<UniformRandomVariable> urv=CreateObject<UniformRandomVariable>();
	ExperimentGlobals::RANDOM_VAR = PeekPointer(urv);*/

	ExperimentGlobals::RANDOM_VAR =CreateObject<UniformRandomVariable>();



	//************************************************
	//Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>("/home/Coaxial/bf_workspace/ns-3-dev1/src/mod/model/"+*(new std::string(argv[1])),std::atoi(argv[2]),std::atoi(argv[4]),std::atoi(argv[5]),std::atoi(argv[6]));
	//************************************************


	string filepath = "/home/Coaxial/bf_workspace/ns-3-dev1/src/mod/model/new_topology.txt";
	Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>(filepath,1,128,4,0);

	bh->parseTopology();

	bh->startExperiment();

	//bh=0;
	//delete Text::getPtr();

	Simulator::Destroy();
	return 0;
}
