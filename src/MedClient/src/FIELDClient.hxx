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
#ifndef _FIELDCLIENT_HXX
#define _FIELDCLIENT_HXX

#include <SALOMEconfig.h>
#include <utilities.h>
#include "MEDMEM_Field.hxx"
#include "SUPPORTClient.hxx"
#include "ReceiverFactory.hxx"
#include "SenderFactory.hxx"
#include CORBA_CLIENT_HEADER(MED)

template<class T>
struct MapCppFieldServ {
  typedef T FieldPtrType;
  typedef T FieldGlobalType;
};

template<>
struct MapCppFieldServ<int> {
  typedef SALOME_MED::FIELDINT_ptr FieldPtrType;
  typedef SALOME_MED::FIELDINT FieldGlobalType;
};

template<>
struct MapCppFieldServ<double> {
  typedef SALOME_MED::FIELDDOUBLE_ptr FieldPtrType;
  typedef SALOME_MED::FIELDDOUBLE FieldGlobalType;
};

namespace MEDMEM{

template<class T>
class FIELDClient : public MEDMEM::FIELD<T>
{
private:
  typename MapCppFieldServ<T>::FieldPtrType _fieldPtr;
  int _refCounter;
public:
  FIELDClient(typename MapCppFieldServ<T>::FieldPtrType ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
private:
  void fillCopy();
};

#include "FIELDClient.cxx"

}
#endif
