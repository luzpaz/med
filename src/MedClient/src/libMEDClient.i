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
#include "FIELDDOUBLEClient.hxx"
#include "FIELDINTClient.hxx"
#include CORBA_CLIENT_HEADER(MED)
/* #define FIELDDOUBLEClient FIELDClient<double,SALOME_MED::FIELDDOUBLE_ptr> */
/* #define FIELDINTClient FIELDClient<int,SALOME_MED::FIELDINT_ptr> */

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

class FIELDDOUBLEClient : public FIELDDOUBLE {
public:
  FIELDDOUBLEClient(SALOME_MED::FIELDDOUBLE_ptr ptrCorba,
		    MEDMEM::SUPPORT * S = NULL);

  ~FIELDDOUBLEClient();
};

class FIELDINTClient : public FIELDINT {
public:
  FIELDINTClient(SALOME_MED::FIELDINT_ptr ptrCorba,
		 MEDMEM::SUPPORT * S = NULL);

  ~FIELDINTClient();
};

FIELDDOUBLE * getDoublePointer(FIELDDOUBLEClient * input);

FIELDINT * getIntPointer(FIELDINTClient * input);

%{
  FIELDDOUBLE * getDoublePointer(FIELDDOUBLEClient * input)
    {
      return (FIELDDOUBLE *) input;
    }

  FIELDINT * getIntPointer(FIELDINTClient * input)
  {
      return (FIELDINT *) input;
  }
%}
