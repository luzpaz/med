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

#include <limits>

static PyObject* convertMEDCalculatorDBField(ParaMEDMEM::MEDCalculatorDBField *f, int owner)
{
  PyObject *ret=0;
  if(dynamic_cast<ParaMEDMEM::MEDCalculatorDBFieldReal *>(f))
    ret=SWIG_NewPointerObj((void*)f,SWIGTYPE_p_ParaMEDMEM__MEDCalculatorDBFieldReal,owner);
  if(dynamic_cast<ParaMEDMEM::MEDCalculatorDBFieldCst *>(f))
    ret=SWIG_NewPointerObj((void*)f,SWIGTYPE_p_ParaMEDMEM__MEDCalculatorDBFieldCst,owner);
  return ret;
}

void convertPyObjToRS(PyObject *o, ParaMEDMEM::MEDCalculatorDBRangeSelection& rs)
{
  if(PyInt_Check(o))
    {
      int val=(int)PyInt_AS_LONG(o);
      rs=val;
      return ;
    }
  if(PyString_Check(o))
    {
      char *s=PyString_AsString(o);
      rs=s;
      return ;
    }
  PyErr_SetString(PyExc_TypeError,"Range selection must be an integer or a string !");
}

void convertPyObjToRS2(PyObject *o, ParaMEDMEM::MEDCalculatorDBRangeSelection& rs, const char *msg)
{
  if(PyInt_Check(o))
    {
      int val=(int)PyInt_AS_LONG(o);
      rs=val;
      return ;
    }
  if(!PySlice_Check(o))
    {
      std::ostringstream oss;
      oss << "Expecting a slice or an integer for " << msg << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  Py_ssize_t strt,stp,step;
  PySliceObject *oC=reinterpret_cast<PySliceObject *>(o);
  PySlice_GetIndices(oC,std::numeric_limits<int>::max(),&strt,&stp,&step);
  rs.setPyStart(strt);
  rs.setPyEnd(stp);
}
