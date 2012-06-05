// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef MEDMEM_SWIG_TEMPLATES_HXX_
#define MEDMEM_SWIG_TEMPLATES_HXX_

#include "MEDMEM_Exception.hxx"

#ifdef WITH_NUMPY
#include <numpy/arrayobject.h>
#endif

template<class T>
  struct Binding {
//     static T Checker(PyObject *a);
//     static T Traducer(PyObject *a);
  };

template<>
  struct Binding<double> {
    //const char *TypeBuild="d";
    static int Checker(PyObject *a)  { return PyFloat_Check(a); }
    static double Traducer(PyObject *a) { return PyFloat_AsDouble(a); }
    static PyObject * Traducer( double value ) { return Py_BuildValue("d", value ); }
    static double Functor(PyObject *func, double value)
  { return Traducer( PyObject_CallFunction( func, (char *)"f", value )); }
#ifdef WITH_NUMPY
  static NPY_TYPES numpy_type() { return NPY_DOUBLE; }
#endif
  };

template<>
  struct Binding<int> {
    //const char *TypeBuild="i";
    static int Checker(PyObject *a) { return PyInt_Check(a); }
    static int Traducer(PyObject *a) { return (int) PyInt_AsLong(a); }
    static PyObject * Traducer( int value ) { return Py_BuildValue("i", value ); }
    static int Functor(PyObject *func, int value)
  { return Traducer( PyObject_CallFunction( func, (char *)"i", value )); }
#ifdef WITH_NUMPY
  static NPY_TYPES numpy_type() { return NPY_INT; }
#endif
  };

template<class T, class U>
  class MyFunction {
  public:
    static PyObject *_pyFunc;
    static int _nbOfComponent;
    static int _spaceDim;
    static void EvalPy2Cpp(const U *coord, T* outputValues)
      {
        int i=0,err;
        PyObject * tuple=PyTuple_New(_spaceDim);
          for(i=0;i<_spaceDim;i++)
            {
              err=PyTuple_SetItem(tuple,i,Binding<U>::Traducer(coord[i]));
              if (err != 0)
                throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldDoubleFromAnalytic");
            }
          PyObject * function_ret = PyObject_CallObject(_pyFunc,tuple);
          if ( !function_ret )
          {
            throw MEDMEM::MEDEXCEPTION(MEDMEM::STRING("Internal Error in createFieldIntFromAnalytic : the call to the user callable fonction has failed (possibly wrong nb of arguments that must be equal to space dimension = ")<< _spaceDim << ")");
          }
          err = PyList_Check(function_ret);
          if (!err)
              {
                Py_DECREF(function_ret);
                throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldIntFromAnalytic : the call to the user callable fonction has failed (its return value must be a list");
              }
          int size=PyList_Size(function_ret);
          if (size!=_nbOfComponent)
            {
              Py_DECREF(function_ret);
              throw MEDMEM::MEDEXCEPTION(MEDMEM::STRING("Internal Error in createFieldIntFromAnalytic : the call to the user callable fonction has failed (its return value must be a list of size equal to _nbOfComponent = ") << _nbOfComponent << ")");
            }
          for(i=0;i<_nbOfComponent;i++)
            {
              PyObject * tmp=PyList_GetItem(function_ret,i);
              err = Binding<T>::Checker(tmp);
              if (!err)
                  {
                    Py_DECREF(function_ret);
                    throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldDoubleFromAnalytic : the call to the user callable fonction has failed (check its return value type)");
                  }
              outputValues[i]=Binding<T>::Traducer(tmp);
            }
      }
  };

template<class T, class U>
PyObject *MyFunction<T,U>::_pyFunc=0;

template<class T, class U>
int MyFunction<T,U>::_nbOfComponent=0;

template<class T, class U>
int MyFunction<T,U>::_spaceDim=0;

#endif
