#ifndef _SUPPORTCLIENT_HXX
#define _SUPPORTCLIENT_HXX

#include "MEDMEM_Support.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED)

#include <memory>

typedef std::auto_ptr<MEDMEM::MESH> TMeshHolder;


namespace MEDMEM {
  class SUPPORTClient : virtual public SUPPORT {
    const SALOME_MED::SUPPORT_var IOR_Support;
    mutable bool _complete_support;
    TMeshHolder myMeshHolder;
  public:
    SUPPORTClient(const SALOME_MED::SUPPORT_ptr theSupport, MESH *theMesh = NULL);
    virtual ~SUPPORTClient();

    void blankCopy();
    void fillCopy();

    MEDSKYLINEARRAY *getnumber() const throw (MEDEXCEPTION);
    const int *getNumber(medGeometryElement GeometricType) const throw (MEDEXCEPTION);
    const int *getNumberIndex() const throw (MEDEXCEPTION);
  };
}


#endif
