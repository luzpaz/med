//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MEDMEM_MedMeshDriver.hxx
//  Module : MED

#ifndef MED_MESH_DRIVER_HXX
#define MED_MESH_DRIVER_HXX

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "utilities.h"

class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;

/*!

  Driver Med for MESH.

  Generic part : implement open and close methods.

*/

class MED_MESH_DRIVER : public GENDRIVER
{
protected:
  
  MESH *   _ptrMesh;
  MED_FR::med_idt        _medIdt;
  string         _meshName;    // const ?
  int            _meshNum;     // INUTILE ?
  
public :

  // all MED cell type
  static const MED_FR::med_geometrie_element all_cell_type[MED_NBR_GEOMETRIE_MAILLE];
  
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
		  med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  MED_MESH_DRIVER(const MED_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_DRIVER() ;

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
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read ( void ) throw (MEDEXCEPTION);

private:
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getDescendingConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber(int * MEDArrayNodeFamily) ;
  int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity) ;
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
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION);

private:
  int writeCoordinates    ()                           const;
  int writeConnectivities (medEntityMesh entity)       const;
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
  void write(void) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read (void) throw (MEDEXCEPTION);

private:
  GENDRIVER * copy(void) const ;

};

#endif /* MED_MESH_DRIVER_HXX */
