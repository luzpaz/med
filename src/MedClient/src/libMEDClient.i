%module libMEDClient

%{
#include "MESHClient.hxx"
#include "SUPPORTClient.hxx"
#include "FIELDClient.hxx"
#include CORBA_CLIENT_HEADER(MED)
#define FIELDDOUBLEClient FIELDClient<double>
//CCRT#define FIELDINTClient FIELDClient<int>
#define FIELDINTClient FIELDClient<med_int>
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

%rename(FIELDDOUBLEClient) FIELDClient<double>;
FIELDDOUBLE * FIELDDOUBLEClient(const SALOME_MED::FIELDDOUBLE_ptr IOR_Field, 
				SUPPORT * S = NULL);

//CCRT%rename(FIELDINTClient) FIELDClient<int>;
%rename(FIELDINTClient) FIELDClient<med_int>;
FIELDDOUBLE * FIELDINTClient   (const SALOME_MED::FIELDINT_ptr IOR_Field, 
				SUPPORT * S = NULL);
