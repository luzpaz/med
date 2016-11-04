// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
#include "MEDCoupling1SGTUMeshServant.hxx"
#include "MEDCoupling1DGTUMeshServant.hxx"
#include "MEDCouplingExtrudedMeshServant.hxx"
#include "MEDCouplingCMeshServant.hxx"
#include "MEDCouplingIMeshServant.hxx"
#include "MEDCouplingCurveLinearMeshServant.hxx"
#include "DataArrayDoubleServant.hxx"
#include "DataArrayIntServant.hxx"
#include "MEDCouplingCorbaTypemaps.i"

using namespace MEDCoupling;
%}

%include "MEDCouplingCommon.i"

namespace MEDCoupling
{
  class MEDCouplingFieldDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldDoubleServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingFieldDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldTemplateServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldTemplate *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldTemplateServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingFieldTemplate *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldTemplateServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingMultiFieldsServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMultiFields *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingMultiFieldsServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingMultiFields *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingMultiFieldsServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingFieldOverTimeServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingFieldOverTime *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingFieldOverTimeServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingFieldOverTime *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingFieldOverTimeServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           if(!cppPointerOfMesh)
             throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this : input pointer is NULL !");
           if(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingUMeshServant>(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCoupling1SGTUMeshServant>(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCoupling1DGTUMeshServant>(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingExtrudedMeshServant>(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingCMeshServant>(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingIMeshServant>(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh))
             return buildServantAndActivate<MEDCouplingCurveLinearMeshServant>(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh));
           throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this : unrecognized type (or not managed type) of Mesh in input !");
         }

         static PyObject *_this2(const MEDCouplingMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           if(!cppPointerOfMesh)
             throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this2 : input pointer is NULL !");
           if(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingUMeshServant>(dynamic_cast<const MEDCouplingUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCoupling1SGTUMeshServant>(dynamic_cast<const MEDCoupling1SGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCoupling1DGTUMeshServant>(dynamic_cast<const MEDCoupling1DGTUMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingExtrudedMeshServant>(dynamic_cast<const MEDCouplingMappedExtrudedMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingCMeshServant>(dynamic_cast<const MEDCouplingCMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingIMeshServant>(dynamic_cast<const MEDCouplingIMesh *>(cppPointerOfMesh));
           if(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh))
             return buildServantAndActivate2<MEDCouplingCurveLinearMeshServant>(dynamic_cast<const MEDCouplingCurveLinearMesh *>(cppPointerOfMesh));
           throw INTERP_KERNEL::Exception("MEDCouplingMeshServant::_this2 : unrecognized type (or not managed type) of Mesh in input !");
         }
       }
  };

  class MEDCouplingUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCoupling1SGTUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCoupling1SGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCoupling1SGTUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCoupling1SGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCoupling1SGTUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCoupling1DGTUMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCoupling1DGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCoupling1DGTUMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCoupling1DGTUMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCoupling1DGTUMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingExtrudedMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingMappedExtrudedMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingExtrudedMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingMappedExtrudedMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingExtrudedMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingCMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingCMeshServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingCMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingCMeshServant>(cppPointerOfMesh);
         }
       }
  };
  
  class MEDCouplingIMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingIMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingIMeshServant>(cppPointerOfMesh);
         }
         
         static PyObject *_this2(const MEDCouplingIMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingIMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class MEDCouplingCurveLinearMeshServant
  {
  public:
    %extend
       {
         static PyObject *_this(const MEDCouplingCurveLinearMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<MEDCouplingCurveLinearMeshServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const MEDCouplingCurveLinearMesh *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<MEDCouplingCurveLinearMeshServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayDoubleServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<DataArrayDoubleServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const DataArrayDouble *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<DataArrayDoubleServant>(cppPointerOfMesh);
         }
       }
  };

  class DataArrayIntServant
  {
  public:
    %extend
       {
         static PyObject *_this(const DataArrayInt *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate<DataArrayIntServant>(cppPointerOfMesh);
         }

         static PyObject *_this2(const DataArrayInt *cppPointerOfMesh) throw(INTERP_KERNEL::Exception)
         {
           return buildServantAndActivate2<DataArrayIntServant>(cppPointerOfMesh);
         }
       }
  };
}

%pythoncode %{
def MEDCouplingDataArrayDoublenew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____new___(cls,args)
def MEDCouplingDataArrayDoubleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____idiv___(self, self, *args)
def MEDCouplingDataArrayDoubleIpow(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDouble____ipow___(self, self, *args)
def MEDCouplingFieldDoublenew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____new___(cls,args)
def MEDCouplingFieldDoubleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____iadd___(self, self, *args)
def MEDCouplingFieldDoubleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____isub___(self, self, *args)
def MEDCouplingFieldDoubleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____imul___(self, self, *args)
def MEDCouplingFieldDoubleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____idiv___(self, self, *args)
def MEDCouplingFieldDoubleIpow(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingFieldDouble____ipow___(self, self, *args)
def MEDCouplingDataArrayIntnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____new___(cls,args)
def MEDCouplingDataArrayIntIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____iadd___(self, self, *args)
def MEDCouplingDataArrayIntIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____isub___(self, self, *args)
def MEDCouplingDataArrayIntImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____imul___(self, self, *args)
def MEDCouplingDataArrayIntIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____idiv___(self, self, *args)
def MEDCouplingDataArrayIntImod(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____imod___(self, self, *args)
def MEDCouplingDataArrayIntIpow(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayInt____ipow___(self, self, *args)
def MEDCouplingDataArrayBytenew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayByte____new___(cls,args)
def MEDCouplingDataArrayDoubleTupleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____isub___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____imul___(self, self, *args)
def MEDCouplingDataArrayDoubleTupleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayDoubleTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayIntTupleIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____iadd___(self, self, *args)
def MEDCouplingDataArrayIntTupleIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____isub___(self, self, *args)
def MEDCouplingDataArrayIntTupleImul(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____imul___(self, self, *args)
def MEDCouplingDataArrayIntTupleIdiv(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____idiv___(self, self, *args)
def MEDCouplingDataArrayIntTupleImod(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DataArrayIntTuple____imod___(self, self, *args)
def ParaMEDMEMDenseMatrixIadd(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DenseMatrix____iadd___(self, self, *args)
def ParaMEDMEMDenseMatrixIsub(self,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.DenseMatrix____isub___(self, self, *args)
def MEDCouplingUMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingUMesh____new___(cls,args)
def MEDCoupling1DGTUMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCoupling1DGTUMesh____new___(cls,args)
def MEDCoupling1SGTUMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCoupling1SGTUMesh____new___(cls,args)
def MEDCouplingCurveLinearMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingCurveLinearMesh____new___(cls,args)
def MEDCouplingCMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingCMesh____new___(cls,args)
def MEDCouplingIMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingIMesh____new___(cls,args)
def MEDCouplingExtrudedMeshnew(cls,*args):
    import _MEDCouplingCorba
    return _MEDCouplingCorba.MEDCouplingMappedExtrudedMesh____new___(cls,args)
%}

%include "MEDCouplingFinalize.i"
