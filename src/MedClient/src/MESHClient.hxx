#ifndef _MESHCLIENT_HXX
#define _MESHCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Mesh.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM {
class MESHClient : public MESH {

private :

  const SALOME_MED::MESH_var IOR_Mesh;

  mutable bool _complete;
  
  mutable int _refCounter;

public :

  MESHClient(const SALOME_MED::MESH_ptr m);
  virtual bool operator==(const MESH& other) const;

  void blankCopy();
  void fillCopy();
  virtual void addReference() const;
  virtual void removeReference() const;

private:
  virtual ~MESHClient();
};
}

#endif

