template<class T, class INTERLACING_TAG>
FIELDClient<T,INTERLACING_TAG>::FIELDClient(typename FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldCorbaPtrType ptrCorba,MEDMEM::SUPPORT * S):_fieldPtr(FIELDI_TRAITS<T,INTERLACING_TAG>::SimpleFieldGlobalType::_duplicate(ptrCorba)),_refCounter(1)
{
  if (!S) 
    {
      SCRUTE(_fieldPtr);
      SCRUTE(_fieldPtr->getSupport());
      MEDMEM::FIELD<T,INTERLACING_TAG>::_support=new MEDMEM::SUPPORTClient(_fieldPtr->getSupport());
    }
  else
    MEDMEM::FIELD<T,INTERLACING_TAG>::setSupport(S);
  
  setName(_fieldPtr->getName());

  MEDMEM::FIELD<T,INTERLACING_TAG>::setDescription(_fieldPtr->getDescription());
  int nc = _fieldPtr->getNumberOfComponents();
  MEDMEM::FIELD<T,INTERLACING_TAG>::setNumberOfComponents(nc);

  MEDMEM::FIELD<T,INTERLACING_TAG>::setNumberOfValues( MEDMEM::FIELD<T,INTERLACING_TAG>::_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

  string * _s = new string[nc];

  SALOME_MED::string_array_var s;
  s = _fieldPtr->getComponentsNames();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T,INTERLACING_TAG>::setComponentsNames(_s);

  s = _fieldPtr->getComponentsDescriptions();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T,INTERLACING_TAG>::setComponentsDescriptions(_s);

  s = _fieldPtr->getComponentsUnits();
  for (int i=0; i<nc; i++)
    _s[i] = s[i];
  MEDMEM::FIELD<T,INTERLACING_TAG>::setMEDComponentsUnits(_s);

  delete [] _s;
  setIterationNumber(_fieldPtr->getIterationNumber());
  setTime(_fieldPtr->getTime());
  setOrderNumber(_fieldPtr->getOrderNumber());
  fillCopy();
}

template<class T, class INTERLACING_TAG>
void FIELDClient<T,INTERLACING_TAG>::fillCopy()
{
  long n;
  typename FIELDI_TRAITS<T,INTERLACING_TAG>::SenderVarType sender=_fieldPtr->getSenderForValue(MEDMEM::FIELD<T,INTERLACING_TAG>::_interlacingType);
  T *v = (T *)ReceiverFactory::getValue(sender,n);
  MEDMEM::MEDMEM_Array_ * array;
  if(_fieldPtr->getGaussPresence())
    array=0;
  //array=new typename MEDMEM::MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array
  //    (v, MEDMEM::FIELD<T,INTERLACING_TAG>::getNumberOfComponents(),MEDMEM::FIELD<T,INTERLACING_TAG>::getNumberOfValues(),true,true);
  else
    array=new typename MEDMEM::MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array
      (v, MEDMEM::FIELD<T,INTERLACING_TAG>::getNumberOfComponents(),MEDMEM::FIELD<T,INTERLACING_TAG>::getNumberOfValues(),true,true);
  MEDMEM::FIELD<T,INTERLACING_TAG>::setArray(array);
}

template<class T, class INTERLACING_TAG>
FIELDClient<T,INTERLACING_TAG>::~FIELDClient()
{
  _fieldPtr->Destroy();
  CORBA::release(_fieldPtr);
  if(FIELD<T,INTERLACING_TAG>::_support)
    FIELD<T,INTERLACING_TAG>::_support->removeReference();
}

