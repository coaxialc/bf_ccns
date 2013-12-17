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
			//ltd=new std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > > ();
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
		//	delete ltd;
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

				if(nd->GetChannel()->GetDevice(0)==nd)
				{
					nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
				}
				else
				{
					nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
				}

				if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
				if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
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
				Ptr<PTuple> rec;
				if(hopc==0)
				{
					rec=p_i_t->check(name);
					if(rec==0)
					{
						p_i_t->update(name,CreateObject<PTuple>(orbf(filter,dtl->find(nd)->second),this->d));//eite paei gia proothisei eite gia anebasma ,thelei megisto hc

						sendInterest(name,this->d,0,nd);//eite paei gia proothisi eite gia anebasma ,thelei megisto hc
					}
					else
					{
						Ptr<PTuple> tuple=p_i_t->check(name);
						p_i_t->erase(name);//an oso einai sbismeno ,tote thelei na apantisei o allos?
					//	std::cout<<"node "<<this->node<<"after erase bf is: "<<tuple->bf->getstring()<<" and ttl is: "<<tuple->ttl<<std::endl;
						p_i_t->update(name,CreateObject<PTuple>( orbf(tuple->bf,(orbf(filter,dtl->find(nd)->second))) ,(this->d)));//eite paei gia proothisei eite gia anebasma ,thelei megisto hc
					}
				}
				else
				{
					this->sendInterest(name , hopc-1 , orbf(filter,dtl->find(nd)->second),nd);//an telika einai gia anebasma ,tha to ayksisoume ksana ,aplos boleuei na einai -1 pros to paron ,-1 tha xreiastei an einai gia proothisi
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
					//	std::cout<<"Packet ignored ,did not know what to do."<<std::endl;
					}
					else
					{
						Ptr<PTuple> pt=p_i_t->check(name);
					//	if(pt->ttl==0) return true;

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
				rec2=CreateObject<Bloomfilter>(this->length);//this constructor returns an emtpy Bloom filter
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

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t *>(&temp2[0]),length+1+this->length+hopc.length());

			Ptr<Receivers> receivers=(FIB->prefix(*name))->re;

			for(unsigned i=0;i<receivers->receivers->size();i++)
			{
				Object* o=&(*(receivers->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)//an einai na dothei se antikeimeno Sender
				{
					int newcounter=this->d;
					if(hcounter!=this->d)//an den mas kalesane me megisto counter simainei oti klithikame apo ti periptosi poy prepei na auksithei o counter an einai na paei se efarmogi
					{
						newcounter=hcounter+1;
					}

					if(hcounter!=this->d)//mas kalesane me oxi megisto counter ,ara klithikame apo ekei poy den exei ginei track idi
					{
						Ptr<PTuple> rec=p_i_t->check(name);

						if(rec==0)
						{
							if(newcounter==this->d)
							{
								p_i_t->update(name,CreateObject<PTuple>(rec2,0));
							}
							else
							{
								p_i_t->update(name,CreateObject<PTuple>(rec2,(this->d)-newcounter));
							}

							bca->InterestReceived(name);
						}
						else
						{
							Ptr<PTuple> tuple=p_i_t->check(name);
							p_i_t->erase(name);//an oso einai sbismeno ,tote thelei na apantisei o allos?
						//	std::cout<<"node "<<this->node<<"after earase bf is: "<<tuple->bf->getstring()<<" and ttl is: "<<tuple->ttl<<std::endl;
						//	me pooi kano update na do an ginetai sosta

							int temporary=0;
							if(newcounter==this->d)
							{
								temporary=0;
							}
							else
							{
								temporary=(this->d)-newcounter;
							}

							if(tuple->ttl<temporary)
							{
								p_i_t->update(name,CreateObject<PTuple>(orbf(rec2,tuple->bf),temporary));
							}
							else
							{
								p_i_t->update(name,CreateObject<PTuple>(orbf(rec2,tuple->bf),tuple->ttl));
							}
						}
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
			int time;
			Ptr<Bloomfilter> rec;

			if(bf==0)
			{
				if(p_i_t->check(name)==0) std::cout<<"null to tuple"<<std::endl;
				rec=(p_i_t->check(name))->bf;
				time=(p_i_t->check(name))->ttl;
			}
			else
			{
				rec=bf;
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
						std::string dt(b2, b2+p->GetSize());	*/
						nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
						if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
						if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
					}
					else
					{
						/*uint8_t* b2=new uint8_t[p->GetSize()];
						p->CopyData(b2,p->GetSize());
						std::string dt(b2, b2+p->GetSize());*/
						nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
						if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
						if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
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

			//	const std::pair < ns3::Ptr< Bloomfilter >, ns3::Ptr< NetDevice > > pa (bf,this->n->GetDevice(i));
			 //   this->ltd->insert(pa);

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

			/*std::cout<<"--------------------------------------------------------------------"<<endl;
			std::cout<<"adding "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
			std::cout<<"result: "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;
			std::cout<<"--------------------------------------------------------------------"<<endl;*/

			return CreateObject<Bloomfilter>(f->length,result);
		}

		bool CcnModule::equals(ns3::Ptr<Bloomfilter> f,ns3::Ptr<Bloomfilter> s)
		{
			if(f->length!=s->length) return false;

			for(int i=0;i<f->length;i++)
			{
				if(f->filter[i]!=s->filter[i])
				{
					/*std::cout<<"--------------------------------------------------------------------"<<endl;
					std::cout<<"equals: "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
					std::cout<<"result: false"<<std::endl;
					std::cout<<"--------------------------------------------------------------------"<<endl;*/
					return false;
				}
			}

//			std::cout<<"--------------------------------------------------------------------"<<endl;
//			std::cout<<"equals: "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
//			std::cout<<"result: true"<<std::endl;
//			std::cout<<"--------------------------------------------------------------------"<<endl;

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

			/*std::cout<<"--------------------------------------------------------------------"<<endl;
			std::cout<<"or: "<<f->getstring()<<" and "<<s->getstring()<<std::endl;
			std::cout<<"result: "<<CreateObject<Bloomfilter>(f->length,result)->getstring()<<std::endl;
			std::cout<<"--------------------------------------------------------------------"<<endl;*/
			return CreateObject<Bloomfilter>(f->length,result);
		}
