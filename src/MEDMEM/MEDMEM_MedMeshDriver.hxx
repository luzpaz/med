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
#ifndef MED_MESH_DRIVER_HXX
#define MED_MESH_DRIVER_HXX

#include <MEDMEM.hxx>

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

/*!

  Driver Med for MESH.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
class MESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MEDMEM_EXPORT MED_MESH_DRIVER : public GENDRIVER
{
protected:

  MESH *         _ptrMesh;
  mutable string _meshName;
  int            _meshNum;     // INUTILE ?


public :

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

  virtual void open() = 0;
  virtual void close() = 0;

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  virtual void   setMeshName(const string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  virtual string getMeshName() const ;

public:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};

/*!

  Driver Med for MESH : Read only.

  Implement read method.

*/

class MEDMEM_EXPORT IMED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  IMED_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  IMED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  IMED_MESH_RDONLY_DRIVER(const IMED_MESH_RDONLY_DRIVER & driver) ;
  
  // CREER UNE METHODE POUR LIRE LA LISTE DES MAILLAGES .....
  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const;
  virtual void activateFacesComputation() = 0;
  virtual void desactivateFacesComputation() = 0;

protected:
//   virtual int getCOORDINATE() = 0 ;
//   virtual int getCONNECTIVITY() = 0 ;
//   virtual int getFAMILY() = 0 ;
//   virtual int getNodalConnectivity(CONNECTIVITY * Connectivity) = 0 ;
  int getDescendingConnectivity(CONNECTIVITY * Connectivity);
//   virtual int getNodesFamiliesNumber(int * MEDArrayNodeFamily) = 0 ;
//   virtual int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity) = 0 ;
  void updateFamily() ;
  void buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) ;
//   virtual void getGRID () = 0 ;

  friend class MED_MESH_RDONLY_DRIVER;

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MEDMEM_EXPORT IMED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER {
  
public :
  
  /*!
    Constructor.
  */
  IMED_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  IMED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces access=MED_EN::WRONLY) ;
  /*!
    Copy constructor.
  */
  IMED_MESH_WRONLY_DRIVER(const IMED_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~IMED_MESH_WRONLY_DRIVER() ;

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void );

// protected:
//   virtual int writeCoordinates    ()                           const = 0 ;
//   virtual int writeConnectivities (MED_EN::medEntityMesh entity)       const = 0 ;
//   virtual int writeFamilyNumbers  ()                           const = 0 ;
//   virtual int writeFamilies       (vector<FAMILY*> & families) const = 0 ;
//   virtual int writeGRID() const = 0 ;

  friend class MED_MESH_WRONLY_DRIVER;
};


/*!

  Driver Med for MESH : Read write.
  - Use read method from MED_MESH_RDONLY_DRIVER
  - Use write method from MED_MESH_WRONLY_DRIVER

*/

class MEDMEM_EXPORT IMED_MESH_RDWR_DRIVER : public virtual IMED_MESH_RDONLY_DRIVER, public virtual IMED_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  IMED_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  IMED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  IMED_MESH_RDWR_DRIVER(const IMED_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~IMED_MESH_RDWR_DRIVER() ;

  friend class MED_MESH_RDWR_DRIVER;

};

class MEDMEM_EXPORT MED_MESH_RDONLY_DRIVER : public virtual IMED_MESH_RDONLY_DRIVER
{
public:
  MED_MESH_RDONLY_DRIVER();
  MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
  MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver);
  ~MED_MESH_RDONLY_DRIVER();
  void   setMeshName(const string & meshName);
  string getMeshName() const;
  void write( void ) const;
  void activateFacesComputation();
  void desactivateFacesComputation();
  void read ( void );
  void open();
  void close();
  virtual void setFileName ( const string & fileName) {_concreteMeshDrv->setFileName(fileName); }
protected:
  GENDRIVER * _concreteMeshDrv;
//   int getCOORDINATE();
//   int getCONNECTIVITY();
//   int getFAMILY();
//   int getNodalConnectivity(CONNECTIVITY * Connectivity);
//   int getDescendingConnectivity(CONNECTIVITY * Connectivity);
//   int getNodesFamiliesNumber(int * MEDArrayNodeFamily);
//   int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity);
//   void getGRID ();
  GENDRIVER * copy ( void ) const;
};

class MEDMEM_EXPORT MED_MESH_WRONLY_DRIVER : public virtual IMED_MESH_WRONLY_DRIVER {
public :
  MED_MESH_WRONLY_DRIVER();
  MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh, MED_EN::med_mode_acces access=MED_EN::WRONLY);
  MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver);
  ~MED_MESH_WRONLY_DRIVER();
  void   setMeshName(const string & meshName);
  string getMeshName() const;
  void read ( void );
  void write( void ) const;
  void open();
  void close();
  virtual void setFileName ( const string & fileName) {_concreteMeshDrv->setFileName(fileName); }
protected:
  GENDRIVER * _concreteMeshDrv;
//   int writeCoordinates    ()                           const;
//   int writeConnectivities (MED_EN::medEntityMesh entity)       const;
//   int writeFamilyNumbers  ()                           const;
//   int writeFamilies       (vector<FAMILY*> & families) const;
//   int writeGRID() const;
  GENDRIVER * copy ( void ) const;
};

class MEDMEM_EXPORT MED_MESH_RDWR_DRIVER : public IMED_MESH_RDWR_DRIVER {
public :
  MED_MESH_RDWR_DRIVER();
  MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
  MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver);
  ~MED_MESH_RDWR_DRIVER();
  void   setMeshName(const string & meshName);
  string getMeshName() const;
  void read ( void );
  void write( void ) const;
  void activateFacesComputation();
  void desactivateFacesComputation();
  void open();
  void close();
  virtual void setFileName ( const string & fileName) {_concreteMeshDrv->setFileName(fileName); }
protected:
  GENDRIVER * _concreteMeshDrv;
//   int getCOORDINATE();
//   int getCONNECTIVITY();
//   int getFAMILY();
//   int getNodalConnectivity(CONNECTIVITY * Connectivity);
//   int getDescendingConnectivity(CONNECTIVITY * Connectivity);
//   int getNodesFamiliesNumber(int * MEDArrayNodeFamily);
//   int getCellsFamiliesNumber(int** Arrays, CONNECTIVITY* Connectivity, MED_EN::medEntityMesh entity);
//   void getGRID ();
//   int writeCoordinates    ()                           const;
//   int writeConnectivities (MED_EN::medEntityMesh entity)       const;
//   int writeFamilyNumbers  ()                           const;
//   int writeFamilies       (vector<FAMILY*> & families) const;
//   int writeGRID() const;
  GENDRIVER * copy ( void ) const;
};

};

#endif /* MED_MESH_DRIVER_HXX */
