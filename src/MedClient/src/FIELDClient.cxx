// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
template<class T>
FIELDClient<T>::FIELDClient(typename MapCppFieldServ<T>::FieldPtrType ptrCorba,MEDMEM::SUPPORT * S):_fieldPtr(MapCppFieldServ<T>::FieldGlobalType::_duplicate(ptrCorba)),_refCounter(1)
{
  if (!S) 
    {
      SCRUTE(_fieldPtr);
      SCRUTE(_fieldPtr->getSupport());
      MEDMEM::FIELD<T>::_support=new MEDMEM::SUPPORTClient(_fieldPtr->getSupport());
    }
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
  _fieldPtr->Destroy();
  CORBA::release(_fieldPtr);
  if(FIELD<T>::_support)
    FIELD<T>::_support->removeReference();
}

