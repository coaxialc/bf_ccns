#include "ns3/Sender.h"
#include <vector>
#include "stdlib.h"

	Sender::Sender(ns3::Ptr<CcnModule> ccnm,int waitingTime)
	{
		this->ccnm=ccnm;
		this->waitingTime=waitingTime;
		//dose FIb eggrafes kai onomata
		this->interests=0;
	}

	Sender::~Sender()
	{

	}

	    void Sender::AnnounceName(ns3::Ptr<CCN_Name> name)
		{
			this->ccnm->announceName(name, this);
		}

	  /*  void Sender::InterestReceived2(ns3::Ptr<CCN_Name> ccnn,ns3::Ptr<Bloomfilter> bf,std::string hopc)
	    		{
	    			//std::cout<<"------------------Interest received: "<<ccnn->getValue()<<std::endl;
	    			ns3::Time t=ns3::Seconds(this->waitingTime);

	    			if(data.find(ccnn)->second==0)
	    			{
	    				std::cout<<"NUL!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	    			}
	    		//	std::cout<<"ONOMA:      "<<ccnn->getValue()<<std::endl;
	    			char* test=data.find(ccnn)->second;

	    			ns3::Simulator::Schedule(t,&Sender::SendData2,this,ccnn,test,length.find(ccnn)->second,bf,hopc);
	    		}*/



		void Sender::InterestReceived(ns3::Ptr<CCN_Name> ccnn)
		{
			interests++;

			ns3::Time t=ns3::Seconds(this->waitingTime);




			char* test=data.find(ccnn)->second;



		//	std::cout<<"InterestReceived: node: "<<ccnm->node<<" replying with data"<<std::endl;

			ns3::Simulator::Schedule(t,&Sender::SendData,this,ccnn,test,length.find(ccnn)->second);//we use tha same length for everyone
		}

		void Sender::SendData(ns3::Ptr<CCN_Name> data,char* buff, int bufflen)
		{
	//		std::cout<<"Sender calling sendData with payload: "<<buff<<" and length "<<bufflen<<std::endl;
			this->ccnm->sendData( data, buff, bufflen,0,0,0);
		}

		void Sender::MapTest(int l)
		{
		//	std::cout<<"MapTest------------------------------------------"<<std::endl;
			/*for(int i=0;i<l;i++)
			{
				int i=0;
				stringstream st;
				st << i;

				Ptr<CCN_Name> name4=Text::getPtr()->giveText(new std::string("domain1/domain2/domain3/"+st.str()));
				char* tc=data.find(name4)->second;
				int ti=length.find(name4)->second;
			//	std::cout<<i<<"pointer: "<<&(*(data.find(name4)->second))<<std::endl;
				std::string s1(tc,ti);
				std::cout<<i<<": data: "<<s1<<std::endl;
				std::cout<<i<<": length: "<<length.find(name4)->second<<std::endl;
			}
*/
//			std::cout<<i<<": data: "<<<<std::endl;
//			std::cout<<i<<": length: "<<length[0]->second<<std::endl;

			//std::cout<<"MapTest------------------------------------------"<<std::endl;
		}

		/*void Sender::SendData2(ns3::Ptr<CCN_Name> data, char* buff, int bufflen,ns3::Ptr<Bloomfilter> bf,std::string hopc)
				{
					this->ccnm->sendData( data, buff, bufflen,bf,atoi(hopc.c_str()),0);
				}*/

		ns3::TypeId Sender::GetTypeId(void)
		{
			static ns3::TypeId t=ns3::TypeId("SENDER");
			t.SetParent<Object>();
			//t.AddConstructor<CCNPacketSizeHeader>();

			return t;
		}

		ns3::TypeId Sender::GetInstanceTypeId(void) const
		{
			return GetTypeId();
		}
