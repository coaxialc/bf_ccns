#ifndef TEXT_H_
#define TEXT_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mod-module.h"


	class Text  : public ns3::Object
	{
		private:

		Text();
		static ns3::Ptr<Text> text;
		std::map<std::string,std::string*>* textContainer;

		public:
		~Text();
		ns3::Ptr<CCN_Name> giveText(std::string* text);
		void removeText(std::string* text);
		static ns3::Ptr<Text> getPtr();
	};

#endif
