#ifndef MED_MED_DRIVER21_HXX
#define MED_MED_DRIVER21_HXX

#include "MEDMEM_MedMedDriver.hxx"
#include <string>
#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_define.hxx"

/*!

  Driver Med for MED.

  Generic part : implement open and close methods.

*/

// This driver pilots within a MED class read/write accesses of fields/meshes
namespace MEDMEM {
class MESH;
class FIELD_;
class MED;

class MED_MED_DRIVER21 : public virtual MED_MED_DRIVER
{
protected:

  med_2_1::med_idt           _medIdt;         // The _medIdt used to write/read Meshes to ::_filename
   
public :
  MED_MED_DRIVER21();
  MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed);
  MED_MED_DRIVER21(const string & fileName,  MED * const ptrMed,
		   MED_EN::med_mode_acces accessMode);
  MED_MED_DRIVER21(const MED_MED_DRIVER21 & driver);
  virtual ~MED_MED_DRIVER21();
  void open() throw (MEDEXCEPTION) ;
  void close();
  virtual void write          ( void ) const ;
  virtual void writeFrom      ( void ) const  ;
  virtual void read           ( void )  ;
  virtual void readFileStruct ( void ) ;
private:
  virtual GENDRIVER * copy ( void ) const  ;
};

/*!

  Driver Med for MED : Read only.

  Implement read method.

*/

  class MED_MED_RDONLY_DRIVER21 : public virtual IMED_MED_RDONLY_DRIVER, public virtual MED_MED_DRIVER21
{
public :
  MED_MED_RDONLY_DRIVER21();
  MED_MED_RDONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
  MED_MED_RDONLY_DRIVER21(const MED_MED_RDONLY_DRIVER21 & driver);
  virtual ~MED_MED_RDONLY_DRIVER21();
  void write          ( void ) const throw (MEDEXCEPTION) ;
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  void read           ( void ) throw (MEDEXCEPTION);
  void readFileStruct ( void ) throw (MEDEXCEPTION) ;
private:
  virtual GENDRIVER * copy ( void ) const ;
};

class MED_MED_WRONLY_DRIVER21 : public virtual IMED_MED_WRONLY_DRIVER, public virtual MED_MED_DRIVER21
{

public :
  MED_MED_WRONLY_DRIVER21();
  MED_MED_WRONLY_DRIVER21(const string & fileName,  MED * const ptrMed);
  MED_MED_WRONLY_DRIVER21(const MED_MED_WRONLY_DRIVER21 & driver);
  virtual ~MED_MED_WRONLY_DRIVER21();
  void write          ( void ) const throw (MEDEXCEPTION);
  void writeFrom      ( void ) const throw (MEDEXCEPTION);
  void read           ( void ) throw (MEDEXCEPTION) ;
  void readFileStruct ( void ) throw (MEDEXCEPTION) ;
private:
  virtual GENDRIVER * copy ( void ) const ;
};

class MED_MED_RDWR_DRIVER21 : public MED_MED_RDONLY_DRIVER21,
			      public MED_MED_WRONLY_DRIVER21,
			      public IMED_MED_RDWR_DRIVER
{
public :
  MED_MED_RDWR_DRIVER21();
  MED_MED_RDWR_DRIVER21(const string & fileName,  MED * const ptrMed);
  MED_MED_RDWR_DRIVER21(const MED_MED_RDWR_DRIVER21 & driver);
  ~MED_MED_RDWR_DRIVER21();
  void write          ( void ) const throw (MEDEXCEPTION);
  void writeFrom      ( void ) const throw (MEDEXCEPTION);
  void read           ( void ) throw (MEDEXCEPTION);
  void readFileStruct ( void ) throw (MEDEXCEPTION);
private:
  virtual GENDRIVER * copy ( void ) const ;
};

}

#endif /* MED_MED_DRIVER21_HXX */
