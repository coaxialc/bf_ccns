/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/CcnModule.h"
#include "ns3/Sender.h"
#include "ns3/Receiver.h"

class Sender;
class Receiver;

using namespace ns3;

int CcnModule::interestCount=0;
int CcnModule::dataCount=0;

		CcnModule::CcnModule(int length,int d)
		{
			p_i_t=CreateObject<PIT>();

			this->length=length;

			this->d=d;

			data=0;

			visited=false;

			text=Text::getPtr();

			FIB=CreateObject<Trie>(this);

			DATA=new std::vector < Ptr<CCN_Name> > ();

			ltd=new std::map < ns3::Ptr < Bloomfilter >, ns3::Ptr < ns3::NetDevice > > ();
			dtl=new std::map < ns3::Ptr < ns3::NetDevice > , ns3::Ptr < Bloomfilter > >();
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

		void CcnModule::send(Ptr<Packet> p,Ptr<NetDevice> nd)
		{
		/*	uint8_t* b=new uint8_t[p->GetSize()];
			p->CopyData(b,p->GetSize());

			std::string keimeno( b, b+p->GetSize() );

			std::cout<<"sending: "<<keimeno<<std::endl;*/
		
			Ptr<PointToPointNetDevice> pd = nd->GetObject<PointToPointNetDevice> ();
		//	Ptr<DropTailQueue> dt = pd->GetQueue()->GetObject<DropTailQueue> ();

		//	dt->S

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
				ns3::Simulator::Schedule(MilliSeconds(10),&CcnModule::send,this,p,nd);
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
			//-----------------------------------

			if(std::atoi(hopcounter.c_str())==0)
			{
				return 0;
			}
			else//modify the hopcounter
			{
				int new_counter=std::atoi(hopcounter.c_str())-1;
				if(new_counter>9)//one digit counter
				{
					std::string nc=static_cast<ostringstream*>( &(ostringstream() << new_counter) )->str();
				}
				else//two digit counter
				{
					std::string nc="0"+static_cast<ostringstream*>( &(ostringstream() << new_counter) )->str();
				}
			}

			dt=dt.substr(this->length+2);


			int pos=dt.find("*");
			prename=dt.substr(4,pos-4);//pairnoume to onoma xoris *
		//	std::cout<<"------------prename: "<<prename<<std::endl;
			char type=prename.at(0);

			prename=prename.substr(1); //std::cout<<"------------prename: "<<prename<<std::endl;

			std::string name_value=prename;
			//std::cout<<"------------name_value: "<<name_value<<std::endl;
		//	static std::string* name2=new std::string(name_value);  std::cout<<"------------name2: "<<*name2<<std::endl;
			static std::string* name2=&name_value; //std::cout<<"------------name2: "<<*name2<<std::endl;
			Ptr<CCN_Name> name=text->giveText(name2);
			name->name.erase(name->name.begin());

		    //ola ta onomata ksekinane me / alla den exoune / sto telos

			if(type=='i')
			{
			//	std::cout<<"mpike pio mesa to interest me name: "<<name->getValue()<<std::endl;
				interestCount++;

				Ptr<Receivers> rec=p_i_t->check(name);

				if(rec!=0)
				{
				//	std::cout<<"already in pit"<<std::endl<<std::endl;
					std::vector < ns3::Ptr<Object > > * receivers=new std::vector < ns3::Ptr < Object > > ();
					receivers->reserve(rec->receivers->size()+1);
					receivers->insert(receivers->end(),rec->receivers->begin(),rec->receivers->end());
					receivers->insert(receivers->end(),nd);

					Ptr<Receivers> nr=new Receivers(receivers);

					p_i_t->erase(name);
					p_i_t->update(name,nr);
				}
				else
				{
					std::vector< Ptr < Object > >* vec=new std::vector < Ptr < Object > >();
					vec->push_back(nd);

					p_i_t->update(name,new Receivers(vec));

					Ptr<Receivers> r=(FIB->prefix(*name))->re;

					std::string value=name->getValue();
					int length=value.length();

					char * d2=new char[length];
					std::copy(value.begin(),value.end(),d2);

					std::string temp(d2,length);
					temp="i"+temp;

					Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>(&temp[0]),length+1);

					CCNPacketSizeHeader h4;
					h4.data=length+1;
					pa->AddHeader(h4);

					for(unsigned i=0;i<r->receivers->size();i++)
					{
						Object* o=&(*(r->receivers->at(i)));
						Sender* bca= dynamic_cast<Sender*> (o);
						if(bca!=0)//an einai i efarmogi mas
						{
							bca->InterestReceived(name);
						}
						else//an einai net device
						{
							this->send(pa,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(r->receivers->at(i))))));
						}
					}
				}
			}
			else if(type=='d')
			{
				data++;
				dataCount++;

				if(p_i_t->check(name)==0)
				{
					//std::cout<<"Data came but no one is interested."<<std::endl<<std::endl;
				}
				else
				{

					Ptr<Receivers> rec=p_i_t->check(name);

					for(unsigned i=0;i<rec->receivers->size();i++)
					{
						Object* o=&(*(rec->receivers->at(i)));

						Receiver* bca= dynamic_cast<Receiver*> (o);
						if(bca!=0)//an einai i efarmogi mas
						{
							std::string value=dt.substr(pos+1);

							char* v=const_cast<char*>(value.c_str());
							bca->DataArrived(name,v,value.length());
						}
						else//an einai net device
						{
							this->send(p->Copy(),Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(rec->receivers->at(i))))));
						}
					}
				}
		    }

			return true;
		}

		void CcnModule::sendInterest(Ptr<CCN_Name> name, ns3::Ptr<Receiver> ba)
		{
		//	std::cout<<"sending "<<<<std::endl<<std::endl;
		/*	std::cout<<"node "<<this->node<<"--------------------------------"<<std::endl<<std::endl;
			std::cout<<"sending interest"<<std::endl<<std::endl;*/
			std::string value=name->getValue();
			int length=value.length();

			char * d2=new char[length];
			std::copy(value.begin(),value.end(),d2);

			std::string temp(d2,length);
			temp="i"+temp;

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>(&temp[0]),length+1);

			CCNPacketSizeHeader h3;
			h3.data=length+1;
			pa->AddHeader(h3);

			Ptr<Receivers> rec=p_i_t->check(name);

			if(rec!=0)
			{
				std::vector < ns3::Ptr<Object > > * receivers=new std::vector < ns3::Ptr < Object > > ();
				receivers->reserve(rec->receivers->size()+1);
				receivers->insert(receivers->end(),rec->receivers->begin(),rec->receivers->end());
				receivers->insert(receivers->end(),ba);

				Ptr<Receivers> nr=new Receivers(receivers);

				p_i_t->erase(name);
				p_i_t->update(name,nr);
			}
			else
			{
				std::vector< Ptr < Object > >* vec=new std::vector < Ptr < Object > >();
				vec->push_back(ba);
				p_i_t->update(name,new Receivers(vec));
			}

			//kseroume oti paei gia devices giati mas kalese i efarmogi mas

		/*	uint8_t* b=new uint8_t[pa->GetSize()];
		    pa->CopyData(b,pa->GetSize());
			std::string keimeno( b, b+pa->GetSize() );
			std::cout<<"sending interest: "<<keimeno<<std::endl;*/

			Ptr<Receivers> rec2=(FIB->prefix(*name))->re;

			//std::cout<<"receivers: "<<rec2->receivers->size()<<std::endl;

			for(unsigned i=0;i<rec2->receivers->size();i++)//for(unsigned i=0;i<1;i++)
			{
				this->send(pa,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(rec2->receivers->at(i))))));
			}
		}

		//to payload einai onoma kai dedomena ,xorizontai apo *
		void CcnModule::sendData(ns3::Ptr<CCN_Name> name, char* buff, int bufflen)
		{
			Ptr<Receivers> rec=p_i_t->check(name);

			int length=name->getValue().length();

			std::string temp(buff,bufflen);
			std::string temp2="d"+name->getValue()+temp;

			Ptr<Packet> pa = Create<Packet>(reinterpret_cast<const uint8_t*>(&temp2[0]),bufflen+length+1);

			CCNPacketSizeHeader h2;
			h2.data=bufflen+length+1;
			pa->AddHeader(h2);

			/*uint8_t* b=new uint8_t[pa->GetSize()];
			pa->CopyData(b,pa->GetSize());

			std::string keimeno( b, b+pa->GetSize() );
			std::cout<<"sending data: "<<keimeno<<std::endl;*/

			for(unsigned i=0;i<rec->receivers->size();i++)
			{
				this->send(pa,Ptr<NetDevice>(dynamic_cast<NetDevice*>(&(*(rec->receivers->at(i))))));
			}

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

				long integer_result1,integer_result2=0;

				//kane ta result1 kai result2 arithmous

				bool filter[]=new bool[this->length];

				for(int i=0;i<4;i++)
				{
					int index=(integer_result1+i*i*integer_result2)%(this->length);
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
