//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
%{
#include <stdio.h>
%}

#if defined(SWIGPYTHON)
%typemap(in) string * , const string * , const string * const {
  /* typemap in for string * , const string * , const string * const */
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = new string[size];
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
	$1[i] = string(PyString_AsString(PyList_GetItem($input,i)));
      else {
	PyErr_SetString(PyExc_TypeError,"list must contain strings");
	free($1);
	return NULL;
      }
    }
  }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}
#endif

#if defined(SWIGPYTHON)
%typemap(in) double * , const double * , const double * const
{
  /* typemap in for double * , const double * , const double * const */
  /* Check if is a list */
  if (PyList_Check($input)) { 
    int size = PyList_Size($input);
    int i = 0; 
    $1 = (double *) malloc(size*sizeof(double));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyFloat_Check(o))
	$1[i] = PyFloat_AsDouble(PyList_GetItem($input,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain floats");
	free($1);
	return NULL;
      }
    }
  } 
  else
    { 
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
} 
#endif

#if defined(SWIGPYTHON)
%typemap(in) int * , const int * , const int * const
{
  /* typemap in for int * , const int * , const int * const */
  /* Check if is a list */
  if (PyList_Check($input)) { 
    int size = PyList_Size($input);
    int i = 0; 
    $1 = (int *) malloc(size*sizeof(int));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyInt_Check(o))
	$1[i] = PyInt_AsLong(PyList_GetItem($input,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain integers");
	free($1);
	return NULL;
      }
    }
  } 
  else
    { 
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
} 
#endif

#if defined(SWIGPYTHON)
%typemap(in) medGeometryElement * , const  medGeometryElement * , const  medGeometryElement * const 
{
  /* typemap in for medGeometryElement * , const  medGeometryElement * , const  medGeometryElement * const */
  /* Check if is a list */
  if (PyList_Check($input)) { 
    int size = PyList_Size($input);
    int i = 0; 
    $1 = (medGeometryElement *) malloc(size*sizeof(int));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyInt_Check(o))
	$1[i] = (medGeometryElement) PyInt_AsLong(PyList_GetItem($input,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain integers");
	free($1);
	return NULL;
      }
    }
  } 
  else
    { 
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
} 
#endif

#if defined(SWIGPYTHON)
%typemap(out) list<string> {
  int i;
  list<string>::iterator iL;

  $result = PyList_New($1->size());
  for (i=0, iL=$1->begin(); iL!=$1->end(); i++, iL++)
    PyList_SetItem($result,i,PyString_FromString((*iL).c_str())); 
}
#endif

%typemap(freearg) int * , const int * , const int * const {
  /* free the memory allocated in the typemap in for int * , const int * , const int * const */
  free($1);
}

%typemap(freearg) double * , const double * , const double * const {
  /* free the memory allocated in the typemap in for double * , const double * , const double * const */
  free($1);
}

%typemap(freearg) medGeometryElement * , const medGeometryElement * , const medGeometryElement * const {
  /* free the memory allocated in the typemap in for medGeometryElement * , const medGeometryElement * , const medGeometryElement * const */
  free($1);
}
