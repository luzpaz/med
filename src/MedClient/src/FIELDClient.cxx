//#include "ReceiverFactory.hxx"

//using namespace MEDMEM;

template<class T1,class T2>
FIELDClient<T1,T2>::FIELDClient(typename T2::_ptr_type ptrCorba,SUPPORT * S):_fieldPtr(T2::_duplicate(ptrCorba)),_ownSupport(false)
{
  if (!S) 
    {
      _ownSupport=true;
      S=new SUPPORTClient(_fieldPtr->getSupport());
    }
  FIELD<T1>::setSupport(S);

  setName(_fieldPtr->getName());

  FIELD<T1>::setDescription(_fieldPtr->getDescription());
  int nc = _fieldPtr->getNumberOfComponents();
  FIELD<T1>::setNumberOfComponents(nc);

  FIELD<T1>::setNumberOfValues( S->getNumberOfElements(MED_ALL_ELEMENTS));

  string * _s = new string[nc];

  SALOME_MED::string_array_var s;
  s = _fieldPtr->getComponentsNames();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  FIELD<T1>::setComponentsNames(_s);

  s = _fieldPtr->getComponentsDescriptions();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  FIELD<T1>::setComponentsDescriptions(_s);

  s = _fieldPtr->getComponentsUnits();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  FIELD<T1>::setMEDComponentsUnits(_s);

  delete [] _s;

//  s = _fieldPtr->getComponentsDescriptions();
//  for (int i=0; i<nc; i++)
//    _s[i] = s[i];
//  F->setComponentsDescriptions(_s);
  setIterationNumber(_fieldPtr->getIterationNumber());
  setTime(_fieldPtr->getTime());
  setOrderNumber(_fieldPtr->getOrderNumber());
  fillCopy();
}

template<class T1,class T2>
void FIELDClient<T1,T2>::fillCopy()
{
  //setValueType(typeChamps); WARNING TO DO.....
  //setValueType(_fieldPtr->getValueType());
  long n;
  T1 *v = (T1 *)ReceiverFactory::getValue(_fieldPtr->getSenderForValue(MED_FULL_INTERLACE),n);
  MEDARRAY<T1> * M = new MEDARRAY<T1>(v, FIELD<T1>::getNumberOfComponents(),FIELD<T1>::getNumberOfValues(),MED_FULL_INTERLACE,true,true);
  setValue(M);
}

template<class T1,class T2>
FIELDClient<T1,T2>::~FIELDClient()
{
  _fieldPtr->release();
  CORBA::release(_fieldPtr);
  if(_ownSupport)
    delete FIELD<T1>::_support;
}

