#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <memory>

#include "MEDMEM_Field.hxx"
#include "MEDMEM_SWIG_FieldDouble.hxx"
#include "MEDMEM_SWIG_FieldInt.hxx"
#include "SUPPORTClient.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MED)


void FIELDClient_value(FIELD<double> *F, 
		       const SALOME_MED::FIELD_ptr IOR_Field);
void FIELDClient_value(FIELD<int> *F, 
		       const SALOME_MED::FIELD_ptr IOR_Field);

typedef std::auto_ptr<MEDMEM::SUPPORT> TSupportHolder;
  

template<typename TField>
void FIELDClientInit(FIELD<TField>* theTarget, 
		     const SALOME_MED::FIELD_ptr theSource, 
		     TSupportHolder& theSupportHolder)
{
  if(!theSupportHolder.get()){
    SALOME_MED::SUPPORT_var aSupport = theSource->getSupport();
    theSupportHolder.reset(new SUPPORTClient(aSupport));
  }
  theTarget->setSupport(theSupportHolder.get());
  theTarget->setName(theSource->getName());
  theTarget->setDescription(theSource->getDescription());
  int aNbOfComponents = theSource->getNumberOfComponents();
  theTarget->setNumberOfComponents(aNbOfComponents);
  int aNbOfElements = theSupportHolder->getNumberOfElements(MED_ALL_ELEMENTS);
  theTarget->setNumberOfValues(aNbOfComponents*aNbOfElements);
  
  std::vector<std::string> aComponentNames(aNbOfComponents);
  SALOME_MED::string_array_var aNames = theSource->getComponentsNames();
  for(int i = 0; i < aNbOfComponents; i++)
    aComponentNames[i] = aNames[i];
  theTarget->setComponentsNames(&aComponentNames[0]);
  
  theTarget->setIterationNumber(theSource->getIterationNumber());
  theTarget->setTime(theSource->getTime());
  theTarget->setOrderNumber(theSource->getOrderNumber());
  
  FIELDClient_value(theTarget,theSource);
}



class FIELDDOUBLEClient : public virtual FIELDDOUBLE{
  TSupportHolder mySupportHolder;
public:
  FIELDDOUBLEClient(const SALOME_MED::FIELD_ptr theField, SUPPORT *theSupport = NULL):
    mySupportHolder(theSupport)
  {
    FIELDClientInit(this,theField,mySupportHolder);
  }
  FIELDDOUBLE* GetPointer(){ return this;}
};


class FIELDINTClient : public virtual FIELDINT{
  TSupportHolder mySupportHolder;
public:
  FIELDINTClient(const SALOME_MED::FIELDINT_ptr theField, SUPPORT *theSupport = NULL):
    mySupportHolder(theSupport)
  {
    FIELDClientInit(this,theField,mySupportHolder);
  }
  FIELDINT* GetPointer(){ return this;}
};


#endif
