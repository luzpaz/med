//  MED MEDMEM_SWIG : binding of C++ implementation and Python
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : my_typemap.i
//  Module : MED

%{
#include <stdio.h>
%}

%typemap(python,in) string * {
  /* Check if is a list */
  if (PyList_Check($source)) {
    int size = PyList_Size($source);
    int i = 0;
    $target = new string[size];
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($source,i);
      if (PyString_Check(o))
	$target[i] = string(PyString_AsString(PyList_GetItem($source,i)));
      else {
	PyErr_SetString(PyExc_TypeError,"list must contain strings");
	free($target);
	return NULL;
      }
    }
    // $target[i] = 0;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

%typemap(python,in) double *
{
  /* typemap in for double * */
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

%typemap(python,in) int *
{
  /* typemap in for int * */
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


%typemap(python,in) medGeometryElement *
{
  /* typemap in for medGeometryElement * */
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

%typemap(python,out) list<string> {
  int i;
  list<string>::iterator iL;

  $result = PyList_New($1->size());
  for (i=0, iL=$1->begin(); iL!=$1->end(); i++, iL++)
    PyList_SetItem($result,i,PyString_FromString((*iL).c_str())); 
}

%typemap(freearg) int * {
  /* free the memory allocated in the typemap in for int * */
  free($1);
}

%typemap(freearg) double * {
  /* free the memory allocated in the typemap in for double * */
  free($1);
}

%typemap(freearg) medGeometryElement * {
  /* free the memory allocated in the typemap in for medGeometryElement * */
  free($1);
}
