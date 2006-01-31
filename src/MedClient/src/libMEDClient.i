// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
%module libMEDClient

%{
#include "MESHClient.hxx"
#include "SUPPORTClient.hxx"
#include "FIELDClient.hxx"
#include CORBA_CLIENT_HEADER(MED)

  using namespace MEDMEM;
  using namespace MED_EN;
%}

%include "libMedCorba_Swig.i"
%include "libMEDMEM_Swig.i"

%typecheck(SWIG_TYPECHECK_POINTER) SALOME_MED::FIELDDOUBLE_ptr, SALOME_MED::FIELDINT_ptr
{
  $1 = ($input != 0);
}


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

%template (FIELDDOUBLEClient) FIELDClient<double, FullInterlace>;
%template (FIELDDOUBLENOINTERLACEClient) FIELDClient<double, NoInterlace>;
%template (FIELDINTClient) FIELDClient<int, FullInterlace>;
%template (FIELDINTNOINTERLACEClient) FIELDClient<int, NoInterlace>;

%extend FIELDClient<double, FullInterlace>
{
  %template(FIELDDOUBLEClients) FIELDClient<SALOME_MED::FIELDDOUBLE_ptr>;
};

%extend FIELDClient<double, NoInterlace>
{
  %template(FIELDDOUBLENOINTERLACEClients) FIELDClient<SALOME_MED::FIELDDOUBLE_ptr>;
};

%extend FIELDClient<int, FullInterlace>
{
  %template(FIELDINTClients) FIELDClient<SALOME_MED::FIELDINT_ptr>;
};

%extend FIELDClient<int, NoInterlace>
{
  %template(FIELDINTNOINTERLACEClients) FIELDClient<SALOME_MED::FIELDINT_ptr>;
};

FIELD<double> * getDoublePointer(FIELDClient<double,FullInterlace> * input);

FIELD<double,NoInterlace> *getDoubleNoInterlacePointer(FIELDClient<double,NoInterlace> * input);

FIELD<int> * getIntPointer(FIELDClient<int,FullInterlace> * input);

FIELD<int,NoInterlace> * getIntNoInterlacePointer(FIELDClient<int,NoInterlace> * input);

%{
  FIELD<double> * getDoublePointer(FIELDClient<double,FullInterlace> * input)
    {
      return (FIELD<double> *) input;
    }

  FIELD<double,NoInterlace> *getDoubleNoInterlacePointer(FIELDClient<double,NoInterlace> * input)
    {
      return (FIELD<double,NoInterlace> *) input;
    }

  FIELD<int> * getIntPointer(FIELDClient<int,FullInterlace> * input)
  {
      return (FIELD<int> *) input;
  }

  FIELD<int,NoInterlace> * getIntNoInterlacePointer(FIELDClient<int,NoInterlace> * input)
  {
      return (FIELD<int,NoInterlace> *) input;
  }
%}
