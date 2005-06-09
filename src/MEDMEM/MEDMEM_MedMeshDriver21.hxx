#ifndef MED_MESH_DRIVER21_HXX
#define MED_MESH_DRIVER21_HXX

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Compatibility21_22.hxx"

/*!

  Driver Med for MESH.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;

class MED_MESH_DRIVER21 : public virtual MED_MESH_DRIVER
{
protected:

  med_2_1::med_idt        _medIdt;
  
public :

  // all MED cell type
  static const med_2_1::med_geometrie_element all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];

  /*!
    Constructor.
  */
  MED_MESH_DRIVER21() ;
 /*!
    Constructor.
  */
  MED_MESH_DRIVER21(const string & fileName,  
		    MESH * ptrMesh, 
		    MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  MED_MESH_DRIVER21(const MED_MESH_DRIVER21 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_DRIVER21() ;

  void open();
  void close();
};

/*!

  Driver Med for MESH : Read only.

  Implement read method.

*/

  class MED_MESH_RDONLY_DRIVER21 : public virtual IMED_MESH_RDONLY_DRIVER , public virtual MED_MESH_DRIVER21
{
 
public :
  
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER21() ;
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER21(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDONLY_DRIVER21(const MED_MESH_RDONLY_DRIVER21 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_RDONLY_DRIVER21() ;
  void read(void);
private:
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber(int * MEDArrayNodeFamily) ;
  int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity) ;
  void getGRID ();

  GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MED_MESH_WRONLY_DRIVER21 : public virtual IMED_MESH_WRONLY_DRIVER, public virtual MED_MESH_DRIVER21 {
  
public :
  
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER21() ;
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER21(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_WRONLY_DRIVER21(const MED_MESH_WRONLY_DRIVER21 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_WRONLY_DRIVER21() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const;

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

class MED_MESH_RDWR_DRIVER21 : public IMED_MESH_RDWR_DRIVER, public MED_MESH_RDONLY_DRIVER21, public MED_MESH_WRONLY_DRIVER21 {

public :

  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER21() ;
  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER21(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDWR_DRIVER21(const MED_MESH_RDWR_DRIVER21 & driver) ;

  /*!
    Destructor.
  */
  ~MED_MESH_RDWR_DRIVER21() ;

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

#endif /* MED_MESH_DRIVER21_HXX */
