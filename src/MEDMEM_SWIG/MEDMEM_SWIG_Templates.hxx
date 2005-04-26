#ifndef MEDMEM_SWIG_TEMPLATES_HXX_
#define MEDMEM_SWIG_TEMPLATES_HXX_

#include "MEDMEM_Exception.hxx"

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
  };

template<>
  struct Binding<int> {
    //static const char *TypeBuild="i";
    static int Checker(PyObject *a) { return PyInt_Check(a); }
    static int Traducer(PyObject *a) { return (int) PyInt_AsLong(a); }
  };

template<class T>
  class MyFunction {
  public:
    static PyObject *_pyFunc;
    static int _nbOfComponent;
    static int _spaceDim;
    static void EvalPy2Cpp(const double *coord, T* outputValues)
      {
	int i=0,err;
	PyObject * tuple=PyTuple_New(_spaceDim);
	  for(i=0;i<_spaceDim;i++)
	    {
	      err=PyTuple_SetItem(tuple,i,Py_BuildValue("d",coord[i]));
	      if (err != 0)
		throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldDoubleFromAnalytic");
	    }
	  PyObject * function_ret = PyObject_CallObject(_pyFunc,tuple);
	  err = PyList_Check(function_ret);
	  if (!err)
	      {
		Py_DECREF(function_ret);
		throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldIntFromAnalytic : the call to the user callable fonction has failed (check its API list of integer fonct (double, double))");
	      }
	  int size=PyList_Size(function_ret);
	  if (size!=_nbOfComponent)
	    {
	      Py_DECREF(function_ret);
	      throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldIntFromAnalytic : the call to the user callable fonction has failed (check its API list of integer fonct (double, double), especially the size of the returned list)");
	    }
	  for(i=0;i<_spaceDim;i++)
	    {
	      PyObject * tmp=PyList_GetItem(function_ret,i);
	      err = Binding<T>::Checker(tmp);
	      if (!err)
		  {
		    Py_DECREF(function_ret);
		    throw MEDMEM::MEDEXCEPTION("Internal Error in createFieldDoubleFromAnalytic : the call to the user callable fonction has failed (check its API list of double fonct (double, double), especially the type of the returned list)");
		  }
	      outputValues[i]=Binding<T>::Traducer(tmp);
	    }
      }
  };

template<class T>
PyObject *MyFunction<T>::_pyFunc=0;

template<class T>
int MyFunction<T>::_nbOfComponent=0;

template<class T>
int MyFunction<T>::_spaceDim=0;

#endif
