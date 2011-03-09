//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

%module MEDCalculator

%include std_vector.i
%include std_string.i

%include "MEDLoader.i"

%{
#include "MEDCalculatorBrowserLiteStruct.hxx"
#include "MEDCalculatorBrowserField.hxx"
#include "MEDCalculatorDBField.hxx"
#include "MEDCalculatorTypemaps.i"

using namespace ParaMEDMEM;
using namespace INTERP_KERNEL;
%}

%typemap(out) ParaMEDMEM::MEDCalculatorDBField*
{
  $result=convertMEDCalculatorDBField($1,$owner);
}

%newobject ParaMEDMEM::MEDCalculatorDBField::operator+;
%newobject ParaMEDMEM::MEDCalculatorDBField::operator-;
%newobject ParaMEDMEM::MEDCalculatorDBField::operator*;
%newobject ParaMEDMEM::MEDCalculatorDBField::operator/;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::__radd__;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::__rsub__;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::__rmul__;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::__rdiv__;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::operator();
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::buildCstFieldFromThis;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::dot;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::operator^;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::crossProduct;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::doublyContractedProduct;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::determinant;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::eigenValues;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::eigenVectors;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::inverse;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::trace;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::deviator;
%newobject ParaMEDMEM::MEDCalculatorDBFieldReal::magnitude;

%feature("unref") MEDCalculatorDBField "$this->decrRef();"

namespace ParaMEDMEM
{
  class MEDCalculatorBrowserLiteStruct
  {
  public :
    MEDCalculatorBrowserLiteStruct(const char *);
    const MEDCalculatorBrowserField& getField(int) const;
  };

  class MEDCalculatorDBField : public RefCountObject
    {
    public:
      virtual MEDCalculatorDBField *operator+(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
      MEDCalculatorDBField *operator+(double val) const throw(INTERP_KERNEL::Exception);
      virtual MEDCalculatorDBField *operator-(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
      MEDCalculatorDBField *operator-(double val) const throw(INTERP_KERNEL::Exception);
      virtual MEDCalculatorDBField *operator*(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
      MEDCalculatorDBField *operator*(double val) const throw(INTERP_KERNEL::Exception);
      virtual MEDCalculatorDBField *operator/(const MEDCalculatorDBField& other) const throw(INTERP_KERNEL::Exception);
      MEDCalculatorDBField *operator/(double val) const throw(INTERP_KERNEL::Exception);
      virtual bool isEqual(const MEDCalculatorDBField& other, double precM, double precF) const;
    };

  class MEDCalculatorDBFieldReal : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldReal(const MEDCalculatorBrowserField& ls);
    ~MEDCalculatorDBFieldReal();
    MEDCalculatorDBFieldReal *buildCstFieldFromThis(double val) const;
    ParaMEDMEM::TypeOfField getType() const;
    int getNumberOfSteps() const;
    int getNumberOfFetchedSteps() const;
    int getNumberOfComponents() const;
    void setName(const char *name);
    void setDescription(const char *descr);
    std::string getInfoOnComponent(int i) const throw(INTERP_KERNEL::Exception);
    void setInfoOnComponent(int i, const char *info) throw(INTERP_KERNEL::Exception);
    void write(const char *fName, bool writeFromScratch) const throw(INTERP_KERNEL::Exception);
    void applyFunc(const char *func) throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *operator^(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *dot(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *crossProduct(const MEDCalculatorDBFieldReal& other) const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *doublyContractedProduct() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *determinant() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *eigenValues() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *eigenVectors() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *inverse() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *trace() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *deviator() const throw(INTERP_KERNEL::Exception);
    MEDCalculatorDBField *magnitude() const throw(INTERP_KERNEL::Exception);
    %extend
       {
         MEDCalculatorDBField *__radd__(double val)
         {
           MEDCalculatorDBFieldCst tmp(val);
           return tmp+(*self);
         }

         MEDCalculatorDBField *__rsub__(double val)
         {
           MEDCalculatorDBFieldCst tmp(val);
           return tmp-(*self);
         }

         MEDCalculatorDBField *__rmul__(double val)
         {
           MEDCalculatorDBFieldCst tmp(val);
           return tmp*(*self);
         }

         MEDCalculatorDBField *__rdiv__(double val)
         {
           MEDCalculatorDBFieldCst tmp(val);
           return tmp/(*self);
         }

         MEDCalculatorDBFieldReal *operator()(PyObject *t, PyObject *p, PyObject *c) throw(INTERP_KERNEL::Exception)
         {
           MEDCalculatorDBRangeSelection tr,pr,cr;
           convertPyObjToRS(t,tr);
           convertPyObjToRS(p,pr);
           convertPyObjToRS(c,cr);
           return self->operator()(tr,pr,cr);
         }

         const MEDCalculatorDBFieldReal& assign(const MEDCalculatorDBFieldReal& other) throw(INTERP_KERNEL::Exception)
         {
           (*self)=other;
           return *self;
         }

         const MEDCalculatorDBFieldReal& assign(const MEDCalculatorDBFieldReal& other, const char *name) throw(INTERP_KERNEL::Exception)
         {
           (*self)=other;
           self->setName(name);
           return *self;
         }

         const MEDCalculatorDBFieldReal& assign(double val) throw(INTERP_KERNEL::Exception)
         {
           (*self)=val;
           return *self;
         }

         const MEDCalculatorDBFieldReal& assign(double val, const char *name) throw(INTERP_KERNEL::Exception)
         {
           (*self)=val;
           self->setName(name);
           return *self;
         }

         const MEDCalculatorDBFieldReal& assign(int val) throw(INTERP_KERNEL::Exception)
         {
           (*self)=(double) val;
           return *self;
         }

         const MEDCalculatorDBFieldReal& assign(int val, const char *name) throw(INTERP_KERNEL::Exception)
         {
           (*self)=(double) val;
           self->setName(name);
           return *self;
         }

         PyObject *getFields() const throw(INTERP_KERNEL::Exception)
         {
           std::vector<MEDCouplingFieldDouble *> r=self->getFields();
           PyObject *ret = PyList_New(r.size());
           for(unsigned int i=0;i<r.size();i++)
             {
               MEDCouplingFieldDouble *tmp=r[i];
               PyList_SetItem(ret,i,SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble, SWIG_POINTER_OWN | 0 ));
             }
           return ret;
         }

         PyObject *getValues() const throw(INTERP_KERNEL::Exception)
         {
           std::vector<MEDCouplingFieldDouble *> r=self->getFields();
           PyObject *ret = PyList_New(r.size());
           for(unsigned int i=0;i<r.size();i++)
             {
               MEDCouplingFieldDouble *tmp=r[i];
               const double *valtmp=tmp->getArray()->getConstPointer();
               int nbvals=tmp->getNumberOfValues();
               PyObject *ret2=PyList_New(nbvals);
               for(int j=0;j<nbvals;j++)
                 PyList_SetItem(ret2,j,PyFloat_FromDouble(valtmp[j]));
               PyList_SetItem(ret,i,ret2);
               tmp->decrRef();
             }
           return ret;
         }

         std::string __str__() const
         {
           return self->simpleRepr();
         }
       }
  };

  class MEDCalculatorDBFieldCst : public MEDCalculatorDBField
  {
  public:
    MEDCalculatorDBFieldCst(double val);
    MEDCalculatorDBFieldCst(int val);
    %extend
    {
      std::string __str__() const
        {
          std::ostringstream oss; oss << self->getValue();
          return oss.str();
        }
    }
  };
}
