#ifndef MED_MESH_DRIVER_HXX
#define MED_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

/*!

  Driver Med for MESH.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MED_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *   _ptrMesh;
  med_2_1::med_idt        _medIdt;
  string         _meshName;    // const ?
  int            _meshNum;     // INUTILE ?
  
public :

  // all MED cell type
  static const med_2_1::med_geometrie_element all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];

  /*!
    Constructor.
  */
  MED_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_DRIVER(const string & fileName,  
		  MESH * ptrMesh, 
		  MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  MED_MESH_DRIVER(const MED_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_DRIVER() ;

  void open();
  void close();

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

/*!

  Driver Med for MESH : Read only.

  Implement read method.

*/

class MED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_RDONLY_DRIVER() ;
  
  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const;
  /*!
    Read MESH in the specified file.
  */
  void read ( void );

private:
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getDescendingConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber(int * MEDArrayNodeFamily) ;
  int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity) ;
  void updateFamily() ;
  void buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) ;
  void getGRID ();

  GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER {
  
public :
  
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_WRONLY_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const;
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void );

private:
  int writeCoordinates    ()                           const;
  int writeConnectivities (MED_EN::medEntityMesh entity)       const;
  int writeFamilyNumbers  ()                           const;
  int writeFamilies       (vector<FAMILY*> & families) const;
  int writeGRID() const;

  GENDRIVER * copy ( void ) const ;
};


/*!

  Driver Med for MESH : Read write.
  - Use read method from MED_MESH_RDONLY_DRIVER
  - Use write method from MED_MESH_WRONLY_DRIVER

*/

class MED_MESH_RDWR_DRIVER : public MED_MESH_RDONLY_DRIVER, public MED_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~MED_MESH_RDWR_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write(void) const;
  /*!
    Read MESH in the specified file.
  */
  void read (void);

private:
  GENDRIVER * copy(void) const ;

};
};

#endif /* MED_MESH_DRIVER_HXX */
