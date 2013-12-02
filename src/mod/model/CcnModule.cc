/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/CcnModule.h"
#include "ns3/Sender.h"
#include "ns3/Receiver.h"

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
			p_i_t=CreateObject<PIT>();
			data=0;
			visited=false;

            if (FIB)
            {
                FIB=0;
            }

            FIB=new Trie(this);

            if (DATA)
            {
            	delete DATA;
            }

			DATA=new std::vector < Ptr<CCN_Name> > ();

			delete ltd;
			delete dtl;
			ltd=new std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > > ();
			dtl=new std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >();

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

			if((pd->GetQueue()->IsEmpty()))
			{
				if(nd->GetChannel()->GetDevice(0)==nd)
				{
					nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
				}
				else
				{
					nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
				}
			}
			else
			{
				ns3::Simulator::Schedule(MilliSeconds(10),&CcnModule::sendThroughDevice,this,p,nd);
			}
		}

		void CcnModule::send(Ptr<Packet> p,Ptr<Bloomfilter> bf,Ptr<NetDevice> excluded)
		{
			for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				if(this->n->GetDevice(i)!=excluded&&equals(add(dtl->find(this->n->GetDevice(i))->second,bf),(dtl->find(this->n->GetDevice(i))->second)))
				{
					Ptr<NetDevice> nd=this->n->GetDevice(i);
					Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();

					if((pd->GetQueue()->IsEmpty()))
					{
						if(nd->GetChannel()->GetDevice(0)==nd)
						{

							nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
						}
						else
						{
							nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
						}
					}
					else
					{
						//ns3::Time t=ns3::MilliSeconds(10);
						ns3::Simulator::Schedule(MilliSeconds(10),&CcnModule::send,this,p,bf,excluded);
					}
				}
			}
		}


		bool CcnModule::receiveabc(Ptr<NetDevice> nd,Ptr<const Packet> p,uint16_t a,const Address& ad)
		{
			std::string prename;

			uint8_t* b2=new uint8_t[p->GetSize()];
			p->CopyData(b2,p->GetSize());

			std::string dt(b2, b2+p->GetSize());//an de metrietai to header na bgalo to 4

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

			char type=prename.at(0);

			prename=prename.substr(1); //std::cout<<"------------prename: "<<prename<<std::endl;

			std::string name_value=prename;

			static std::string* name2=&name_value; //std::cout<<"------------name2: "<<*name2<<std::endl;
			Ptr<CCN_Name> name=text->giveText(name2);
			name->name.erase(name->name.begin());

		    //ola ta onomata ksekinane me / alla den exoune / sto telos

			if(type=='i')
			{
				interestCount++;
				/*Ptr<Receivers> receivers=(FIB->prefix(*name))->re;//koita an leei kati to FIB gia auto to interest

				if(receivers==0) return true;//an de ksereis ti na to kaneis agnoise to

				for(unsigned i=0;i<receivers->receivers->size();i++)
				{
					Object* o=&(*(receivers->receivers->at(i)));
					Sender* bca= dynamic_cast<Sender*> (o);

					if(bca!=0)//an einai na dothei se antikeimeno Sender
					{
						//ftiakse to PIT
						//----------------------------------

						//----------------------------------

						bca->InterestReceived(name);//push to app
					}
					else
					{
						//ftiakse to PIT
						//----------------------------------

						//----------------------------------

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
						p_i_t->update(name,CreateObject<PTuple>(filter,(this->d)-hopc));

						sendInterest(name,this->d,0);
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
				else
				{
					this->sendInterest(name , hopc-1 , orbf(filter,dtl->find(nd)->second));
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
					std::string value=dt.substr(pos+1);
					char* v=const_cast<char*>(value.c_str());
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

						std::string value=dt.substr(pos+1);
						char* v=const_cast<char*>(value.c_str());
						this->sendData(name,v,value.length(),pt->bf,pt->ttl,nd);//exluding a netdevice
					}
				}
				else//continue using Bloom filters
				{
					std::string value=dt.substr(pos+1);
					char* v=const_cast<char*>(value.c_str());
					this->sendData(name,v,value.length(),filter,hopc-1,nd);//excluding a netdevice
				}
				//--------------------------------------------
		    }

			return true;
		}

		void CcnModule::sendInterest(Ptr<CCN_Name> name,int hcounter,ns3::Ptr < Bloomfilter > bf)
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
				    hopc=static_cast<ostringstream*>( &(ostringstream() << (this->rv->GetInteger())) )->str();
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
			temp=rec2->getstring()+hopc+"i"+temp;

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>(&temp[0]),length+1+this->length+hopc.length());

			Ptr<Receivers> receivers=(FIB->prefix(*name))->re;

			for(unsigned i=0;i<receivers->receivers->size();i++)
			{
				Object* o=&(*(receivers->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)//an einai na dothei se antikeimeno Sender
				{
					bca->InterestReceived(name);
				}
				else
				{
					sendThroughDevice(pa,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(receivers->receivers->at(i))))));
				}
			}
		}

		void CcnModule::sendData(ns3::Ptr<CCN_Name> name, char* buff, int bufflen,ns3::Ptr < Bloomfilter > bf,int ttl,Ptr<NetDevice> excluded)
		{
			int time;
			Ptr<Bloomfilter> rec;
			if(bf==0)
			{
				Ptr<Bloomfilter> rec=(p_i_t->check(name))->bf;
			}
			else
			{
				rec=bf;
			}

			if(ttl==0)
			{
				time=(p_i_t->check(name))->ttl;
			}
			else
			{
				time=ttl;
			}

			int length=name->getValue().length();
			std::string hopc;

			hopc=static_cast<ostringstream*>( &(ostringstream() << time) )->str();

			std::string temp(buff,bufflen);
			std::string temp2=rec->getstring()+hopc+"d"+name->getValue()+temp;

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>(&temp2[0]),bufflen+length+1+this->length+hopc.length());

			this->send(pa,rec,excluded);

			p_i_t->erase(name);
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

				std::cout<<"string produced: "<<s.str()<<std::endl;

				std::string result1=md5(s.str());
				std::cout<<"md5 produced: "<<result1<<std::endl;

				std::string result2=sha1(s.str());
				std::cout<<"sha1 produced: "<<result2<<std::endl;


				unsigned long integer_result1=(bitset<128>(stringtobinarystring(result1))).to_ulong();
				unsigned long integer_result2=(bitset<160>(stringtobinarystring(result2))).to_ulong();

				bool* filter=new bool[this->length];

				for(int j=0;j<4;j++)
				{
					int index=(integer_result1+j*j*integer_result2)%(this->length);
					filter[index]=1;
				}
				Ptr<Bloomfilter> bf=CreateObject<Bloomfilter>(this->length,filter);

				std::cout<<"filter: "<<bf->getstring()<<std::endl;

				const std::pair < ns3::Ptr< Bloomfilter >, ns3::Ptr< NetDevice > > pa (bf,this->n->GetDevice(i));
			    this->ltd->insert(pa);

			    const std::pair < ns3::Ptr< NetDevice > , ns3::Ptr < Bloomfilter > > pa2 (this->n->GetDevice(i),bf);
			    this->dtl->insert(pa2);
			}
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

			return Ptr<Bloomfilter>(new Bloomfilter(f->length,result));
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
			if(f->length!=s->length)
			{
			     return 0;
			}

			bool* result=new bool [f->length];

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

			return Ptr<Bloomfilter>(new Bloomfilter(f->length,result));
		}

