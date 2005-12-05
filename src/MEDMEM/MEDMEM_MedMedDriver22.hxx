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
#ifndef MED_MED_DRIVER22_HXX
#define MED_MED_DRIVER22_HXX

#include "MEDMEM_MedMedDriver.hxx"
#include <string>
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

class MED_MED_DRIVER22 : public virtual MED_MED_DRIVER
{
protected:
  
  med_2_2::med_idt           _medIdt;         // The _medIdt used to write/read Meshes to ::_filename
   
  //private:
  //  MED_MED_DRIVER();

public :
  /*!
    Constructor.
  */
  MED_MED_DRIVER22();
  /*!
    Constructor.
  */
  MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed);
  /*!
    Constructor.
  */
  MED_MED_DRIVER22(const string & fileName,  MED * const ptrMed,
		   MED_EN::med_mode_acces accessMode);

  // OPERATEUR DE RECOPIE AVEC _medIdt ??
  /*!
    Copy constructor.
  */
  MED_MED_DRIVER22(const MED_MED_DRIVER22 & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_DRIVER22() ;

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

  class MED_MED_RDONLY_DRIVER22 : public virtual IMED_MED_RDONLY_DRIVER, public virtual MED_MED_DRIVER22
{
public :
  /*!
    Constructor.
  */
  MED_MED_RDONLY_DRIVER22();
  /*!
    Constructor.
  */
  MED_MED_RDONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_RDONLY_DRIVER22(const MED_MED_RDONLY_DRIVER22 & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_RDONLY_DRIVER22();

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

class MED_MED_WRONLY_DRIVER22 : public virtual IMED_MED_WRONLY_DRIVER, public virtual MED_MED_DRIVER22
{

public :
  /*!
    Constructor.
  */
  MED_MED_WRONLY_DRIVER22();
  /*!
    Constructor.
  */
  MED_MED_WRONLY_DRIVER22(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_WRONLY_DRIVER22(const MED_MED_WRONLY_DRIVER22 & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MED_WRONLY_DRIVER22();

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

class MED_MED_RDWR_DRIVER22 : public virtual MED_MED_RDONLY_DRIVER22,
			      public virtual MED_MED_WRONLY_DRIVER22,
		              public virtual IMED_MED_RDWR_DRIVER
{

public :
  /*!
    Constructor.
  */
  MED_MED_RDWR_DRIVER22();
  /*!
    Constructor.
  */
  MED_MED_RDWR_DRIVER22(const string & fileName,  MED * const ptrMed);
  /*!
    Copy constructor.
  */
  MED_MED_RDWR_DRIVER22(const MED_MED_RDWR_DRIVER22 & driver);

  /*!
    Destructor.
  */
  ~MED_MED_RDWR_DRIVER22();

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
};

#endif /* MED_MED_DRIVER22_HXX */
