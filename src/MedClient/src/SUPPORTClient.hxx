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

public :

  SUPPORTClient(const SALOME_MED::SUPPORT_ptr S, 
		MESH * M = NULL);
  virtual ~SUPPORTClient();

  void blankCopy();
  void fillCopy();

  MEDSKYLINEARRAY *  getnumber()
    const throw (MEDEXCEPTION);
//CCRT  const int *  getNumber(medGeometryElement GeometricType) 
  const med_int *  getNumber(medGeometryElement GeometricType) 
    const throw (MEDEXCEPTION);
//CCRT  const int *  getNumberIndex()
  const med_int *  getNumberIndex()
    const throw (MEDEXCEPTION);

};
}


#endif
