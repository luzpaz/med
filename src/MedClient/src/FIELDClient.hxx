#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include "ReceiverFactory.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM{
    
//exemple _FIELDClient<double,FIELDDOUBLE_ptr>
//_FIELDClient<int,FIELDINT_ptr>
template<class T1,class T2>
class FIELDClient : public MEDMEM::FIELD<T1>
{
private:
  typename T2::_ptr_type _fieldPtr;
  bool _ownSupport;
public:
  FIELDClient(typename T2::_ptr_type ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
private:
  void fillCopy();
};
#include "FIELDClient.cxx"

}
#endif
