#ifndef GIBI_MESH_DRIVER_HXX
#define GIBI_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include <fstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

/////
#include <sstream>
#include <iomanip>
/////

/*!

  Driver GIBI for MESH.

  Generic part : implement the readopen and close methods.
  
*/

namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class GIBI_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *          _ptrMesh;
  // A VOIR FILE DESCRIPTEUR ? MED_FR::med_idt _medIdt;
  string          _meshName;
  /////
  ifstream _gibi;
  
  // tableau de correspondance des types géométriques de CASTEM -> MED
  static const size_t nb_geometrie_gibi=47;
  static const medGeometryElement geomGIBItoMED[nb_geometrie_gibi];
  /////

public :

  /*!
    Constructor.
  */
  GIBI_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_DRIVER(const string & fileName,  
		  MESH * ptrMesh, 
		  med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_DRIVER() ;

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


class GIBI_MESH_RDONLY_DRIVER : public virtual GIBI_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  GIBI_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_RDONLY_DRIVER() ;
  
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

class GIBI_MESH_WRONLY_DRIVER : public virtual GIBI_MESH_DRIVER {
  
public :
  
  /*!
    Constructor.
  */
  GIBI_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_WRONLY_DRIVER() ;

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

  Driver GIBI for MESH : Read write.
  - Use read method from GIBI_MESH_RDONLY_DRIVER
  - Use write method from GIBI_MESH_WRONLY_DRIVER

*/

class GIBI_MESH_RDWR_DRIVER : public GIBI_MESH_RDONLY_DRIVER, public GIBI_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  GIBI_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~GIBI_MESH_RDWR_DRIVER() ;

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


#endif /* GIBI_MESH_DRIVER_HXX */
