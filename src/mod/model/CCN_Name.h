
#ifndef CCN_NAME_H_
#define CCN_NAME_H_

#include <string>
#include <ns3/core-module.h>

class CCN_Name : public ns3::Object
{
    public:
	CCN_Name(std::vector< std::string* > name);
	std::vector< std::string* > name;
	~CCN_Name();
	std::string getValue();
};


#endif /* CCN_NAME_H_ */
