%module libMEDClient

%{
#include "MESHClient.hxx"
#include "SUPPORTClient.hxx"
#include "FIELDClient.hxx"
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


class FIELDDOUBLEClient : public FIELDDOUBLE{
public:
  FIELDDOUBLEClient(const SALOME_MED::FIELDDOUBLE_ptr theField, SUPPORT *theSupport = NULL);
  FIELDDOUBLE* GetPointer();
};


class FIELDINTClient : public FIELDINT{
public:
  FIELDINTClient(const SALOME_MED::FIELDINT_ptr theField, SUPPORT *theSupport = NULL);
  FIELDINT* GetPointer();
};
