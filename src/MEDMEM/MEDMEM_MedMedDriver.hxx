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

/*!

  Driver Med for MED.

  Generic part : implement open and close methods.

*/

// This driver pilots within a MED class read/write accesses of fields/meshes
class MED_MED_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMed;         // Store 'MED_DRIVER (0..n)----(1) MED' associations
  med_idt           _medIdt;         // The _medIdt used to write/read Meshes to ::_filename
   
  //private:
  //  MED_MED_DRIVER();

public :
  /*!
    Constructor.
  */
  MED_MED_DRIVER();
  /*!
    Constructor.
  */
  MED_MED_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Constructor.
  */
  MED_MED_DRIVER(const string & fileName,  MED * const ptrMed,
		 MED_EN::med_mode_acces accessMode);

  // OPERATEUR DE RECOPIE AVEC _medIdt ??
  /*!
    Copy constructor.
  */
  MED_MED_DRIVER(const MED_MED_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_DRIVER() ;

  void open() throw (MEDEXCEPTION) ;
  void close();

  //virtual void write          ( void ) const = 0 ;
  virtual void write          ( void ) const ;
  //virtual void writeFrom      ( void ) const = 0 ;
  virtual void writeFrom      ( void ) const  ;
  //virtual void read           ( void ) = 0 ;
  virtual void read           ( void )  ;
  //virtual void readFileStruct ( void ) = 0 ;
  virtual void readFileStruct ( void ) ;

private:
  //virtual GENDRIVER * copy ( void ) const = 0 ;
  virtual GENDRIVER * copy ( void ) const  ;

};

/*!

  Driver Med for MED : Read only.

  Implement read method.

*/

class MED_MED_RDONLY_DRIVER : public virtual MED_MED_DRIVER
{
public :
  /*!
    Constructor.
  */
  MED_MED_RDONLY_DRIVER();
  /*!
    Constructor.
  */
  MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_RDONLY_DRIVER(const MED_MED_RDONLY_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_RDONLY_DRIVER();

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write          ( void ) const throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  /*!
    Read all from the file.
  */
  void read           ( void ) throw (MEDEXCEPTION);
  /*!
    Read only table of contents of the file.

    All objects are created but there values are not read.
  */
  void readFileStruct ( void ) throw (MEDEXCEPTION) ;

private:
  virtual GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MED : Write only.

  Implement write method.

*/

class MED_MED_WRONLY_DRIVER : public virtual MED_MED_DRIVER
{

public :
  /*!
    Constructor.
  */
  MED_MED_WRONLY_DRIVER();
  /*!
    Constructor.
  */
  MED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_WRONLY_DRIVER(const MED_MED_WRONLY_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_WRONLY_DRIVER();

  /*!
    Write all in file.
  */
  void write          ( void ) const throw (MEDEXCEPTION);
  /*!
    Write only objects created from this MED driver in file.
  */
  void writeFrom      ( void ) const throw (MEDEXCEPTION);
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read           ( void ) throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void readFileStruct ( void ) throw (MEDEXCEPTION) ;

private:
  virtual GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MED : Read write.
  - Use read method from MED_MED_RDONLY_DRIVER
  - Use write method from MED_MED_WDONLY_DRIVER

*/

class MED_MED_RDWR_DRIVER : public virtual MED_MED_RDONLY_DRIVER,
			    public virtual MED_MED_WRONLY_DRIVER
{

public :
  /*!
    Constructor.
  */
  MED_MED_RDWR_DRIVER();
  /*!
    Constructor.
  */
  MED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_RDWR_DRIVER(const MED_MED_RDWR_DRIVER & driver);

  /*!
    Destructor.
  */
  ~MED_MED_RDWR_DRIVER();

  /*!
    Write all in file.
  */
  void write          ( void ) const throw (MEDEXCEPTION);
  /*!
    Write only objects created from this MED driver in file.
  */
  void writeFrom      ( void ) const throw (MEDEXCEPTION);
  /*!
    Read all from the file.
  */
  void read           ( void ) throw (MEDEXCEPTION);
  /*!
    Read only table of contents of the file.

    All objects are created but there values are not read.
  */
  void readFileStruct ( void ) throw (MEDEXCEPTION);

private:
  virtual GENDRIVER * copy ( void ) const ;

};

#endif /* MED_MED_DRIVER_HXX */
