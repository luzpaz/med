%{
#include <stdio.h>
%}

%typemap(python,in) string * {
  /* typemap in for string * */
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

//CCRT%typemap(python,in) int *
%typemap(python,in) med_int *
{
  /* typemap in for med_int * */
  /* Check if is a list */
  if (PyList_Check($input)) { 
    int size = PyList_Size($input);
    int i = 0; 
//CCRT    $1 = (int *) malloc(size*sizeof(int));
    $1 = (med_int *) malloc(size*sizeof(med_int));
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

//CCRT%typemap(freearg) int * {
%typemap(freearg) med_int * {
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

