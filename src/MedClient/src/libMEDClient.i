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

%module libMEDClient

%{
#include "GMESHClient.hxx"
#include "MESHClient.hxx"
#include "SUPPORTClient.hxx"
#include "FIELDClient.hxx"
#include CORBA_CLIENT_HEADER(MED)

  using namespace MEDMEM;
  using namespace MED_EN;
%}

%typemap(in) MESH* {
  if ((SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, 0)) == -1) {
    MESHClient *client;
    if ((SWIG_ConvertPtr($input, (void **) &client, $descriptor(MESHClient *), 0)) == -1) {
      SWIG_Python_TypeError("MESH* or MESHClient*", $input);
      return NULL;
    }
    $1 = (MESH *) client;
  }
}

%include "libMedCorba_Swig.i"
%include "libMEDMEM_Swig.i"

%typecheck(SWIG_TYPECHECK_POINTER) SALOME_MED::FIELDDOUBLE_ptr, SALOME_MED::FIELDINT_ptr
{
  $1 = ($input != 0);
}

/*
  managing C++ exception in the Python API
*/
/*%exception
{
  class PyAllowThreadsGuard {
   public:
    PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
    ~PyAllowThreadsGuard() { PyEval_RestoreThread(_save); }
   private:
    PyThreadState *_save;
  };

  PyAllowThreadsGuard guard;

  $action
}*/

class GMESHClient : public GMESH {

 public:
  
  GMESHClient(const SALOME_MED::GMESH_ptr m);

  void blankCopy();
  void fillCopy();
  %extend {
    ~GMESHClient(){
      self->removeReference();
    }
  }
};

class MESHClient : public MESH {

 public:
  
  MESHClient(const SALOME_MED::MESH_ptr m);

  void blankCopy();
  void fillCopy();
  %extend {
    ~MESHClient(){
      self->removeReference();
    }
  }
};

MESH * getMeshPointer(MESHClient * input);
%{
  MESH * getMeshPointer(MESHClient * input)
  {
    return (MESH *) input;
  }
%}

class SUPPORTClient : public SUPPORT {

 public:

  SUPPORTClient(const SALOME_MED::SUPPORT_ptr S, 
		MESH * M = NULL);

  void blankCopy();
  void fillCopy();
  %extend {
    ~SUPPORTClient() {
      self->removeReference();
    }
  }

};

template<class T, class INTERLACING_TAG>
class FIELDClient : public FIELD<T,INTERLACING_TAG>
{
public:
  template<class U>
  FIELDClient(U ptrCorba,MEDMEM::SUPPORT * S = NULL);
  ~FIELDClient();
};

%template (FIELDDOUBLEClient)                  FIELDClient<double, FullInterlace>;
%template (FIELDDOUBLENOINTERLACEClient)       FIELDClient<double, NoInterlace>;
%template (FIELDDOUBLENOINTERLACEBYTYPEClient) FIELDClient<double, NoInterlaceByType>;
%template (FIELDINTClient)                     FIELDClient<int, FullInterlace>;
%template (FIELDINTNOINTERLACEClient)          FIELDClient<int, NoInterlace>;
%template (FIELDINTNOINTERLACEBYTYPEClient)    FIELDClient<int, NoInterlaceByType>;

%extend FIELDClient<double, FullInterlace>
{
  %template(FIELDDOUBLEClient) FIELDClient<SALOME_MED::FIELDDOUBLE_ptr>;
};

%extend FIELDClient<double, NoInterlace>
{
  %template(FIELDDOUBLENOINTERLACEClient) FIELDClient<SALOME_MED::FIELDDOUBLE_ptr>;
};

%extend FIELDClient<double, NoInterlaceByType>
{
  %template(FIELDDOUBLENOINTERLACEBYTYPEClient) FIELDClient<SALOME_MED::FIELDDOUBLE_ptr>;
};

%extend FIELDClient<int, FullInterlace>
{
  %template(FIELDINTClient) FIELDClient<SALOME_MED::FIELDINT_ptr>;
};

%extend FIELDClient<int, NoInterlace>
{
  %template(FIELDINTNOINTERLACEClient) FIELDClient<SALOME_MED::FIELDINT_ptr>;
};

%extend FIELDClient<int, NoInterlaceByType>
{
  %template(FIELDINTNOINTERLACEBYTYPEClient) FIELDClient<SALOME_MED::FIELDINT_ptr>;
};

FIELD<double> * getDoublePointer(FIELDClient<double,FullInterlace> * input);

FIELD<double,NoInterlace> *getDoubleNoInterlacePointer(FIELDClient<double,NoInterlace> * input);

FIELD<double,NoInterlaceByType> *getDoubleNoInterlaceByTypePointer(FIELDClient<double,NoInterlaceByType> * input);

FIELD<int> * getIntPointer(FIELDClient<int,FullInterlace> * input);

FIELD<int,NoInterlace> * getIntNoInterlacePointer(FIELDClient<int,NoInterlace> * input);

FIELD<int,NoInterlaceByType> * getIntNoInterlaceByTypePointer(FIELDClient<int,NoInterlaceByType> * input);

%{
  FIELD<double> * getDoublePointer(FIELDClient<double,FullInterlace> * input)
    {
      return (FIELD<double> *) input;
    }

  FIELD<double,NoInterlace> *getDoubleNoInterlacePointer(FIELDClient<double,NoInterlace> * input)
    {
      return (FIELD<double,NoInterlace> *) input;
    }

  FIELD<double,NoInterlaceByType> *getDoubleNoInterlaceByTypePointer(FIELDClient<double,NoInterlaceByType> * input)
    {
      return (FIELD<double,NoInterlaceByType> *) input;
    }

  FIELD<int> * getIntPointer(FIELDClient<int,FullInterlace> * input)
  {
      return (FIELD<int> *) input;
  }

  FIELD<int,NoInterlace> * getIntNoInterlacePointer(FIELDClient<int,NoInterlace> * input)
  {
      return (FIELD<int,NoInterlace> *) input;
  }

  FIELD<int,NoInterlaceByType> * getIntNoInterlaceByTypePointer(FIELDClient<int,NoInterlaceByType> * input)
  {
      return (FIELD<int,NoInterlaceByType> *) input;
  }
%}
