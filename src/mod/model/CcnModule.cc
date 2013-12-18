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

		bool CcnModule::handlePacket(Ptr<NetDevice> nd,Ptr<const Packet> p,uint16_t a,const Address& ad)
		{
			char type = extract_packet_type(p);
			if (type == "i")
			{
				handleIncomingInterest(p, nd);
			}
			else if (type == "d")
			{
				handleIncomingData(p, nd);
			}

			return true;
		}
		
		char CcnModule::extract_packet_type(Ptr<Packet> p)
		{
			uint8_t* b2=new uint8_t[p->GetSize()];
			p->CopyData(b2,p->GetSize());

			return ((char)(b2[p->GetSize()-1]));
		}

		void CcnModule::handleIncomingData(Ptr<const Packet> p, Ptr<NetDevice> nd)
		{
			data++;
			dataCount++;

			Ptr<CCN_Data> data=CreateObject<CCN_Data>(p);
			if(data->getHopCounter()<0) break;

			//mipos einai gia 'mena?
			//--------------------------------------------
			ns3::Ptr<TrieNode> tn=FIB->prefix(*(data->getName()));
			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0&&bca==)
			}
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
		}

		void CcnModule::handleIncomingInterest(Ptr<const Packet> p, Ptr<NetDevice> nd)
		{
			Ptr<CCN_Interest> interest = CreateObject<CCN_Interest>(p);
			interest->decreaseHopCounter();
			interest->getBloomfilter()->OR(Ptr<Bloomfilter>(dtl->find(nd)));
			
			Ptr<PTuple> tuple=this->p_i_t->check(interest->name);
			if (tuple!=0)
			{
				tuple->bf=interest->getBloomfilter();
				tuple->ttl=interest->getHopCounter();
			}
			else if (interest->getHopCounter() == 0)
			{
				existing_record_found = this->p_i_t->addRecord(interest->name, interest->getBF(), d - interest->hopCounter()+1)
				if (existing_record_found)
				{
					//aggregate interest, do not forward
					return;
				}
			}
			
			ns3::Ptr<TrieNode> tn = this->FIB->prefix(*(interest->name));
			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)
				{
					this->p_i_t->addRecord(interest->name, interest->getBloomfilter(), d - interest->getHopCounter());
					bca->InterestReceived(interest);
				}
				else
				{
					this->sendInterest(interest, Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*o)))); //transmits only data over NetDevice
				}
			}
		}

		void CcnModule::sendInterest(ns3::Ptr<CCN_Interest> interest,ns3::Ptr<ns3::NetDevice> nd)
		{
			std::string payload=interest->getBloomfilter()->getstring()+stringForm(interest->getHopCounter())+interest->getName()+"i";
			Ptr<Packet> p=Create<Packet>(reinterpret_cast<const uint8_t *>(&payload[0]),payload.length());
			sendThroughDevice(p,nd);
		}

		void CcnModule::sendInterest(ns3::Ptr<CCN_Name> name)
		{
			Ptr<CCN_Interest> interest=CreateObject<CCN_Interest>(name,decideTtl());
			ns3::Ptr<TrieNode> tn = this->FIB->prefix(*(name));
			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)//mporei auto poy zitame na to exei alli efarmogi pano apo ton idio kombo ,i kai emeis oi idioi
				{
					this->p_i_t->addRecord(interest->name, interest->getBloomfilter(), d - interest->getHopCounter());
					bca->InterestReceived(interest);
				}
				else
				{
					this->sendInterest(interest, Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*o)))); //transmits only data over NetDevice
				}
			}
		}

		int CcnModule::decideTtl()
		{
			if(switchh==0)//switchh 0 means using a random value
			{
				this->rv->SetAttribute ("Min", DoubleValue (0));
				this->rv->SetAttribute ("Max", DoubleValue (this->d));
				return this->rv->GetInteger();
			}
			else
			{
				return this->d;
			}
		}

		std::string CcnModule::stringForm(int hc)
		{
			if(hc>9)
			{
				return static_cast<ostringstream*>( &(ostringstream() << hc) )->str();
			}
			else
			{
				return "0"+static_cast<ostringstream*>( &(ostringstream() << hc) )->str();
			}
		}

			














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

				    stringForm(inthopc);
				}
				else
					stringForm(this->d);

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
