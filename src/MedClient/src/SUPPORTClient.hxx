#ifndef _SUPPORTCLIENT_HXX
#define _SUPPORTCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Support.hxx"
#include CORBA_CLIENT_HEADER(MED)


namespace MEDMEM {
class SUPPORTClient : virtual public SUPPORT {

private :

  const SALOME_MED::SUPPORT_var IOR_Support;

  mutable bool _complete_support;

  mutable int _refCounter;

protected:

  virtual ~SUPPORTClient();

public :

  SUPPORTClient(const SALOME_MED::SUPPORT_ptr S, 
		MESH * M = NULL);

  void blankCopy();
  void fillCopy();

  MEDSKYLINEARRAY *  getnumber()
    const throw (MEDEXCEPTION);
  const int *  getNumber(MED_EN::medGeometryElement GeometricType) 
    const throw (MEDEXCEPTION);
  const int *  getNumberIndex()
    const throw (MEDEXCEPTION);
  void addReference() const;
  void removeReference() const;
};
}


#endif

