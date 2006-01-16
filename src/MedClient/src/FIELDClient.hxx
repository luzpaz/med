#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include "ReceiverFactory.hxx"
#include "SenderFactory.hxx"
#include "MEDMEM_TraitsForFields.hxx"
#include "MEDMEM_InterlacingTraits.hxx"
#include CORBA_CLIENT_HEADER(MED)

namespace MEDMEM{

template<class T, class INTERLACING_TAG>
class FIELDClient : public MEDMEM::FIELD<T,INTERLACING_TAG>
{
private:
  typename FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldCorbaPtrType _fieldPtr;
  int _refCounter;
public:
  FIELDClient(typename FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldCorbaPtrType ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
private:
  void fillCopy();
};

#include "FIELDClient.cxx"

}
#endif
