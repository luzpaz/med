#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include CORBA_CLIENT_HEADER(MED)

template <typename T>
class FIELDClient : virtual public FIELD<T> {

private :

  const SALOME_MED::FIELD_var IOR_Field;

  mutable bool _complete_field;

public :

  FIELDClient(const SALOME_MED::FIELD_ptr S, 
		SUPPORT * S = NULL);
  virtual ~FIELDClient();

  void blankCopy();
  void fillCopy();


};

template <typename T>
FIELDClient<T>::FIELDClient(const SALOME_MED::FIELD_ptr F,
			    SUPPORT * S) : 
  FIELD<T>(), 
  IOR_Field(SALOME_MED::FIELD::_duplicate(F))
{
  BEGIN_OF("FIELDClient::FIELDClient(SALOME_MED::FIELD_ptr m)");

  if (S) setSupport(S);

  END_OF("FIELDClient::FIELDClient(SALOME_MED::FIELD_ptr m)");
}

template <typename T>
void FIELDClient<T>::blankCopy()
{
  BEGIN_OF("FIELDClient::blankCopy");


  END_OF("FIELDClient::blankCopy");

}

template <typename T>
void FIELDClient<T>::fillCopy()
{
  BEGIN_OF("FIELDClient::fillCopy");

  END_OF("FIELDClient::fillCopy");
}


template <typename T>
FIELDClient<T>::~FIELDClient()
{
  BEGIN_OF("FIELDClient::~FIELDClient");

  END_OF("FIELDClient::~FIELDClient");
}

#endif
