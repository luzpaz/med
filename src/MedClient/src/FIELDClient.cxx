template<class T>
FIELDClient<T>::FIELDClient(typename MapCppFieldServ<T>::FieldPtrType ptrCorba,MEDMEM::SUPPORT * S):_fieldPtr(MapCppFieldServ<T>::FieldGlobalType::_duplicate(ptrCorba)),_refCounter(1)
{
  if (!S) 
    MEDMEM::FIELD<T>::_support=new MEDMEM::SUPPORTClient(_fieldPtr->getSupport());
  else
    MEDMEM::FIELD<T>::setSupport(S);
  
  setName(_fieldPtr->getName());

  MEDMEM::FIELD<T>::setDescription(_fieldPtr->getDescription());
  int nc = _fieldPtr->getNumberOfComponents();
  MEDMEM::FIELD<T>::setNumberOfComponents(nc);

  MEDMEM::FIELD<T>::setNumberOfValues( MEDMEM::FIELD<T>::_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  string * _s = new string[nc];

  SALOME_MED::string_array_var s;
  s = _fieldPtr->getComponentsNames();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T>::setComponentsNames(_s);

  s = _fieldPtr->getComponentsDescriptions();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T>::setComponentsDescriptions(_s);

  s = _fieldPtr->getComponentsUnits();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T>::setMEDComponentsUnits(_s);

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

template<class T>
void FIELDClient<T>::fillCopy()
{
  //setValueType(typeChamps); WARNING TO DO.....
  //setValueType(_fieldPtr->getValueType());
  long n;
  typename mapCppSender<T>::SenderVarType sender=_fieldPtr->getSenderForValue(MED_EN::MED_FULL_INTERLACE);
  T *v = (T *)ReceiverFactory::getValue(sender,n);
  MEDMEM::MEDARRAY<T> * M = new MEDMEM::MEDARRAY<T>(v, MEDMEM::FIELD<T>::getNumberOfComponents(),MEDMEM::FIELD<T>::getNumberOfValues(),MED_EN::MED_FULL_INTERLACE,true,true);
  setValue(M);
}

template<class T>
FIELDClient<T>::~FIELDClient()
{
  _fieldPtr->release();
  CORBA::release(_fieldPtr);
  if(_support)
    _support->removeReference();
}

