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
			//s=0;
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
				n->GetDevice(i)->SetReceiveCallback(MakeCallback(&CcnModule::handlePacket,this));
			}
		}

		void CcnModule::sendThroughDevice(Ptr<const Packet> p,Ptr<NetDevice> nd)
		{
			uint8_t* b=new uint8_t[p->GetSize()];
			p->CopyData(b,p->GetSize());

			Ptr<Packet> p2=Create<Packet>(b,p->GetSize());

			Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();

			if(nd->GetChannel()->GetDevice(0)==nd)
			{
				nd->Send(p2,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
			}
			else
			{
				nd->Send(p2,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
			}

			if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
			if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
		}

		bool CcnModule::handlePacket(Ptr<NetDevice> nd,Ptr< const Packet> p,uint16_t a,const Address& ad)
		{
			char type = extract_packet_type(p);
			if (type == 'i')
			{
				handleIncomingInterest(p, nd);
			}
			else if (type == 'd')
			{
				handleIncomingData(p, nd);
			}

			return true;
		}
		
		char CcnModule::extract_packet_type(Ptr<const Packet> p)
		{
			uint8_t* b2=new uint8_t[p->GetSize()];
			p->CopyData(b2,p->GetSize());

			return ((char)(b2[p->GetSize()-1]));
		}

		void CcnModule::handleIncomingInterest(Ptr<const Packet> p, Ptr<NetDevice> nd)
		{
			Ptr<CCN_Interest> interest = CreateObject<CCN_Interest>(p,this->length);
			interest->decreaseHopCounter();
			interest->getBloomfilter()->OR(this->dtl->find(nd)->second); //update BF in interest packet

			//perform fib lookup
			//pair<Ptr<NetDevice>, Prt<LocalApp> fib_lookup
			Ptr<TrieNode> tn= this->FIB->prefix(*(interest->getName()));

			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)
				{
				//	this->p_i_t->addRecord(interest->getName(), interest->getBloomfilter(), d - interest->getHopCounter());

					Ptr<PTuple> pt=p_i_t->check(interest->getName());
					if(pt!=0)
					{
						pt->bf->OR(interest->getBloomfilter());
						if(interest->getHopCounter()>pt->ttl)
						{
							pt->ttl=interest->getHopCounter();
						}
					}
					else
					{
						Ptr<PTuple> pt2=CreateObject<PTuple>(interest->getBloomfilter(),interest->getHopCounter());
						p_i_t->update(interest->getName(),pt2);
					}

					//bca->InterestReceived(interest);
					//this is ns3 specific, we 'll discuss why it needs to be done this way
					//ns3_schedule_event(localApp->handleInterest(interest))
					bca->InterestReceived(interest->getName());

					break;//we are done, no more Interest forwarding
				}
				/*else
				{
					this->sendInterest(interest, Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*o)))); //transmits only data over NetDevice
				}*/

				Ptr<PTuple> pt=p_i_t->check(interest->getName());
				if(pt!=0)//we already have the Interest in the PIT
				{
					interest->getBloomfilter()->OR(pt->bf);
					pt->bf=interest->getBloomfilter();
					if(interest->getHopCounter()>pt->ttl)
						{
						 pt->ttl=interest->getHopCounter();
						}
					// aggregate interest, no further forwarding
					break;
				}
			}

			if (interest->getHopCounter() == 0)
			{
				//Interest must be tracked because HC is 0
				this->p_i_t->update(interest->getName(),CreateObject<PTuple>( interest->getBloomfilter(), d - interest->getHopCounter()));

				//reset bf and HC in packet
				interest->setTtl(this->d);
				interest->getBloomfilter()=CreateObject<Bloomfilter>(this->length);
			}

			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca==0)
				{
					Ptr<Packet> packet = interest->serializeToPacket();
					sendThroughDevice(p,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(tn->re->receivers->at(i))))));
				}
			}
		}

		void CcnModule::handleIncomingData(Ptr<const Packet> p, Ptr<NetDevice> nd)
		{
			Ptr<CCN_Data> data = ns3::CreateObject<CCN_Data>(p,this->length);
			data->decreaseHopCounter();

			//always check PIT
			ns3::Ptr<PTuple> pt = this->p_i_t->check(data->getName());

			/* pit_lookup is a structure/class/whatever that contains:
			 * 1. Ptr<Bloomfilter> and ttl for further forwarding the data packet
			 * 2. set<Ptr<LocalApp> > for apps in this node that have requested this data packet
			 */

			//watch this carefully, we 'll discuss this closely on Tuesday
			if (pt != 0)//interest has been tracked at PIT
			{
				//give data to any local app
				std::vector < ns3::Ptr < Receiver > >* lr = pt->r;

				for(unsigned i=0;i<lr->size();i++)
				{
					ns3::Ptr<Receiver> l = lr->at(i);
					//ns3 specific, I 'll explain
				//	ns3_schedule_event(l->handleDataPacket(data.name, data.buffer))
					l->DataArrived(data->getName(),reinterpret_cast<char*>(data->getData()),data->getLength());
				}

				ns3::Ptr<Bloomfilter> bf = pt->bf;
				int ttl = pt->ttl;
				if (data->getHopCounter() == 0)
				{
					//replace bf and ttl in packet
					data->setBloomfilter(bf);
					data->setTtl(ttl);
				}
				else
				{ //THIS IS TRICKY, add (OR) bf and update ttl, NOT replace, we 'll discuss why this needs to be done
					data->getBloomfilter()->OR(bf);

					if(data->getHopCounter()>ttl)
					{
						data->setTtl(data->getHopCounter());
					}
					else
					{
						data->setTtl(ttl);
					}
				}

				if (data->getHopCounter() != 0)
				{
					sendDataLow(data->getBloomfilter(), data, nd);
				}
			}
		}

		void CcnModule::sendDataLow(ns3::Ptr<Bloomfilter> bf,ns3::Ptr<CCN_Data> data ,ns3::Ptr<ns3::NetDevice> excluded)
		{
			for(unsigned i=0;i<this->n->GetNDevices();i++)
			{
				ns3::Ptr<Packet> p=data->serializeToPacket();
				if(this->n->GetDevice(i)!=excluded&&equals(((dtl->find(this->n->GetDevice(i))->second)->AND(bf)),(dtl->find(this->n->GetDevice(i))->second)))
				{
					ns3::Ptr<NetDevice> nd=this->n->GetDevice(i);
					ns3::Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();

					if(nd->GetChannel()->GetDevice(0)==nd)
					{
						nd->Send(p,nd->GetChannel()->GetDevice(1)->GetAddress(),0x88DD);
						if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
						if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
					}
					else
					{
						nd->Send(p,nd->GetChannel()->GetDevice(0)->GetAddress(),0x88DD);
						if((pd->GetQueue()->GetTotalDroppedBytes()!=0)) std::cout<<"bytes dropped"<<std::endl;
						if((pd->GetQueue()->GetTotalDroppedPackets()!=0)) std::cout<<"packets dropped"<<std::endl;
					}
				}
			}
		}

		void CcnModule::sendInterest(ns3::Ptr<CCN_Interest> interest,ns3::Ptr<ns3::NetDevice> nd)
		{
			std::string payload=interest->getBloomfilter()->getstring()+stringForm(interest->getHopCounter())+interest->getName()->getValue()+"i";
			Ptr<Packet> p=Create<Packet>(reinterpret_cast<const uint8_t *>(&payload[0]),payload.length());
			sendThroughDevice(p,nd);
		}

		void CcnModule::sendInterest(ns3::Ptr<CCN_Name> name)
		{
			Ptr<CCN_Interest> interest=CreateObject<CCN_Interest>(name,decideTtl(),this->length);
			ns3::Ptr<TrieNode> tn = this->FIB->prefix(*(name));

			Ptr<PTuple> pt=p_i_t->check(name);
			if(pt!=0)
			{
				pt->r->push_back(r);
			}
			else
			{
				Ptr<PTuple> pt2=CreateObject<PTuple>(CreateObject<Bloomfilter>(this->length),0);
				pt2->r->push_back(r);
				p_i_t->update(name,pt2);
			}

			for(unsigned i=0;i<tn->re->receivers->size();i++)
			{
				Object* o=&(*(tn->re->receivers->at(i)));
				Sender* bca= dynamic_cast<Sender*> (o);

				if(bca!=0)//mporei auto poy zitame na to exei alli efarmogi pano apo ton idio kombo ,i kai emeis oi idioi
				{
					//this->p_i_t->addRecord(interest->getName(), interest->getBloomfilter(), d - interest->getHopCounter());
					bca->InterestReceived(interest->getName());
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

		/*void CcnModule::sendInterest(Ptr<CCN_Name> name,int hcounter,ns3::Ptr < Bloomfilter > bf,ns3::Ptr<ns3::NetDevice> nd)
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
		}*/

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
				if(this->n->GetDevice(i)!=excluded&&equals(((dtl->find(this->n->GetDevice(i))->second)->AND(rec)),(dtl->find(this->n->GetDevice(i))->second)))
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
