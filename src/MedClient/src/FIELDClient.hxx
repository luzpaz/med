#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include CORBA_CLIENT_HEADER(MED)




void FIELDClient_value(FIELD<double> *F, 
		       const SALOME_MED::FIELD_ptr IOR_Field);
//CCRTvoid FIELDClient_value(FIELD<int> *F, 
void FIELDClient_value(FIELD<med_int> *F, 
		       const SALOME_MED::FIELD_ptr IOR_Field);
  

template <typename T>
FIELD<T> * FIELDClient(const SALOME_MED::FIELD_ptr IOR_Field, 
			SUPPORT * S = NULL)
{
  BEGIN_OF("FIELDClient<T>");

  if (!S) S = new SUPPORTClient(IOR_Field->getSupport());

  FIELD<T> *F = new FIELD<T>();
  F->setSupport(S);

  F->setName(IOR_Field->getName());
  SCRUTE(F->getName());

  F->setDescription(IOR_Field->getDescription());
  SCRUTE(F->getDescription());

  int nc = IOR_Field->getNumberOfComponents();
  F->setNumberOfComponents(nc);
  SCRUTE(F->getNumberOfComponents());

  F->setNumberOfValues(nc * S->getNumberOfElements(MED_ALL_ELEMENTS));
  SCRUTE(F->getNumberOfValues());

  string * _s = new string[nc];

  SALOME_MED::string_array_var s;

  s = IOR_Field->getComponentsNames();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  F->setComponentsNames(_s);

//  s = IOR_Field->getComponentsDescriptions();
//  for (int i=0; i<nc; i++)
//    _s[i] = s[i];
//  F->setComponentsDescriptions(_s);

  F->setIterationNumber(IOR_Field->getIterationNumber());
  F->setTime(IOR_Field->getTime());
  F->setOrderNumber(IOR_Field->getOrderNumber());

  FIELDClient_value(F, IOR_Field);

  END_OF("FIELDClient<T>");
  return F;
}

#endif
