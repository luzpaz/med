#ifndef _FAMILYCLIENT_HXX
#define _FAMILYCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Family.hxx"
#include CORBA_CLIENT_HEADER(MED)
#include "SUPPORTClient.hxx"


namespace MEDMEM {
class FAMILYClient : 
  public SUPPORTClient, public FAMILY {

private :

  const SALOME_MED::FAMILY_var IOR_Family;

  mutable bool _complete;

public :

  FAMILYClient(const SALOME_MED::FAMILY_ptr S, 
		MESH * M = NULL);
  virtual ~FAMILYClient();

  virtual void blankCopy(bool blankSupport = true);
  virtual void fillCopy(bool fillSupport = true);
};

};

#endif
