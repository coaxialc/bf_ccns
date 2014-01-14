/*
 * test-ccn-packets.h
 *
 *  Created on: Dec 28, 2013
 *      Author: tsilochr
 */

#ifndef TEST_CCN_PACKETS_H_
#define TEST_CCN_PACKETS_H_

#include "ns3/test.h"
#include "ns3/mod-module.h"

#include <iostream>
#include <string>

using namespace ns3;
using namespace std;

class CCNPacketsTestCase: public TestCase{
public:
	CCNPacketsTestCase();
	~CCNPacketsTestCase();

private:
  virtual void DoRun (void);
  void testInterests();
  void testData();


};


CCNPacketsTestCase::CCNPacketsTestCase(): TestCase("test ccn packets"){}

CCNPacketsTestCase::~CCNPacketsTestCase(){}

void CCNPacketsTestCase::DoRun (void){
	testInterests();
	testData();
}

void CCNPacketsTestCase::testInterests(){

}

void CCNPacketsTestCase::testData(){

}


#endif /* TEST_CCN_PACKETS_H_ */
