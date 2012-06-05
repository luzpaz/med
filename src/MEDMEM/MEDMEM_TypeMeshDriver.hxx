// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#ifndef TYPE_MESH_DRIVER_HXX
#define TYPE_MESH_DRIVER_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

/*!

  Driver TYPE for MESH.

  Generic part : implement the readopen and close methods.
  
*/


namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MEDMEM_EXPORT TYPE_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *          _ptrMesh;
  // A VOIR FILE DESCRIPTEUR ? MED_FR::med_idt _medIdt;
  string          _meshName;    
  
public :

  /*!
    Constructor.
  */
  TYPE_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  TYPE_MESH_DRIVER(const std::string & fileName,  
                  MESH * ptrMesh, 
                  med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  TYPE_MESH_DRIVER(const TYPE_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~TYPE_MESH_DRIVER() ;

  void open() throw (MEDEXCEPTION);
  void close() throw (MEDEXCEPTION);

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  void   setMeshName(const string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  string getMeshName() const ;

private:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};


class MEDMEM_EXPORT TYPE_MESH_RDONLY_DRIVER : public virtual TYPE_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  TYPE_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  TYPE_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  TYPE_MESH_RDONLY_DRIVER(const TYPE_MESH_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~TYPE_MESH_RDONLY_DRIVER() ;
  
  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read ( void ) throw (MEDEXCEPTION);

private:
 
  GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MEDMEM_EXPORT TYPE_MESH_WRONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
  
public :
  
  /*!
    Constructor.
  */
  TYPE_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  TYPE_MESH_WRONLY_DRIVER(const TYPE_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~TYPE_MESH_WRONLY_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION);

private:

  GENDRIVER * copy ( void ) const ;
};


/*!

  Driver TYPE for MESH : Read write.
  - Use read method from TYPE_MESH_RDONLY_DRIVER
  - Use write method from TYPE_MESH_WRONLY_DRIVER

*/

class MEDMEM_EXPORT TYPE_MESH_RDWR_DRIVER : public TYPE_MESH_RDONLY_DRIVER, public TYPE_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  TYPE_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  TYPE_MESH_RDWR_DRIVER(const TYPE_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~TYPE_MESH_RDWR_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write(void) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read (void) throw (MEDEXCEPTION);

private:
  GENDRIVER * copy(void) const ;

};
};


#endif /* TYPE_MESH_DRIVER_HXX */
