#include "MEDCouplingMeshFieldFactoryComponentServ.hxx"
#include "MEDCouplingMeshFieldFactoryComponent.hxx"

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace ParaMEDMEM;

int mainCorbaServ(int argc, char **argv);

int mainCorbaServ(int argc, char **argv)
{
  CORBA::ORB_var orb=CORBA::ORB_init(argc,argv);
  CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
  PortableServer::POAManager_var mgr=poa->the_POAManager();
  mgr->activate();
  SALOME_TEST::MEDCouplingMeshFieldFactoryComponent *compo=new SALOME_TEST::MEDCouplingMeshFieldFactoryComponent(orb);
  SALOME_TEST::MEDCouplingMeshFieldFactory_var compoPtr=compo->_this();
  CORBA::String_var str=orb->object_to_string(compoPtr);
  std::ofstream stream(SALOME_TEST::MEDCouplingCorbaServBasicsTest::buildFileNameForIOR().c_str());
  stream << str << std::endl;
  stream.close();
  orb->run();
  delete compo;
  return 0;
}

int main(int argc, char* argv[])
{
  return mainCorbaServ(argc,argv);
}
