#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include "ReceiverFactory.hxx"
#include "SenderFactory.hxx"
#include CORBA_CLIENT_HEADER(MED)

template<class T>
struct MapCppFieldServ {
  typedef T FieldPtrType;
  typedef T FieldGlobalType;
};

template<>
struct MapCppFieldServ<int> {
  typedef SALOME_MED::FIELDINT_ptr FieldPtrType;
  typedef SALOME_MED::FIELDINT FieldGlobalType;
};

template<>
struct MapCppFieldServ<double> {
  typedef SALOME_MED::FIELDDOUBLE_ptr FieldPtrType;
  typedef SALOME_MED::FIELDDOUBLE FieldGlobalType;
};

namespace MEDMEM{

template<class T>
class FIELDClient : public MEDMEM::FIELD<T>
{
private:
  typename MapCppFieldServ<T>::FieldPtrType _fieldPtr;
  int _refCounter;
public:
  FIELDClient(typename MapCppFieldServ<T>::FieldPtrType ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
private:
  void fillCopy();
};

#include "FIELDClient.cxx"

}
#endif
