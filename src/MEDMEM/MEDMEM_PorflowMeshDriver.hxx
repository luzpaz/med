#ifndef PORFLOW_MESH_DRIVER_HXX
#define PORFLOW_MESH_DRIVER_HXX

#include <string>
#include <fstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_DriverTools.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

/*!

  Driver PORFLOW for MESH.

  Generic part : implement the readopen and close methods.
  
*/


namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class PORFLOW_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *          _ptrMesh;
  // A VOIR FILE DESCRIPTEUR ? MED_FR::med_idt _medIdt;
  string          _meshName;    
  ifstream        _porflow;

  // tableau de correspondance des types géométriques de PORFLOW -> MED
  static const size_t nb_geometrie_porflow = 6;
  static const MED_EN::medGeometryElement geomPORFLOWtoMED[nb_geometrie_porflow];
  // indirection table from PORFLOW order to MED one for nodes numerotation in all PORFLOW geometries
  static const size_t nb_nodes_max = 8;  // maximal number of nodes for a porflow geometrie
  static const size_t nb_nodes2_max = 4; // maximal number of nodes for a 2D porflow geometrie
  static const size_t nb_faces_max = 6;  // maximal number of faces for a porflow geometrie
  static const int numPORFLOWtoMED[nb_geometrie_porflow] [nb_nodes_max];
  static const int connectivityPORFLOW[nb_geometrie_porflow][nb_faces_max][nb_nodes2_max];
  inline static int geomMEDtoPorflow(MED_EN::medGeometryElement medGeo);


public :

  /*!
    Constructor.
  */
  PORFLOW_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  PORFLOW_MESH_DRIVER(const string & fileName,  
		  MESH * ptrMesh, 
		  MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  PORFLOW_MESH_DRIVER(const PORFLOW_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~PORFLOW_MESH_DRIVER() ;

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


class PORFLOW_MESH_RDONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  PORFLOW_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  PORFLOW_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  PORFLOW_MESH_RDONLY_DRIVER(const PORFLOW_MESH_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~PORFLOW_MESH_RDONLY_DRIVER() ;
  
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

  static void readPorflowCoordinateFile(const string & coorFileName,_intermediateMED & medi,const int space_dimension);
  static void readPorflowConnectivityFile(bool hybride,const string & connecFileName,_intermediateMED & medi,std::vector<set<_maille>::iterator>& p_ma_table,int mesh_dimension);

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class PORFLOW_MESH_WRONLY_DRIVER : public virtual PORFLOW_MESH_DRIVER {
  
public :
  
  /*!
    Constructor.
  */
  PORFLOW_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  PORFLOW_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  PORFLOW_MESH_WRONLY_DRIVER(const PORFLOW_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~PORFLOW_MESH_WRONLY_DRIVER() ;

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

  Driver PORFLOW for MESH : Read write.
  - Use read method from PORFLOW_MESH_RDONLY_DRIVER
  - Use write method from PORFLOW_MESH_WRONLY_DRIVER

*/

class PORFLOW_MESH_RDWR_DRIVER : public PORFLOW_MESH_RDONLY_DRIVER, public PORFLOW_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  PORFLOW_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  PORFLOW_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  PORFLOW_MESH_RDWR_DRIVER(const PORFLOW_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~PORFLOW_MESH_RDWR_DRIVER() ;

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


#endif /* PORFLOW_MESH_DRIVER_HXX */
