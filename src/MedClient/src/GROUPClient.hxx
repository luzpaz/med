#ifndef _GROUPCLIENT_HXX
#define _GROUPCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Group.hxx"
#include CORBA_CLIENT_HEADER(MED)
#include "SUPPORTClient.hxx"

class GROUPClient : 
  public SUPPORTClient,
  public GROUP
{

private :

  const SALOME_MED::GROUP_var IOR_Group;

  mutable bool _complete;

public :

  GROUPClient(const SALOME_MED::GROUP_ptr S, 
		MESH * M = NULL);
  virtual ~GROUPClient();

  virtual void blankCopy();
  virtual void fillCopy();
};


#endif
