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
#ifndef MED_MESH_DRIVER22_HXX
#define MED_MESH_DRIVER22_HXX

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

class MED_MESH_DRIVER22 : public virtual MED_MESH_DRIVER
{
protected:
  
  med_2_3::med_idt  _medIdt ;
  
public :

  // all MED cell type
  static const med_2_3::med_geometrie_element all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
  static const char * const all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE];

  /*!
    Constructor.
  */
  MED_MESH_DRIVER22() ;
  /*!
    Constructor.
  */
  MED_MESH_DRIVER22(const string & fileName,
		    MESH * ptrMesh,
		    MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  MED_MESH_DRIVER22(const MED_MESH_DRIVER22 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_DRIVER22() ;

  void open();
  void close();
};

/*!

  Driver Med for MESH : Read only.

  Implement read method.

*/

class MED_MESH_RDONLY_DRIVER22 : public virtual IMED_MESH_RDONLY_DRIVER, public virtual MED_MESH_DRIVER22
{

public :
  
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER22() ;
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER22(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDONLY_DRIVER22(const MED_MESH_RDONLY_DRIVER22 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_RDONLY_DRIVER22() ;
  
  /*!
    Read MESH in the specified file.
  */
  void read ( void );
  /*!
   *  This method activate global faces computation from SCRATCH if a family on FACE exists in the MED file.
   *  This implies a complete renumbering of FACES. This is the default behaviour of this driver.
   */
  void activateFacesComputation() { _computeFaces=true; }
  /*!
   *  This method desactivate global face computation.
   *  That is to say that FACES described in file are supposed to
   *  be correct and complete. The consequence is that reading is much faster. Use with care !
   */
  void desactivateFacesComputation() { _computeFaces=false; }

private:
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber(int * MEDArrayNodeFamily) ;
  int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity) ;
  void getGRID ();

  GENDRIVER * copy ( void ) const ;

private:
  bool _computeFaces;
};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MED_MESH_WRONLY_DRIVER22 : public virtual IMED_MESH_WRONLY_DRIVER, public virtual MED_MESH_DRIVER22 {
  
public :
  
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER22() ;
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER22(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_WRONLY_DRIVER22(const MED_MESH_WRONLY_DRIVER22 & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_WRONLY_DRIVER22() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const;

private:
  int writeCoordinates    ()                           const;
  int writeConnectivities (MED_EN::medEntityMesh entity)       const;
  void groupFamilyConverter(const vector<GROUP*>& groups,vector<FAMILY*>& families) const;
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

class MED_MESH_RDWR_DRIVER22 : public IMED_MESH_RDWR_DRIVER, public MED_MESH_RDONLY_DRIVER22, public MED_MESH_WRONLY_DRIVER22 {

public :

  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER22() ;
  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER22(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDWR_DRIVER22(const MED_MESH_RDWR_DRIVER22 & driver) ;

  /*!
    Destructor.
  */
  ~MED_MESH_RDWR_DRIVER22() ;

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

#endif /* MED_MESH_DRIVER22_HXX */
