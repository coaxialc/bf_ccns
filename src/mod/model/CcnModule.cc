/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/CcnModule.h"
#include "ns3/Sender.h"
#include "ns3/Receiver.h"
#include <string>

class Sender;
class Receiver;

using namespace ns3;

int CcnModule::interestCount=0;
int CcnModule::dataCount=0;

		CcnModule::CcnModule(int length,int d,int switchh,ns3::Ptr<ns3::UniformRandomVariable> rv)
		{
			p_i_t=CreateObject<PIT>();
			this->length=length;//length of the Bloom filters
			this->switchh=switchh;//if zero ,hop counters start randomly ,otherwise they all start at max ,which is d
			this->d=d;//d is tha maximum value of the hop counter
			data=0;
			visited=false;
			text=Text::getPtr();
			FIB=CreateObject<Trie>(this);
			DATA=new std::vector < Ptr<CCN_Name> > ();
			ltd=new std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > > ();
			dtl=new std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >();
			this->rv=rv;
		}

		void CcnModule::reInit()
		{
			p_i_t=0;
			p_i_t=CreateObject<PIT>();
			data=0;
			visited=false;

            if (FIB)
            {
                FIB=0;
            }

            FIB=CreateObject<Trie>(this);

            if (DATA)
            {
            	delete DATA;
            }

			DATA=new std::vector < Ptr<CCN_Name> > ();

			/*delete ltd;
			delete dtl;
			ltd=new std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > > ();
			dtl=new std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >();*/

			//zero out apps to avoid having a Receiver or a Sender app next time if you don't need it
			s=0;
			r=0;
		}

		CcnModule::~CcnModule()
		{
			p_i_t=0;
			FIB=0;
			delete DATA;
			delete ltd;
			delete dtl;
		}

		void CcnModule::setNode(Ptr<Node> n)
		{
			this->n=n;

			for(unsigned i=0;i<n->GetNDevices();i++)
			{
				n->GetDevice(i)->SetReceiveCallback(MakeCallback(&CcnModule::receiveabc,this));
			}
		}

		void CcnModule::sendThroughDevice(Ptr<Packet> p,Ptr<NetDevice> nd)
		{
			Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();

		/*	if((pd->GetQueue()->IsEmpty()))
			{*/

				if(nd->GetChannel()->GetDevice(0)==nd)
				{
				//	Ptr<DropTailQueue> dtq=pd->GetQueue()->GetObject<DropTailQueue>();
				  //  dtq->SetMode(ns3::Queue::QUEUE_MODE_PACKETS);
					nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
				}
				else
				{
					//Ptr<DropTailQueue> dtq=pd->GetQueue()->GetObject<DropTailQueue>();
				//	dtq->SetMode(ns3::Queue::QUEUE_MODE_PACKETS);
					nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
				}
		/*	}
			else
			{
				ns3::Simulator::Schedule(MilliSeconds(10),&CcnModule::sendThroughDevice,this,p,nd);
			}*/
		}

		void CcnModule::send(Ptr<Packet> p,Ptr<Bloomfilter> bf,Ptr<NetDevice> excluded, std::string calledby)
		{
			/*for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				if(this->n->GetDevice(i)!=excluded&&equals(add(dtl->find(this->n->GetDevice(i))->second,bf),(dtl->find(this->n->GetDevice(i))->second)))
				{
					Ptr<NetDevice> nd=this->n->GetDevice(i);
					Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();

					if((pd->GetQueue()->IsEmpty()))
					{
					//	pd->GetQueue()->ResetStatistics();
						if(nd->GetChannel()->GetDevice(0)==nd)
						{
							//Ptr<DropTailQueue> dtq=pd->GetQueue()->GetObject<DropTailQueue>();
							//dtq->SetMode(ns3::Queue::QUEUE_MODE_PACKETS);

							uint8_t* b2=new uint8_t[p->GetSize()];
							p->CopyData(b2,p->GetSize());
							std::string dt(b2, b2+p->GetSize());
							std::cout<<"send:"<<" (caled by "<<calledby<<")"<<": packet before sending: "<<dt<<std::endl;
							nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
						}
						else
						{
							//Ptr<DropTailQueue> dtq=pd->GetQueue()->GetObject<DropTailQueue>();
							//dtq->SetMode(ns3::Queue::QUEUE_MODE_PACKETS);
							uint8_t* b2=new uint8_t[p->GetSize()];
							p->CopyData(b2,p->GetSize());
							std::string dt(b2, b2+p->GetSize());
							std::cout<<"send:"<<" (caled by "<<calledby<<")"<<": packet before sending: "<<dt<<std::endl;
							nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
						}
//					}
//					else
//					{
//						ns3::Simulator::Schedule(MilliSeconds(10),&CcnModule::send,this,p,bf,excluded);
//					}
				}
			}*/
		}


		bool CcnModule::receiveabc(Ptr<NetDevice> nd,Ptr<const Packet> p,uint16_t a,const Address& ad)
		{
			if(nd==0)
			{
				 std::cout<<"receiveabc: called with null device"<<std::endl;
			}

			std::string prename;

			uint8_t* b2=new uint8_t[p->GetSize()];
			p->CopyData(b2,p->GetSize());
			std::string dt(b2, b2+p->GetSize());

			int counter=0;
			while(dt[0]!='0'&&dt[0]!='1')
			{
				counter++;
				dt=dt.substr(1);
			}

			//int newsize=p->GetSize()-counter;

			//std::cout<<"Incoming packet size: "<<p->GetSize()<<std::endl;
		//	std::cout<<"Incoming payload: "<<dt<<std::endl;

			//extract bloom filter (variable length)
			//-----------------------------------
			std::string filter_string=dt.substr(0,this->length);

			ns3::Ptr<Bloomfilter> filter=CreateObject<Bloomfilter>(this->length,filter_string);
			//-----------------------------------

			//extract hop counter (2 characters)
			//-----------------------------------
			std::string hopcounter=dt.substr(this->length,2);
			int hopc=std::atoi(hopcounter.c_str());
			//-----------------------------------

			dt=dt.substr(this->length+2);

			int pos=dt.find("*");
			prename=dt.substr(0,pos);//pairnoume to onoma xoris *

			char type=dt.at(dt.length()-1);

			std::string name_value=prename;

			static std::string* name2=&name_value; //std::cout<<"------------name2: "<<*name2<<std::endl;
			Ptr<CCN_Name> name=text->giveText(name2);
			name->name.erase(name->name.begin());

		    //ola ta onomata ksekinane me / alla den exoune / sto telos

			if(type=='i')
			{
				interestCount++;
		//		std::cout<<"Interest received!"<<std::endl;
				/*Ptr<Receivers> receivers=(FIB->prefix(*name))->re;//koita an leei kati to FIB gia auto to interest

				if(receivers==0) return true;//an de ksereis ti na to kaneis agnoise to

			   //an o hop counter einai katallilos tote ftiakse to PIT
			   //----------------------------------
				if(hopc==0)
				{
					Ptr<PTuple> rec;
					rec=p_i_t->check(name);
					if(rec==0)
					{
						p_i_t->update(name,CreateObject<PTuple>(filter,(this->d)-hopc));
					}
					else
					{
						Ptr<PTuple> tuple=p_i_t->check(name);
						p_i_t->erase(name);
						if(tuple->ttl<(this->d)-hopc)
						{
							p_i_t->update(name,CreateObject<PTuple>(orbf(filter,dtl->find(nd)->second),(this->d)-hopc));
						}
						else
						{
							p_i_t->update(name,CreateObject<PTuple>(orbf(filter,dtl->find(nd)->second),tuple->ttl));
						}
					}
				}
			   //----------------------------------

				for(unsigned i=0;i<receivers->receivers->size();i++)
				{
					Object* o=&(*(receivers->receivers->at(i)));
					Sender* bca= dynamic_cast<Sender*> (o);

					if(bca!=0)//an einai na dothei se antikeimeno Sender
					{
						bca->InterestReceived(name);//push to app
					}
					else
					{
						//proothise
					}
				}
*/


				Ptr<PTuple> rec;
				if(hopc==0)
				{
					rec=p_i_t->check(name);
					if(rec==0)
					{
				//		std::cout<<"receiveabc: node: "<<this->node<<" updating PIT with name"<<name->getValue()<<std::endl;
						p_i_t->update(name,CreateObject<PTuple>(filter,(this->d)-hopc));

						sendInterest(name,this->d,0,nd);
					}
					else
					{
						Ptr<PTuple> tuple=p_i_t->check(name);
						p_i_t->erase(name);
						if(tuple->ttl<(this->d)-hopc)
						{
		//					std::cout<<"receiveabc: node: "<<this->node<<" updating PIT with name"<<name->getValue()<<std::endl;
							p_i_t->update(name,CreateObject<PTuple>(orbf(filter,dtl->find(nd)->second),(this->d)-hopc));
						}
						else
						{
		//					std::cout<<"receiveabc: node: "<<this->node<<" updating PIT with name"<<name->getValue()<<std::endl;
							p_i_t->update(name,CreateObject<PTuple>(orbf(filter,dtl->find(nd)->second),tuple->ttl));
						}
					}
				}
				else
				{
					this->sendInterest(name , hopc-1 , orbf(filter,dtl->find(nd)->second),nd);
				}
			}
			else if(type=='d')
			{
				data++;
				dataCount++;

				if(hopc<0)
				{
					return true;
				}

				//mipos einai gia 'mena?
				//--------------------------------------------
				if(this->r!=0)
				{
					std::string value=dt.substr(pos+1,dt.length()-pos-2);
					char* v=&value[0];
					r->DataArrived(name,v,value.length());
				}
				//--------------------------------------------


				//--------------------------------------------
				if(hopc==0)//continue using PIT
				{
					if(p_i_t->check(name)==0)
					{
						//agnoeitai
					}
					else
					{
						Ptr<PTuple> pt=p_i_t->check(name);
						if(pt->ttl==0) return true;

						std::string value=dt.substr(pos+1,dt.length()-pos-2);

						char* v=&value[0];

						this->sendData(name,v,value.length(),pt->bf,pt->ttl,nd);//exluding a netdevice
					}
				}
				else//continue using Bloom filters
				{
					std::string value=dt.substr(pos+1,dt.length()-pos-2);

					char* v=&value[0];

					this->sendData(name,v,value.length(),filter,hopc-1,nd);//excluding a netdevice
				}
				//--------------------------------------------
		    }

			return true;
		}

		void CcnModule::sendInterest(Ptr<CCN_Name> name,int hcounter,ns3::Ptr < Bloomfilter > bf,ns3::Ptr<ns3::NetDevice> nd)
		{
			Ptr<Bloomfilter> rec2;
			if(bf==0)
			{
				rec2=CreateObject<Bloomfilter>(this->length);//this constructor return an emtpy Bloom filter
			}
			else
			{
				rec2=bf;
			}

			std::string value=name->getValue();
			int length=value.length();

			char * d2=new char[length];
			std::copy(value.begin(),value.end(),d2);

			std::string hopc;
			if(hcounter==-1)//-1 hcounter means we must look at switchh
			{
				if(switchh==0)//switchh 0 means using a random value
				{
					this->rv->SetAttribute ("Min", DoubleValue (0));
				    this->rv->SetAttribute ("Max", DoubleValue (this->d));
				    int inthopc=this->rv->GetInteger();

				    if(inthopc>9)
				    {
				    	hopc=static_cast<ostringstream*>( &(ostringstream() << inthopc) )->str();
				    }
				    else
				    {
				    	hopc="0"+static_cast<ostringstream*>( &(ostringstream() << inthopc) )->str();
				    }
				}
				else//switchh 1 means using the maximium value ,d
				{
					if(this->d>9)
					{
						hopc=static_cast<ostringstream*>( &(ostringstream() << this->d) )->str();
					}
					else
					{
						hopc="0"+static_cast<ostringstream*>( &(ostringstream() << this->d) )->str();
					}
				}
			}
			else
			{
				if(hcounter>9)
				{
					hopc=static_cast<ostringstream*>( &(ostringstream() << hcounter) )->str();
				}
				else
				{
					hopc="0"+static_cast<ostringstream*>( &(ostringstream() << hcounter) )->str();
				}
			}

			std::string temp(d2,length);
			std::string temp2=rec2->getstring()+hopc+temp+"i";

			//const void* pointer2=temp.c_str();

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t *>(&temp2[0]),length+1+this->length+hopc.length());

			/*CCNPacketSizeHeader h2;
			h2.data=30;
			pa->AddHeader(h2);*/

			/*uint8_t* b2=new uint8_t[pa->GetSize()];
			pa->CopyData(b2,pa->GetSize());
*/
			Ptr<Receivers> receivers=(FIB->prefix(*name))->re;

			for(unsigned i=0;i<receivers->receivers->size();i++)
			{
				Object* o=&(*(receivers->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)//an einai na dothei se antikeimeno Sender
				{
					if(hcounter!=this->d)
					{
						//bca->InterestReceived2(name,bf,hopc);

						//interest tracking because the insterest was for us although hop counter is not zero
						//--------------------------------------------
						Ptr<PTuple> rec=p_i_t->check(name);
						if(rec==0)
						{
							p_i_t->update(name,CreateObject<PTuple>(bf,std::atoi(hopc.c_str())));
						}
						else
						{
							Ptr<PTuple> tuple=p_i_t->check(name);
							p_i_t->erase(name);
							if(tuple->ttl<(this->d)-std::atoi(hopc.c_str()))
							{
								p_i_t->update(name,CreateObject<PTuple>(orbf(bf,dtl->find(nd)->second),std::atoi(hopc.c_str())));
							}
							else
							{
								if(dtl->find(nd)->second==0)
								{
									if(nd==0)
									{
								//		std::cout<<"sendInterest: nd is null in node "<<this->node<<std::endl;
									}
									else
									{
										//std::cout<<"sendInterest: address not in map: "<<nd->GetAddress()<<std::endl;
									//	std::cout<<"sendInterest: map:"<<std::endl;
										for(unsigned i=0;i<this->n->GetNDevices();i++)
										{
											if(dtl->find(this->n->GetDevice(i))->second!=0)
											{
										//		std::cout<<"sendInterest: address: "<<this->n->GetDevice(i)->GetAddress()<<" "<<dtl->find(this->n->GetDevice(i))->second<<std::endl;
										    }
											else
											{
											//    std::cout<<"sendInterest: address: "<<this->n->GetDevice(i)->GetAddress()<<" not in map"<<std::endl;
											}
										}
									}
								}

								p_i_t->update(name,CreateObject<PTuple>(orbf(bf,dtl->find(nd)->second),tuple->ttl));
							}
						}
						//--------------------------------------------

						bca->InterestReceived(name);
					}
					else
					{
						bca->InterestReceived(name);
					}

				}
				else
				{
					sendThroughDevice(pa,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(receivers->receivers->at(i))))));
				}
			}
		}

		void CcnModule::sendData(ns3::Ptr<CCN_Name> name,char* buff, int bufflen,ns3::Ptr < Bloomfilter > bf,int ttl,Ptr<NetDevice> excluded)
		{
			//std::cout<<"sendData: node: "<<this->node<<" just called with name "<<name->getValue()<<std::endl;
			int time;
			Ptr<Bloomfilter> rec;
			if(bf==0)
			{
				if(p_i_t->check(name)==0)
				{
					std::cout<<"null to tuple"<<std::endl;
				}
				else
				{

				}
				rec=(p_i_t->check(name))->bf;
			}
			else
			{
				rec=bf;
			}

			if(bf==0)
			{
				time=(p_i_t->check(name))->ttl;
			}
			else
			{
				time=ttl;
			}

			std::string hopc;

			if(time>9)
			{
					hopc=static_cast<ostringstream*>( &(ostringstream() << time) )->str();
			}
			else
			{
					hopc="0"+static_cast<ostringstream*>( &(ostringstream() << time) )->str();
			}

			std::string temp(buff,bufflen);

			std::string n=name->getValue();

			std::string temp2=rec->getstring()+hopc+n+temp+"d";

		//	std::string stemp2=*temp2;
		//	const void* pointer=stemp2.c_str();




		//	Ptr<Packet> p = Create<Packet>(reinterpret_cast<const uint8_t *>(&temp2[0]),this->length+hopc.length()+1+n.length()+bufflen);
			//Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>("0011000000000000000010000000100000001000000000001000000100000000000010000001000000000000000000000010101000100000001100001010000000/domain1/domain2/domain3/4*hellod"),163);


			/* CCNPacketSizeHeader h2;
			 h2.data=30;
			 pa->AddHeader(h2);*/
		//	std::cout<<"sendData: bufflen: "<<bufflen<<std::endl;
			/*std::cout<<"sendData: sizes------------------------ "<<std::endl;
			std::cout<<"sendData: buffer size: "<<this->length+hopc.length()+1+n.length()+bufflen<<std::endl;
			std::cout<<"sendData: bufflen: "<<bufflen<<std::endl;
			std::cout<<"sendData: n.length(): "<<n.length()<<std::endl;
			std::cout<<"sendData: hopc size: "<<hopc.length()<<std::endl;
			std::cout<<"sendData: this->length: "<<this->length<<std::endl;
			std::cout<<"sendData: number 1: "<<"1"<<std::endl;
			std::cout<<"sendData: sizes------------------------ "<<std::endl;

			uint8_t* b2=new uint8_t[pa->GetSize()];
			pa->CopyData(b2,pa->GetSize());

			std::cout<<"sendData: copy data returned: "<<pa->CopyData(b2,pa->GetSize())<<std::endl;
			std::cout<<"sendData: Packet constructed contains: "<<b2<<std::endl;
			std::cout<<"sendData: Packet size: "<<pa->GetSize()<<std::endl;*/

			std::string calledby="modified";

			for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				Ptr<Packet> p=Create<Packet>(reinterpret_cast<const uint8_t *>(&temp2[0]),this->length+hopc.length()+1+n.length()+bufflen);
				if(this->n->GetDevice(i)!=excluded&&equals(add(dtl->find(this->n->GetDevice(i))->second,rec),(dtl->find(this->n->GetDevice(i))->second)))
				{
					Ptr<NetDevice> nd=this->n->GetDevice(i);
					Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();


					if(nd->GetChannel()->GetDevice(0)==nd)
					{


						/*uint8_t* b2=new uint8_t[p->GetSize()];
						p->CopyData(b2,p->GetSize());
						std::string dt(b2, b2+p->GetSize());
						std::cout<<"send:"<<" (caled by "<<calledby<<")"<<": packet before sending: "<<dt<<std::endl;*/
						nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
					}
					else
					{
						/*uint8_t* b2=new uint8_t[p->GetSize()];
						p->CopyData(b2,p->GetSize());
						std::string dt(b2, b2+p->GetSize());
						std::cout<<"send:"<<" (caled by "<<calledby<<")"<<": packet before sending: "<<dt<<std::endl;*/
						nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
					}
			    }
			}

			//p_i_t->erase(name);
		}

		void CcnModule::announceName(ns3::Ptr<CCN_Name> name, ns3::Ptr<Sender> app)
		{

		}

		void CcnModule::takeCareOfHashes()
		{
			for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				std::stringstream s;
				s<<this->n->GetDevice(i)->GetAddress();

				//std::cout<<"string produced: "<<s.str()<<std::endl;

				std::string result1=md5(s.str());
				//std::cout<<"md5 produced: "<<result1<<std::endl;

				std::string result2=sha1(s.str());
				//std::cout<<"sha1 produced: "<<result2<<std::endl;

				uint32_t integer_result1=(bitset<32>(stringtobinarystring(result1).substr(96))).to_ulong();//we only keep the last 32 bits
				uint32_t integer_result2=(bitset<32>(stringtobinarystring(result2).substr(96))).to_ulong();//we only keep the last 32 bits

				bool* filter=new bool[this->length];
				std::fill_n(filter, this->length, 0);

				for(int j=0;j<4;j++)
				{
					int index=(integer_result1+j*j*integer_result2)%(this->length);
					filter[index]=1;
				}
			//	std::cout<<"takecareofhashes Constructing filter with string: "<<filter<<std::endl;
				Ptr<Bloomfilter> bf=CreateObject<Bloomfilter>(this->length,filter);

			//	std::cout<<"filter: "<<bf->getstring()<<std::endl;

				const std::pair < ns3::Ptr< Bloomfilter >, ns3::Ptr< NetDevice > > pa (bf,this->n->GetDevice(i));
			    this->ltd->insert(pa);

			    const std::pair < ns3::Ptr< NetDevice > , ns3::Ptr < Bloomfilter > > pa2 (this->n->GetDevice(i),bf);
			    this->dtl->insert(pa2);
			}

			/*std::cout<<"diagnostic for the maps-----------------------"<<std::endl;
			for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				std::cout<<"device "<<i<<" gives "<<dtl->find(this->n->GetDevice(i))->second->getstring()<<std::endl;
				std::cout<<"Bloom filter "<<i<<" gives device with address"<<ltd->find(dtl->find(this->n->GetDevice(i))->second)->second->GetAddress()<<std::endl;
			}
			std::cout<<"diagnostic for the maps-----------------------"<<std::endl;*/
		}


		bool operator<(const ns3::Ptr<NetDevice>& f,const ns3::Ptr<NetDevice>& s)
		{
			std::stringstream stream;
			stream<<f->GetAddress();

			std::stringstream stream2;
			stream2<<s->GetAddress();

			if(stream.str()<stream2.str())
			{
				return true;
			}
			else if(stream.str()>stream2.str())
			{
				return false;
			}
			else
			{
				return false;
			}
		}

		std::string CcnModule::stringtobinarystring(std::string s)
		{
			std::string result="";

			for (std::size_t i = 0; i < s.size(); ++i)
			{
				result=result+(std::bitset<8>(s.c_str()[i])).to_string();
			}

			return result;
		}

		ns3::Ptr<Bloomfilter> CcnModule::add(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s)
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

			return CreateObject<Bloomfilter>(f->length,result);
		}

		bool CcnModule::equals(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s)
		{
			if(f->length!=s->length) return false;

			for(int i=0;i<f->length;i++)
			{
				if(f->filter[i]!=s->filter[i])
				{
					return false;
				}
			}

			return true;
		}

		ns3::Ptr<Bloomfilter> CcnModule::orbf(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s)
		{
			if(f==0)
			{
				 std::cout<<"orbf: f is null"<<std::endl;
			}
			else if(s==0)
			{
				 std::cout<<"orbf: s is null"<<std::endl;
			}


			if(f->length!=s->length)
			{
				 std::cout<<"orbf: different sizes"<<std::endl;
			     return 0;
			}

			bool* result=new bool [f->length];
			for(int i=0;i<f->length;i++)
			{
				result[i]=0;
			}

			for(int i=0;i<f->length;i++)
			{
			     if(f->filter[i]==1||s->filter[i]==1)
			     {
			         result[i]=1;
			     }
			     else
			     {
			         result[i]=0;
			     }
			}

		//	std::cout<<"OR between "<<f->getstring()<<" and "<<std::endl<<s->getstring()<<std::endl<<"gives "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;

			return CreateObject<Bloomfilter>(f->length,result);
		}
