#include "MEDCouplingMeshFieldFactoryComponentClt.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"
#include "MEDCouplingUMeshClient.hxx"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestRunner.h>

#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServantTest)

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace ParaMEDMEM;

CPPUNIT_TEST_SUITE_REGISTRATION( SALOME_TEST::MEDCouplingCorbaServBasicsTestClt );

int main(int argc, char* argv[])
{
  CPPUNIT_NS::TestResult controller;
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        
#ifdef WIN32
  CPPUNIT_NS::TextTestProgressListener progress;
#else
  CPPUNIT_NS::BriefTestProgressListener progress;
#endif
  controller.addListener( &progress );
  SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argc=argc;
  SALOME_TEST::MEDCouplingCorbaServBasicsTestClt::_argv=argv;
  CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( suite );
  runner.run( controller);
  // ---  Print test in a compiler compatible format.
  std::ofstream testFile;
  testFile.open("UnitTestsResult", std::ios::out |  std::ios::trunc);
  //CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
  CPPUNIT_NS::CompilerOutputter outputter( &result, testFile );
  outputter.write(); 
  // ---  Run the tests.
  bool wasSucessful = result.wasSuccessful();
  testFile.close();
  // ---  Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}
