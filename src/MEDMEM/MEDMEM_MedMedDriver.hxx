#ifndef MED_MED_DRIVER_HXX
#define MED_MED_DRIVER_HXX

#include <string>
#include "MEDMEM_STRING.hxx"
#include "utilities.h"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_define.hxx"

//using namaspace MED_FR ;

class MESH;
class FIELD_;
class MED;

// This driver pilots within a MED class read/write accesses of fields/meshes
class MED_MED_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMed;         // Store 'MED_DRIVER (0..n)----(1) MED' associations
  med_idt           _medIdt;         // The _medIdt used to write/read Meshes to ::_filename
   
  //private:
  //  MED_MED_DRIVER();

public :
  MED_MED_DRIVER();

  MED_MED_DRIVER(const string & fileName,  MED * const ptrMed);

  MED_MED_DRIVER(const string & fileName,  MED * const ptrMed,
		 MED_EN::med_mode_acces accessMode);
  // OPERATEUR DE RECOPIE AVEC _medIdt ??

  void open();
  void close();

  virtual void write          ( void ) const ;
  virtual void writeFrom      ( void ) const ;
  virtual void read           ( void ) ;
  virtual void readFileStruct ( void ) ;

};

class MED_MED_RDONLY_DRIVER : public virtual MED_MED_DRIVER
{
public :
  MED_MED_RDONLY_DRIVER():MED_MED_DRIVER(){};

  MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed);

  ~MED_MED_RDONLY_DRIVER() { 
    MESSAGE("MED_MED_RDONLY_DRIVER::~MED_MED_RDONLY_DRIVER() has been destroyed");
  } 

  void open();
  void close();

  void read           ( void ) ;
  void readFileStruct ( void ) ;
};

class MED_MED_WRONLY_DRIVER : public virtual MED_MED_DRIVER
{

public :
  MED_MED_WRONLY_DRIVER():MED_MED_DRIVER(){};

  MED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed);

  ~MED_MED_WRONLY_DRIVER() { 
    MESSAGE("MED_MED_WRONLY_DRIVER::~MED_MED_WRONLY_DRIVER() has been destroyed");
  } 

  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;
};

class MED_MED_RDWR_DRIVER : public virtual MED_MED_RDONLY_DRIVER,
			    public virtual MED_MED_WRONLY_DRIVER,
			    public virtual MED_MED_DRIVER
{

public :
  MED_MED_RDWR_DRIVER():MED_MED_DRIVER(){};

  MED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed);

  ~MED_MED_RDWR_DRIVER() { 
    MESSAGE("MED_MED_RDWR_DRIVER::~MED_MED_RDWR_DRIVER() has been destroyed");
  } 

  void open();
  void close();

  void write          ( void ) const ;
  void writeFrom      ( void ) const ;
  void read           ( void ) ;
  void readFileStruct ( void ) ;
};

#endif /* MED_MED_DRIVER_HXX */
