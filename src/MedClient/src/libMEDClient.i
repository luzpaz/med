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

class MESHClient : public MESH {

 public:
  
  MESHClient(const SALOME_MED::MESH_ptr m);
  virtual ~MESHClient();

  void blankCopy();
  void fillCopy();

};

class SUPPORTClient : public SUPPORT {

 public:

  SUPPORTClient(const SALOME_MED::SUPPORT_ptr S, 
		MESH * M = NULL);
  virtual ~SUPPORTClient();

  void blankCopy();
  void fillCopy();

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
