
#include "ns3/Parser.h"
#include <vector>
#include <stdlib.h>
#include <string>

using namespace ns3;


	//	std::string text;

		Parser::Parser(std::string filename)
		{
			this->filename=filename;
			kombos=new std::vector <  Ptr<Node>  >();
			connection=new std::vector< std::vector < int > >();
			v1=new std::vector <unsigned> ();
			v2=new std::vector <unsigned> ();

			//topology.directed_category;
		}

		Parser::~Parser()
		{
			//delete kombos;
		}



		void Parser::parse()
		{
			std::ifstream f;



			std::string line;//std::cout<<this->filename<<std::endl;
			f.open(("/home/Coaxial/workspaceb/ns-3-dev/src/mod/model/"+this->filename).c_str());

			int megethos=0;
			if(f.is_open())
			{
				while(getline(f,line))
				{
					std::istringstream s(line);
					std::string t;

					std::getline(s,t,'\t');
					if(std::atoi(t.c_str())>megethos)
					{
						megethos=std::atoi(t.c_str());
					}
				}

				f.close();
			}

			for(int i=0;i<megethos;i++)
			{
				kombos->push_back(CreateObject<Node>());
				connection->push_back(*(new std::vector<int>()));
			}

			std::string line2;

			f.open(("/home/Coaxial/workspaceb/ns-3-dev/src/mod/model/"+this->filename).c_str());
			if(f.is_open())
			{
				while(getline(f,line2))
				{
				    std::istringstream s(line2);
				    std::string t;

				    int position=0;
				    while(std::getline(s,t,'\t'))
				    {
				    	if(t.at(0)!='<')
				    	{
				    		position=std::atoi(t.c_str());
				    		position--;
				    	}
				    	else if(t.at(0)=='<')
				    	{
				    	//  	std::cout<<"pushing at pos: "<<position<<std::endl;
				    		connection->at(position).push_back(std::atoi((t.substr(1,t.length()-2)).c_str())-1);
				    	}
				    }
				}

				f.close();
			}


			    for(unsigned i=0;i<connection->size();i++)
				{

			    		for(unsigned j=0;j<connection->at(i).size();j++)
			    		{
			    			bool connected=false;
			    			for(unsigned k=0;k<v1->size();k++)
			    			{
			    				if((v1->at(k)==i&&v2->at(k)==(unsigned)(connection->at(i).at(j)))||(v1->at(k)==(unsigned)(connection->at(i).at(j))&&v2->at(k)==i))
			    				{
			    					connected=true;
			    				}
			    			}

			    			if(!connected)
			    			{
			    				NodeContainer n;
			    				Ptr<Node> k1=(kombos->at(i));
			    				Ptr<Node> k2=(kombos->at(connection->at(i).at(j)));
			    				n.Add(k1);
			    				n.Add(k2);

			    				PointToPointHelper pph;
			    				pph.SetQueue(std::string("ns3::DropTailQueue"),std::string("MaxPackets"),ns3::UintegerValue(50000000), std::string("MaxBytes"),ns3::UintegerValue(50000000));
			    				pph.SetDeviceAttribute("DataRate", StringValue ("5Mbps"));
			    				pph.SetChannelAttribute("Delay", StringValue ("2ms"));
			    				ndc=pph.Install(n);

			    								v1->push_back(i);
			    				v2->push_back(connection->at(i).at(j));

			    				boost::add_edge(i,connection->at(i).at(j),topology);
			    			}

			    		}

			    }
			}


		graph Parser::getGraph()
		{
			return topology;
		}










