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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef MED_MED_DRIVER_HXX
#define MED_MED_DRIVER_HXX

#include <string>
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Utilities.hxx"
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

class MED_MED_DRIVER : public GENDRIVER
{
protected:
  
  MED * const       _ptrMed;         // Store 'MED_DRIVER (0..n)----(1) MED' associations
  GENDRIVER * _concreteMedDrv;

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

  virtual void open() throw (MEDEXCEPTION) = 0  ;
  virtual void close() = 0 ;
  virtual void write          ( void ) const = 0 ;
  virtual void writeFrom      ( void ) const = 0 ;
  virtual void read           ( void ) = 0 ;
  virtual void readFileStruct ( void ) = 0 ;

protected:
  virtual GENDRIVER * copy ( void ) const = 0 ;

  friend class MED_MED_RDONLY_DRIVER;
  friend class MED_MED_WRONLY_DRIVER;
  friend class MED_MED_RDWR_DRIVER;
};

/*!

  Driver Med for MED : Read only.

  Implement read method.

*/

class IMED_MED_RDONLY_DRIVER : public virtual MED_MED_DRIVER
{
public :
  /*!
    Constructor.
  */
  IMED_MED_RDONLY_DRIVER();
  /*!
    Constructor.
  */
  IMED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  IMED_MED_RDONLY_DRIVER(const IMED_MED_RDONLY_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~IMED_MED_RDONLY_DRIVER();

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write          ( void ) const throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;

protected:
  // PAL14192: to allow painless repeated readFileStruct() calls
  bool _fileStructIsRead;
};

/*!

  Driver Med for MED : Write only.

  Implement write method.

*/

class IMED_MED_WRONLY_DRIVER : public virtual MED_MED_DRIVER
{

public :
  /*!
    Constructor.
  */
  IMED_MED_WRONLY_DRIVER();
  /*!
    Constructor.
  */
  IMED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  IMED_MED_WRONLY_DRIVER(const IMED_MED_WRONLY_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~IMED_MED_WRONLY_DRIVER();

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read           ( void ) throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void readFileStruct ( void ) throw (MEDEXCEPTION) ;

};

/*!

  Driver Med for MED : Read write.
  - Use read method from MED_MED_RDONLY_DRIVER
  - Use write method from MED_MED_WDONLY_DRIVER

*/

class IMED_MED_RDWR_DRIVER : public virtual IMED_MED_RDONLY_DRIVER,
			     public virtual IMED_MED_WRONLY_DRIVER
{

public :
  /*!
    Constructor.
  */
  IMED_MED_RDWR_DRIVER();
  /*!
    Constructor.
  */
  IMED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  IMED_MED_RDWR_DRIVER(const IMED_MED_RDWR_DRIVER & driver);

  /*!
    Destructor.
  */
  ~IMED_MED_RDWR_DRIVER();

};

class MED_MED_RDONLY_DRIVER : public virtual IMED_MED_RDONLY_DRIVER
{
public :
  MED_MED_RDONLY_DRIVER();
  MED_MED_RDONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  MED_MED_RDONLY_DRIVER(const MED_MED_RDONLY_DRIVER & driver);
  virtual ~MED_MED_RDONLY_DRIVER();
  void write          ( void ) const throw (MEDEXCEPTION) ;
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  virtual void open() throw (MEDEXCEPTION) ;
  virtual void close() ;
  virtual void read           ( void ) ;
  virtual void readFileStruct ( void ) ;
private:
  GENDRIVER * copy ( void ) const;
};

class MED_MED_WRONLY_DRIVER : public virtual IMED_MED_WRONLY_DRIVER
{

public :
  MED_MED_WRONLY_DRIVER();
  MED_MED_WRONLY_DRIVER(const string & fileName,  MED * const ptrMed);
  MED_MED_WRONLY_DRIVER(const MED_MED_WRONLY_DRIVER & driver);
  virtual ~MED_MED_WRONLY_DRIVER();
  void write          ( void ) const throw (MEDEXCEPTION) ;
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  virtual void open() throw (MEDEXCEPTION) ;
  virtual void close() ;
  virtual void read           ( void ) throw (MEDEXCEPTION) ;
  virtual void readFileStruct ( void ) throw (MEDEXCEPTION) ;
private:
  GENDRIVER * copy ( void ) const;
};

class MED_MED_RDWR_DRIVER : public IMED_MED_RDWR_DRIVER
{

public :
  MED_MED_RDWR_DRIVER();
  MED_MED_RDWR_DRIVER(const string & fileName,  MED * const ptrMed);
  MED_MED_RDWR_DRIVER(const MED_MED_RDWR_DRIVER & driver);
  ~MED_MED_RDWR_DRIVER();
  void write          ( void ) const throw (MEDEXCEPTION) ;
  void writeFrom      ( void ) const throw (MEDEXCEPTION) ;
  virtual void open() throw (MEDEXCEPTION) ;
  virtual void close() ;
  virtual void read           ( void ) throw (MEDEXCEPTION) ;
  virtual void readFileStruct ( void ) throw (MEDEXCEPTION) ;
private:
  GENDRIVER * copy ( void ) const;
};
}

#endif /* MED_MED_DRIVER_HXX */
