template<class T1,class T2>
FIELDClient<T1,T2>::FIELDClient(typename T2::_ptr_type ptrCorba,MEDMEM::SUPPORT * S):_fieldPtr(T2::_duplicate(ptrCorba)),_refCounter(1)
{
  if (!S) 
    MEDMEM::FIELD<T1>::_support=new MEDMEM::SUPPORTClient(_fieldPtr->getSupport());
  else
    MEDMEM::FIELD<T1>::setSupport(S);
  
  setName(_fieldPtr->getName());

  MEDMEM::FIELD<T1>::setDescription(_fieldPtr->getDescription());
  int nc = _fieldPtr->getNumberOfComponents();
  MEDMEM::FIELD<T1>::setNumberOfComponents(nc);

  MEDMEM::FIELD<T1>::setNumberOfValues( MEDMEM::FIELD<T1>::_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  string * _s = new string[nc];

  SALOME_MED::string_array_var s;
  s = _fieldPtr->getComponentsNames();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T1>::setComponentsNames(_s);

  s = _fieldPtr->getComponentsDescriptions();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T1>::setComponentsDescriptions(_s);

  s = _fieldPtr->getComponentsUnits();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T1>::setMEDComponentsUnits(_s);

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
  T1 *v = (T1 *)ReceiverFactory::getValue(_fieldPtr->getSenderForValue(MED_EN::MED_FULL_INTERLACE),n);
  MEDMEM::MEDARRAY<T1> * M = new MEDMEM::MEDARRAY<T1>(v, MEDMEM::FIELD<T1>::getNumberOfComponents(),MEDMEM::FIELD<T1>::getNumberOfValues(),MED_EN::MED_FULL_INTERLACE,true,true);
  setValue(M);
}

template<class T1,class T2>
FIELDClient<T1,T2>::~FIELDClient()
{
  _fieldPtr->release();
  CORBA::release(_fieldPtr);
  if(_support)
    _support->removeReference();
}

