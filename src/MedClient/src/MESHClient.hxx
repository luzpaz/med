#ifndef _MESHCLIENT_HXX
#define _MESHCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Mesh.hxx"
#include CORBA_CLIENT_HEADER(MED)

class MESHClient : public MESH {

private :

  const SALOME_MED::MESH_var IOR_Mesh;

  mutable bool _complete;
  

public :

  MESHClient(const SALOME_MED::MESH_ptr m);

  virtual ~MESHClient();

  void blankCopy();
  void fillCopy();

};

#endif

