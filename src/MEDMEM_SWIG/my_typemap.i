%{
#include <stdio.h>
%}

%typemap(python,in) double *
{
  /* Check if is a list */
  if (PyList_Check($source)) { 
    int size = PyList_Size($source);
    int i = 0; 
    $target = (double *) malloc(size*sizeof(double));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($source,i);
      if (PyFloat_Check(o))
	$target[i] = PyFloat_AsDouble(PyList_GetItem($source,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain floats");
	free($target);
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
  /* Check if is a list */
  if (PyList_Check($source)) { 
    int size = PyList_Size($source);
    int i = 0; 
    $target = (int *) malloc(size*sizeof(int));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($source,i);
      if (PyInt_Check(o))
	$target[i] = PyInt_AsLong(PyList_GetItem($source,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain integers");
	free($target);
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
  /* Check if is a list */
  if (PyList_Check($source)) { 
    int size = PyList_Size($source);
    int i = 0; 
    $target = (medGeometryElement *) malloc(size*sizeof(int));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($source,i);
      if (PyInt_Check(o))
	$target[i] = (medGeometryElement) PyInt_AsLong(PyList_GetItem($source,i));
      else { 
	PyErr_SetString(PyExc_TypeError,"list must contain integers");
	free($target);
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

  $target = PyList_New($source->size());
  for (i=0, iL=$source->begin(); iL!=$source->end(); i++, iL++)
    PyList_SetItem($target,i,PyString_FromString((*iL).c_str())); 
}
