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
	Ptr<UniformRandomVariable> urv=CreateObject<UniformRandomVariable>();
	RngSeedManager::SetSeed (129);

	//ExperimentGlobals::RANDOM_VAR=new UniformRandomVariable();

	//Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>(std::atoi(argv[3]),argv[1],std::atoi(argv[2]),std::atoi(argv[4]),std::atoi(argv[5]),std::atoi(argv[6]));
	Ptr<BootstrappingHelper> bh=CreateObject<BootstrappingHelper>("/home/Coaxial/bf_workspace/ns-3-dev1/src/mod/model/1221.txt",12,128,4,0,urv);

	//ExperimentGlobals::RANDOM_VAR = new UniformRandomVariable();


	/*std::cout<<"prin"<<std::endl;
	std::cout<<"TEST: "<<ExperimentGlobals::RANDOM_VAR->GetInteger(0,10)<<std::endl;
	std::cout<<"meta"<<std::endl;
*/
	//Ptr<BootstrappingHelper> bh;

	//bh.staticStart(std::atoi(argv[3]),argv[1],std::atoi(argv[2]),std::atoi(argv[4]),std::atoi(argv[5]),std::atoi(argv[6]));

	bh->parseTopology();

	bh->startExperiment();

	//bh=0;
	//delete Text::getPtr();

	Simulator::Destroy();
	return 0;
}
