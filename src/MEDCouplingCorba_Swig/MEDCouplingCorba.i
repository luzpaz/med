// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay (CEA/DEN)

%module MEDCouplingCorba

%{
#include "MEDCouplingFieldDoubleServant.hxx"  
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingMultiFieldsServant.hxx"
#include "MEDCouplingFieldOverTimeServant.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "DataArrayIntServant.hxx"
#include "MEDCouplingCorbaTypemaps.i"

using namespace ParaMEDMEM;
%}

%include "MEDCouplingCommon.i"

namespace ParaMEDMEM
{
  class MEDCouplingFieldDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldDouble *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingFieldDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldTemplateServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldTemplate *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingFieldTemplateServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingMultiFieldsServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMultiFields *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingMultiFieldsServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldOverTimeServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldOverTime *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingFieldOverTimeServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingUMesh *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingExtrudedMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingExtrudedMesh *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingExtrudedMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingCMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCMesh *cppPointerOfMesh)
         {
           return buildServantAndActivate<MEDCouplingCMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayDouble *cppPointerOfMesh)
         {
           return buildServantAndActivate<DataArrayDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayIntServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayInt *cppPointerOfMesh)
         {
           return buildServantAndActivate<DataArrayIntServant>(cppPointerOfMesh);
         }
       }
  };
}

%pythoncode %{
def ParaMEDMEMDataArrayDoubleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____iadd___(self, self, *args)
def ParaMEDMEMDataArrayDoubleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____isub___(self, self, *args)
def ParaMEDMEMDataArrayDoubleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____imul___(self, self, *args)
def ParaMEDMEMDataArrayDoubleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____idiv___(self, self, *args)
def ParaMEDMEMMEDCouplingFieldDoubleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____iadd___(self, self, *args)
def ParaMEDMEMMEDCouplingFieldDoubleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____isub___(self, self, *args)
def ParaMEDMEMMEDCouplingFieldDoubleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____imul___(self, self, *args)
def ParaMEDMEMMEDCouplingFieldDoubleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____idiv___(self, self, *args)
def ParaMEDMEMDataArrayIntIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____iadd___(self, self, *args)
def ParaMEDMEMDataArrayIntIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____isub___(self, self, *args)
def ParaMEDMEMDataArrayIntImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____imul___(self, self, *args)
def ParaMEDMEMDataArrayIntIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____idiv___(self, self, *args)
def ParaMEDMEMDataArrayIntImod(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____imod___(self, self, *args)
def ParaMEDMEMDataArrayDoubleTupleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____iadd___(self, self, *args)
def ParaMEDMEMDataArrayDoubleTupleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____isub___(self, self, *args)
def ParaMEDMEMDataArrayDoubleTupleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____imul___(self, self, *args)
def ParaMEDMEMDataArrayDoubleTupleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____idiv___(self, self, *args)
def ParaMEDMEMDataArrayIntTupleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____iadd___(self, self, *args)
def ParaMEDMEMDataArrayIntTupleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____isub___(self, self, *args)
def ParaMEDMEMDataArrayIntTupleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____imul___(self, self, *args)
def ParaMEDMEMDataArrayIntTupleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____idiv___(self, self, *args)
def ParaMEDMEMDataArrayIntTupleImod(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____imod___(self, self, *args)
%}

%include "MEDCouplingFinalize.i"
