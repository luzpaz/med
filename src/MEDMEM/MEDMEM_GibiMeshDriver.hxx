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
  static const medGeometryElement geomGIBItoMED[nb_geometrie_gibi] =  // A COMPLETER
    {   /*1 */ MED_POINT1 ,/*2 */ MED_SEG2   ,/*3 */ MED_SEG3   ,/*4 */ MED_TRIA3  ,/*5 */ MED_NONE   ,
	/*6 */ MED_TRIA6  ,/*7 */ MED_NONE   ,/*8 */ MED_QUAD4  ,/*9 */ MED_NONE   ,/*10*/ MED_QUAD8  ,
	/*11*/ MED_NONE   ,/*12*/ MED_NONE   ,/*13*/ MED_NONE   ,/*14*/ MED_HEXA8  ,/*15*/ MED_HEXA20 ,
	/*16*/ MED_PENTA6 ,/*17*/ MED_PENTA15,/*18*/ MED_NONE   ,/*19*/ MED_NONE   ,/*20*/ MED_NONE   ,
	/*21*/ MED_NONE   ,/*22*/ MED_NONE   ,/*23*/ MED_TETRA4 ,/*24*/ MED_TETRA10,/*25*/ MED_PYRA5  ,
	/*26*/ MED_PYRA13 ,/*27*/ MED_NONE   ,/*28*/ MED_NONE   ,/*29*/ MED_NONE   ,/*30*/ MED_NONE   ,
	/*31*/ MED_NONE   ,/*32*/ MED_NONE   ,/*33*/ MED_NONE   ,/*34*/ MED_NONE   ,/*35*/ MED_NONE   ,
	/*36*/ MED_NONE   ,/*37*/ MED_NONE   ,/*38*/ MED_NONE   ,/*39*/ MED_NONE   ,/*40*/ MED_NONE   ,
	/*41*/ MED_NONE   ,/*42*/ MED_NONE   ,/*43*/ MED_NONE   ,/*44*/ MED_NONE   ,/*45*/ MED_NONE   ,
	/*46*/ MED_NONE   ,/*47*/ MED_NONE   };
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
