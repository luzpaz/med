%module libMEDClient

%{
#include "MESHClient.hxx"
#include "SUPPORTClient.hxx"
#include "FIELDClient.hxx"
#include CORBA_CLIENT_HEADER(MED)
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
  ~SUPPORTClient();

  void blankCopy();
  void fillCopy();

};

template<typename T>
class FIELDClient : public FIELD<T> {

 public:

  FIELDClient(const SALOME_MED::FIELD_ptr S, 
		MESH * M = NULL);
  ~FIELDClient();

  void blankCopy();
  void fillCopy();

};
