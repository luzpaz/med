//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

%include "MEDCoupling.i"

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
