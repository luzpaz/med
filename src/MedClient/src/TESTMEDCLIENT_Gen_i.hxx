#ifndef _TESTMEDCLIENT_GEN_I_HXX_
#define _TESTMEDCLIENT_GEN_I_HXX_

#include <TESTMEDCLIENT_Gen.hh>
#include <iostream.h>
#include "SALOME_Component_i.hxx"

class TESTMEDCLIENT_Gen_i :
  public virtual POA_SALOME_TESTMEDCLIENT::TESTMEDCLIENT_Gen,
  public virtual Engines_Component_i 
{
public:
  TESTMEDCLIENT_Gen_i(CORBA::ORB_ptr orb,
	    PortableServer::POA_ptr poa,
	    PortableServer::ObjectId * contId, 
	    const char *instanceName, 
	    const char *interfaceName);
  virtual ~TESTMEDCLIENT_Gen_i();

  void go(SALOME_MED::MED_ptr objMed);
  void go2(SALOME_MED::MED_ptr objMed);
};

#endif
