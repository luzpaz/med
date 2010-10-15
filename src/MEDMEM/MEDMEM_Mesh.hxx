//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef MESH_HXX
#define MESH_HXX

#include <MEDMEM.hxx>

#include <string>
#include <vector>
#include <list>
#include <map>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_RCBase.hxx"
#include "MEDMEM_FieldForward.hxx"

namespace MEDMEM {

class CELLMODEL;
class FAMILY;
class GROUP;
class SUPPORT;
class MESH;

MEDMEM_EXPORT ostream & operator<<(ostream &os, const MESH &my);

/*! This class contains all the informations related with a MESH :
  - COORDINATES
  - CONNECTIVITIES
  - FAMILIES OF NODES
  - FAMILIES OF CELLS
  - FAMILIES OF FACES
  - FAMILIES OF EDGES

  NOTE: A Family is only on one type of entity (MED_CELL, MED_FACE, MED_EDGE, MED_NODE).
  You can't have a family on MED_CELL and MED_FACE

*/

class MEDMEM_EXPORT MESH : public RCBASE
{
  //-----------------------//
  //   Attributes
  //-----------------------//

protected :

  string        _name; // A POSITIONNER EN FCT DES IOS ?
  string        _description;

  mutable COORDINATE *   _coordinate;
  mutable CONNECTIVITY * _connectivity;

  int   _spaceDimension;
  int   _meshDimension;
  int   _numberOfNodes;

  //////////////////////////////////////////////////////////////////////////////////////
  ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
  //////////////////////////////////////////////////////////////////////////////////////
  ///
  ///  La map suivante donne le lien numérotation optionnelle => numérotation cannonique
  ///  Elle  sera calculée apres l'appel de MEDnumLire(...)
  ///  Et sera utilisée avant chaque appel a MEDconnLire(...) pour renuméroter toutes les mailles de façon canonique [1...n]
  ///  _coordinate->NodeNumber sera utilisé avant chaque appel à MEDconnEcri pour démunéroter les mailles en leur numérotation originelle
  ///  Ce traitement devrait prévenir tout plantage du aux numérotations optionnelles DES NOEUDS
  ///  Et ne ralentira que tres peu les traitements sans numéros optionnels

  int _arePresentOptionnalNodesNumbers;
  map<int,int> _optionnalToCanonicNodesNumbers;

  vector<FAMILY*> _familyNode;
  vector<FAMILY*> _familyCell;
  vector<FAMILY*> _familyFace;
  vector<FAMILY*> _familyEdge;

  vector<GROUP*> _groupNode;
  vector<GROUP*> _groupCell;
  vector<GROUP*> _groupFace;
  vector<GROUP*> _groupEdge;
  // list of all Group

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use
  map<int,bool>   _isRead;
  bool           _isAGrid;      // am I a GRID or not

  map<MED_EN::medEntityMesh,SUPPORT*> _entitySupport;

  //-----------------------//
  //   Methods
  //-----------------------//

//   inline void checkGridFillCoords() const;
//   inline void checkGridFillConnectivity() const;
  bool isEmpty() const;
  // if this->_isAGrid, assure that _coordinate and _connectivity are filled
public :

  // Add your personnal driver line (step 2)
  friend class IMED_MESH_RDONLY_DRIVER;
  friend class IMED_MESH_WRONLY_DRIVER;
  friend class MED_MESH_RDONLY_DRIVER21;
  friend class MED_MESH_WRONLY_DRIVER21;
  friend class MED_MESH_RDONLY_DRIVER22;
  friend class MED_MESH_WRONLY_DRIVER22;

  friend class MED_MED_RDONLY_DRIVER21;
  friend class MED_MED_WRONLY_DRIVER21;
  friend class MED_MED_RDWR_DRIVER21;
  friend class MED_MED_RDONLY_DRIVER22;
  friend class MED_MED_WRONLY_DRIVER22;
  friend class MED_MED_RDWR_DRIVER22;

  friend class GIBI_MESH_RDONLY_DRIVER;
  friend class GIBI_MESH_WRONLY_DRIVER;
  friend class GIBI_MESH_RDWR_DRIVER;

  friend class PORFLOW_MESH_RDONLY_DRIVER;
  friend class PORFLOW_MESH_WRONLY_DRIVER;
  friend class PORFLOW_MESH_RDWR_DRIVER;

  friend class VTK_MESH_DRIVER;

  friend class ENSIGHT_MESH_RDONLY_DRIVER;

  void init();
  MESH();
  MESH(MESH &m);
  MESH & operator=(const MESH &m);
  virtual bool operator==(const MESH& other) const;
  virtual bool deepCompare(const MESH& other) const;
  MESH( driverTypes driverType, const string & fileName="",
        const string & meshName="") throw (MEDEXCEPTION);
  virtual ~MESH();
  friend ostream & operator<<(ostream &os, const MESH &my);
  virtual void printMySelf(ostream &os) const;

  int  addDriver(driverTypes driverType,
                 const string & fileName  ="Default File Name.med",
                 const string & driverName="Default Mesh Name",
                 MED_EN::med_mode_acces access=MED_EN::RDWR);
  int  addDriver(GENDRIVER & driver);
  void rmDriver(int index=0);

  virtual void read(int index=0);
  inline void read(const GENDRIVER & genDriver);
  bool isRead(int index=0);
  //inline void write(int index=0, const string & driverName = "");
  virtual void write(int index=0, const string & driverName = "");
  inline void write(const GENDRIVER & genDriver);

  inline void         setName(string name);
  inline void         setDescription(string description);
  inline void         setMeshDimension(int dim);
  inline string       getName() const;
  inline string       getDescription() const;
  inline int          getSpaceDimension() const;
  inline int          getMeshDimension() const;
  inline bool         getIsAGrid();

  inline int                        getNumberOfNodes() const;
  virtual inline const COORDINATE * getCoordinateptr() const;
  inline string                     getCoordinatesSystem() const;
  virtual inline const double *     getCoordinates(MED_EN::medModeSwitch Mode) const;
  virtual inline const double       getCoordinate(int Number,int Axis) const;
  inline const string *             getCoordinatesNames() const;
  inline const string *             getCoordinatesUnits() const;
  //inline int *                    getNodesNumbers();

  virtual inline int             getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
  virtual int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
  virtual inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
  virtual MED_EN::medGeometryElement * getTypesWithPoly(MED_EN::medEntityMesh Entity) const;
  virtual inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
  virtual inline string * getCellTypeNames(MED_EN::medEntityMesh Entity) const;
  virtual const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
  virtual inline int getNumberOfElements(MED_EN::medEntityMesh Entity,
                                         MED_EN::medGeometryElement Type) const;
  virtual int getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity,
                                         MED_EN::medGeometryElement Type) const;
  virtual inline bool existConnectivity(MED_EN::medConnectivity ConnectivityType,
                                        MED_EN::medEntityMesh Entity) const;
  virtual bool existConnectivityWithPoly(MED_EN::medConnectivity ConnectivityType,
                                         MED_EN::medEntityMesh Entity) const;
  inline bool existPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
                                        MED_EN::medEntityMesh Entity) const;
  inline bool existPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType,
                                          MED_EN::medEntityMesh Entity) const;

  virtual inline MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh Entity,
                                                   int Number) const;
  virtual inline MED_EN::medGeometryElement getElementTypeWithPoly(MED_EN::medEntityMesh Entity,
                                                   int Number) const;
  virtual inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
                                            MED_EN::medConnectivity ConnectivityType,
                                            MED_EN::medEntityMesh Entity) const ;
  virtual inline int getConnectivityLength(MED_EN::medModeSwitch Mode,
                                             MED_EN::medConnectivity ConnectivityType,
                                             MED_EN::medEntityMesh Entity,
                                             MED_EN::medGeometryElement Type) const;
  virtual inline const int * getConnectivity(MED_EN::medModeSwitch Mode,
                                             MED_EN::medConnectivity ConnectivityType,
                                             MED_EN::medEntityMesh Entity,
                                             MED_EN::medGeometryElement Type) const;
  virtual inline const int * getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh Entity) const;

  inline int getPolygonsConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                           MED_EN::medEntityMesh Entity) const;
  inline const int * getPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
                                             MED_EN::medEntityMesh Entity) const;
  inline const int * getPolygonsConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh Entity) const;
  inline int getNumberOfPolygons(MED_EN::medEntityMesh Entity=MED_EN::MED_ALL_ENTITIES) const;
  inline int getPolyhedronConnectivityLength(MED_EN::medConnectivity ConnectivityType) const;
  inline const int * getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const;
  inline const int * getPolyhedronFacesIndex() const;
  inline const int * getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const;
  inline int getNumberOfPolyhedronFaces() const;
  inline int getNumberOfPolyhedron() const;

  virtual int                 getElementNumber(MED_EN::medConnectivity ConnectivityType,
                                               MED_EN::medEntityMesh Entity,
                                               MED_EN::medGeometryElement Type,
                                               int * connectivity) const;
  virtual inline int getReverseConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline const int * getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,
                                                    MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline int getReverseConnectivityIndexLength(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline const int * getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;

  virtual int                          getNumberOfFamilies(MED_EN::medEntityMesh Entity) const;
  virtual inline const vector<FAMILY*> getFamilies(MED_EN::medEntityMesh Entity) const;
  virtual inline const FAMILY*         getFamily(MED_EN::medEntityMesh Entity,int i) const;
  virtual int                          getNumberOfGroups(MED_EN::medEntityMesh Entity) const;
  virtual const vector<GROUP*>  getGroups(MED_EN::medEntityMesh Entity) const;
  virtual const GROUP*          getGroup(MED_EN::medEntityMesh Entity,int i) const;
        virtual const GROUP*          getGroup(const string& name) const throw (MEDEXCEPTION);
  virtual inline const CONNECTIVITY*   getConnectivityptr() const;
        inline void setConnectivityptr(CONNECTIVITY*);
  virtual SUPPORT *                    getBoundaryElements(MED_EN::medEntityMesh Entity)
                                                throw (MEDEXCEPTION);
  virtual SUPPORT *                    getSupportOnAll(MED_EN::medEntityMesh Entity)
                                                throw (MEDEXCEPTION);
  // problème avec le maillage dans le support :
  //            le pointeur n'est pas const, mais sa valeur oui. A voir !!! PG

  SUPPORT *                            getSkin(const SUPPORT * Support3D)
                                                throw (MEDEXCEPTION);

  //  Node DonneBarycentre(const Cell &m) const;
  virtual FIELD<double>* getVolume (const SUPPORT * Support, bool isAbs = true) const
                                throw (MEDEXCEPTION);
                                // Support must be on 3D elements
  virtual FIELD<double>* getArea (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 2D elements
  virtual FIELD<double>* getLength (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 1D elements
  virtual FIELD<double>* getNormal (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 2D elements
  virtual FIELD<double>* getBarycenter (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
  //  FIELD<int>* getNeighbourhood(SUPPORT * Support) const
  //                            throw (MEDEXCEPTION); // Il faut preciser !

  /*!
    returns a SUPPORT pointer on the union of all SUPPORTs in Supports.
    You should delete this pointer after use to avois memory leaks.
  */
  static SUPPORT * mergeSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION) ;

  /*!
    returns a SUPPORT pointer on the intersection of all SUPPORTs in Supports.
    The (SUPPORT *) NULL pointer is returned if the intersection is empty.
    You should delete this pointer after use to avois memory leaks.
   */
  static SUPPORT * intersectSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION) ;

  /*!
   * Create families from groups.
   * This function is automaticaly called whenever we ask for families that are not up-to-date.
   * (The creation of families is delayed to the need of user.)
   * If a new created family hapen to already exist, we keep the old one.
   * (There is no way to know which family has change.)
   */
  void createFamilies();
        void createGroups();
  SUPPORT *buildSupportOnNodeFromElementList(const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
  void fillSupportOnNodeFromElementList(const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION);
  SUPPORT *buildSupportOnElementsFromElementList(const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
  int getElementContainingPoint(const double *coord);
//  vector< vector<double> > MESH::getBoundingBox() const;
  vector< vector<double> > getBoundingBox() const;
  template<class T> static
  FIELD<T> * mergeFields(const vector< FIELD<T> * > & others, bool meshCompare=false);
  void convertToPoly();

  /*!
   *For ref counter. Only for client
   */
  virtual void addReference() const;
  virtual void removeReference() const;
};

// ---------------------------------------
//              Methodes Inline
// ---------------------------------------

inline const CONNECTIVITY* MESH::getConnectivityptr() const
{
//   checkGridFillConnectivity();
  return _connectivity;
}

inline void MESH::setConnectivityptr(CONNECTIVITY* conn)
{
        _connectivity=conn;
}
// This method is MED specific : don't use it
// must be private.
inline void MESH::write(const GENDRIVER & genDriver)
{
  const char* LOC = "MESH::write(const MED_MED_DRIVER & genDriver): ";
  BEGIN_OF_MED(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {

      // EAP for MEDMEMTest_Med.cxx:305 :
      // CPPUNIT_ASSERT_MED_NO_THROW(myMed->writeFrom(idMedV21_from));
      // CPPUNIT_ASSERT_MED(access(filenameout21_from.data(), F_OK) != 0);
      string myDrvName = _drivers[index]->getFileName();
      string otherName = genDriver.getFileName();
      if ( !otherName.empty() )
        _drivers[index]->setFileName( otherName );
      // end EAP for MEDMEMTest_Med.cxx:305

      _drivers[index]->open();
      _drivers[index]->write();
      _drivers[index]->close();

      _drivers[index]->setFileName( myDrvName );// EAP for MEDMEMTest_Med.cxx:305
      // ? FINALEMENT PAS BESOIN DE L'EXCEPTION ?
    }

  END_OF_MED(LOC);

}

// This method is MED specific : don't use it
// must be private.
inline void MESH::read(const GENDRIVER & genDriver)
{
  const char* LOC = "MESH::read(const MED_MED_DRIVER & genDriver): ";
  BEGIN_OF_MED(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->open();
      _drivers[index]->read();
      _drivers[index]->close();
      // ? FINALEMENT PAS BESOIN DE L'EXCEPTION ?
    }

  END_OF_MED(LOC);

}

/*! Sets the MESH name. Name should not exceed MED_TAILLE_NOM
length defined in Med (i.e. 32 characters).*/
inline void MESH::setName(string name)
{
  _name=name; //NOM interne à la classe
}

/*! \if MEDMEM_ug
\addtogroup MESH_general
@{
\endif
*/

/*!
Gets the MESH name.*/
inline string MESH::getName() const
{
  return _name;
}

inline void MESH::setMeshDimension(int dim)
{
  _meshDimension = dim;
}

/*! Set the MESH description */
/*! \ifnot MEDMEM_ug
Sets the MESH description. Description should not exceed MED_TAILLE_DESC length defined in Med (i.e. 200 characters).
\endif */
inline void MESH::setDescription(string description)
{
  _description = description; //NOM interne à la classe
}

/*! Gets the MESH description. The string returned contains
a short description of the mesh, which is stored for
information purposes only.*/
inline string MESH::getDescription() const
{
  return _description;
}

/*! Gets the dimension of the space in which the
mesh is described (2 for planar meshes, 3 for volumes and
3D surfaces) . */
inline int MESH::getSpaceDimension() const
{
  return _spaceDimension;
}

/*! Gets the dimension of the mesh (2 for 2D- and 3D-surfaces, 3 for volumes). */
inline int MESH::getMeshDimension() const
{
  return _meshDimension;
}

/*! \if MEDMEM_ug
The retrieval of general information about a mesh is illustrated in the following C++ example. Its Python equivalent can be found in \a MESHgeneral.py.
This example illustrates how to retrieve the name, description, mesh and space dimensions.

\example MESHgeneral.cxx
@}
\endif
*/
/*! \if MEDMEM_ug
\addtogroup MESH_nodes
@{
\endif
*/
/*! Gets the number of nodes used in the mesh. */
inline int MESH::getNumberOfNodes() const
{
  return _numberOfNodes;
}

/*! \ifnot MEDMEM_ug
 Gets the COORDINATES object. Use it only if you need COORDINATES informations not provided by the MESH class.
\endif
*/
inline const COORDINATE * MESH::getCoordinateptr() const
{
//   checkGridFillCoords();
  return _coordinate;
}

/*! Retrieves the system in which coordinates are given (CARTESIAN,CYLINDRICAL,SPHERICAL). */
inline string MESH::getCoordinatesSystem() const
{
  return _coordinate->getCoordinatesSystem();
}

/*! Gets the whole coordinates array in a given interlacing mode. The interlacing mode are :
  - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
  - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2
 */
inline const double * MESH::getCoordinates(MED_EN::medModeSwitch Mode) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinates(Mode);
}

/*! Gets the coordinate number \a number on axis \a axis.*/
inline const double MESH::getCoordinate(int number, int axis) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinate(number,axis);
}

/*! Gets a pointer to the coordinate names array.
*/
inline const string * MESH::getCoordinatesNames() const
{
  return _coordinate->getCoordinatesNames();
}

/*! Gets a pointer to the coordinate units array.
*/
inline const string * MESH::getCoordinatesUnits() const
{
  return _coordinate->getCoordinatesUnits();
}
/*!\if MEDMEM_ug

Here is a small C++ example program for which the
Python version may be found in
\a MESHcoordinates.py.

\example MESHcoordinates.cxx

@}
\endif
*/

//  int * MESH::getNodesNumbers() const
//  {
//    return nodesNumbers;
//  }

/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*! Gets the number of different geometric types for a given entity type.

    For example getNumberOfTypes(MED_CELL) would return 3 if the MESH
    have some MED_TETRA4, MED_PYRA5 and MED_HEXA8 in it.
    If entity is not defined, returns 0.
    If there is no connectivity, returns an exception.

    \param entity  entity type (MED_CELL, MED_FACE, MED_EDGE, MED_NODE, MED_ALL_ENTITIES)
*/
inline int MESH::getNumberOfTypes(MED_EN::medEntityMesh entity) const
{
  MESSAGE_MED("MESH::getNumberOfTypes(medEntityMesh entity) : "<<entity);
  if (entity == MED_EN::MED_NODE)
    return 1;
//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getNumberOfTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Gets the list of geometric types used by a given entity.
 If entity is not defined, it returns an exception.
\param entity Entity type must be MED_CELL, MED_FACE, MED_EDGE or MED_ALL_ENTITIES.
 Passing MED_NODE as an entity type will throw an exception.
*/
inline const MED_EN::medGeometryElement * MESH::getTypes(MED_EN::medEntityMesh entity) const
{
  if (entity == MED_EN::MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : No medGeometryElement with MED_NODE entity !"));
  // return un tableau de taille 1 contenant MED_NONE, comme les supports pour etre coherent avec getNumberOfTypes ???? PG

//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGeometricTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : Connectivity not defined !"));
}
/*! \if MEDMEM_ug @} \endif */

/*!
  Get the whole list of CELLMODEL used by cells of given type (medEntityMesh).
 Passing MED_NODE as an entity type will throw an exception.
*/
inline const CELLMODEL * MESH::getCellsTypes(MED_EN::medEntityMesh Entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getCellsTypes(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellsTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Get an array (it should deleted after use) of the whole list of CELLMODEL
  Name of a given type (medEntityMesh).

  REMARK : Don't use MED_NODE as medEntityMesh
*/
inline string * MESH::getCellTypeNames(MED_EN::medEntityMesh Entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getCellTypeNames(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellTypesName( medEntityMesh ) : Connectivity not defined !"));
}
/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*! Returns an array of size NumberOfTypes+1 which contains, for each
    geometric type of the given entity, the first global element number
    of this type.

    For exemple, if we have a mesh with 5 triangles and 4 quadrangle :
    - size of GlobalNumberingIndex is 3
    - GlobalNumberingIndex[0]=1 (the first type)
    - GlobalNumberingIndex[1]=6 (the second type)
    - GlobalNumberingIndex[2]=10
*/
inline const int * MESH::getGlobalNumberingIndex(MED_EN::medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGlobalNumberingIndex(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}
/*!
  Returns the number of elements of given geometric type of given entity. Returns 0 if query is not defined.

  Example :
  - getNumberOfElements(MED_NODE,MED_NONE) : number of nodes
  - getNumberOfElements(MED_NODE,MED_TRIA3) : returns 0 (not defined)
  - getNumberOfElements(MED_FACE,MED_TRIA3) : returns number of triangle
  elements defined in face entity (0 if not defined)
  - getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) : returns total number
  of elements defined in cell entity
 */
inline int MESH::getNumberOfElements(MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  if (entity==MED_EN::MED_NODE)
    if ((Type==MED_EN::MED_NONE)|(Type==MED_EN::MED_ALL_ELEMENTS))
      return _numberOfNodes;
    else
      return 0;
  //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"wrong medGeometryElement with MED_NODE"));
  else
    {
//       checkGridFillConnectivity();
      if (_connectivity != (CONNECTIVITY*)NULL)
        return _connectivity->getNumberOf(entity,Type);
      else
        return 0;
      //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"connectivity not defined !"));
    }
}
/*! \if MEDMEM_ug

This example shows the use of connectivity retrieval methods on a mesh which
corresponds to the four-element mesh given in figure ??. Note
the use of connectivity and connnectivity index tables, and the
offsets used to convert Fortran-style numbering to C arrays.

The output of this program reads :
\code
Number of types : 3
Type #0
Element 1 : 1 2 3
Type #1
Element 1 : 2 4 5 3
Element 2 : 5 6 7 8
Type #2
Element 1 : 4 6 5 10 11 9
\endcode

\example read_example.cxx

A more complete example involving descending connectivities
can be found in \c MESHconnectivities.cxx and \c MESHconnectivities.py.

@}
\endif
*/
/*!
  Returns true if the wanted connectivity exist, else returns false
  (to use before a getSomething method).
 */
inline bool MESH::existConnectivity(MED_EN::medConnectivity connectivityType, MED_EN::medEntityMesh entity) const
{
//   checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivity(connectivityType,entity);
}
/*!
  Returns true if the wanted polygons connectivity exist, else returns false
*/
inline bool MESH::existPolygonsConnectivity(MED_EN::medConnectivity connectivityType, MED_EN::medEntityMesh entity) const
{
  if (_connectivity == (CONNECTIVITY*) NULL)
    throw MEDEXCEPTION("MESH::existPolygonsConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existPolygonsConnectivity(connectivityType,entity);
}
/*!
  Returns true if the wanted polyhedron connectivity exist, else returns false
*/
inline bool MESH::existPolyhedronConnectivity(MED_EN::medConnectivity connectivityType, MED_EN::medEntityMesh entity) const
{
  if (_connectivity == (CONNECTIVITY*) NULL)
    throw MEDEXCEPTION("MESH::existPolyhedronConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existPolyhedronConnectivity(connectivityType,entity);
}

/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*!
  Returns the geometric type of global element number \a Number of entity \a Entity.

  Throw an exception if \a Entity is not defined or if \a Numberis wrong.
 */
inline MED_EN::medGeometryElement MESH::getElementType(MED_EN::medEntityMesh Entity,int Number) const
{
  //  checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementType(Entity,Number);
}
/*!
\if MEDMEM_ug
@}
\endif
 */

/*!\if MEDMEM_ug
\addtogroup MESH_poly
@{
\endif
*/

/*!
  Method equivalent to getElementType except that it includes not only classical Types but polygons/polyhedra also.
 */
MED_EN::medGeometryElement MESH::getElementTypeWithPoly(MED_EN::medEntityMesh Entity, int Number) const
{
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementTypeWithPoly(Entity,Number);
}

/*!
\if MEDMEM_ug
@}
\endif
 */

/*!
  Calculates the required connectivity. Returns an exception if this could not be
  done. Do nothing if connectivity already exist.
 */

inline void MESH::calculateConnectivity(MED_EN::medModeSwitch Mode,MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  if (Mode==MED_EN::MED_FULL_INTERLACE)
    _connectivity->calculateConnectivity(ConnectivityType,entity);
  else
    throw MEDEXCEPTION(LOCALIZED("MESH::calculateConnectivity : only for MED_FULL_INTERLACE mode"));
}
/*!
 Returns the corresponding length of the array returned by MESH::getConnectivity with exactly the same arguments.
 Used particulary for wrapping CORBA and python.
 */
inline int MESH::getConnectivityLength(MED_EN::medModeSwitch Mode,MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  int nbOfElm = getNumberOfElements(entity,Type);
  int size;

  if (Type == MED_EN::MED_ALL_ELEMENTS)
    {
      size = getConnectivityIndex(ConnectivityType,entity)[nbOfElm]-1;
    }
  else
    {
      if ( ConnectivityType==MED_EN::MED_NODAL )
        size = nbOfElm*(((int) Type)%100);
      else
        size = _connectivity->getConnectivityLength(ConnectivityType,entity,Type); // issue 19983
    }
  return size;
}

/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*!
  Returns the required connectivity in mode \a Mode for the
  geometric type \a Type of the entity type \a entity.
\a ConnectivityType specifies descending or nodal connectivity.

  To get connectivity for all geometric type, use \a Mode=MED_FULL_INTERLACE
  and \a Type=MED_ALL_ELEMENTS.
  You must also get the corresponding index array.
 */
inline const int * MESH::getConnectivity(MED_EN::medModeSwitch Mode,MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  //  checkGridFillConnectivity();
  if (Mode==MED_EN::MED_FULL_INTERLACE)
    return _connectivity->getConnectivity(ConnectivityType,entity,Type);
  throw MEDEXCEPTION(LOCALIZED("MESH::getConnectivity : only for MED_FULL_INTERLACE mode"));
}
/*!
  Returns the required index array for a connectivity received in
  MED_FULL_INTERLACE mode and MED_ALL_ELEMENTS type.

  This array allows to find connectivity of each element.

  Example : Connectivity of i-th element (1<=i<=NumberOfElement) begins
  at index ConnectivityIndex[i-1] and ends at index ConnectivityIndex[i]-1
  in Connectivity array (Connectivity[ConnectivityIndex[i-1]-1] is the
  first node of the element)
 */
inline const int * MESH::getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  return _connectivity->getConnectivityIndex(ConnectivityType, entity);
}

/*!\if MEDMEM_ug @} \endif */




/*!
 Returns the corresponding length of the array returned by MESH::getPolygonsConnectivity.
 Used particulary for wrapping CORBA and python.
 */
inline int MESH::getPolygonsConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                               MED_EN::medEntityMesh Entity) const
{
  return getPolygonsConnectivityIndex (ConnectivityType,Entity)[ getNumberOfPolygons(Entity) ] - 1;
}

/*!\if MEDMEM_ug
\addtogroup MESH_poly
@{
\endif
*/
/*!
  Return the required connectivity of polygons for the given entity.
  You must also get the corresponding index array.
 */
inline const int * MESH::getPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
                                                 MED_EN::medEntityMesh Entity) const
{
  return _connectivity->getPolygonsConnectivity(ConnectivityType,Entity);
}
/*!
  Return the required index array for polygons connectivity.
 */
inline const int * MESH::getPolygonsConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                      MED_EN::medEntityMesh Entity) const
{
  return _connectivity->getPolygonsConnectivityIndex(ConnectivityType,Entity);
}
/*!
  Return the number of polygons.
 */
inline int MESH::getNumberOfPolygons(MED_EN::medEntityMesh Entity) const
{
  return _connectivity->getNumberOfPolygons(Entity);
}
/*! \if MEDMEM_ug @} \endif */

/*!
 Returns the corresponding length of the array returned by MESH::getPolyhedronConnectivity with exactly the same arguments.
 Used particulary for wrapping CORBA and python.
 */
inline int MESH::getPolyhedronConnectivityLength(MED_EN::medConnectivity ConnectivityType) const
{
  if ( ConnectivityType == MED_EN::MED_DESCENDING )
    return getPolyhedronIndex (ConnectivityType) [ getNumberOfPolyhedron() ] - 1;

  return getPolyhedronFacesIndex()[ getNumberOfPolyhedronFaces() ] - 1;
}

/*! \if MEDMEM_ug
\addtogroup MESH_poly
@{ \endif */

/*!
  Returns the required connectivity of polyhedron :
  - in nodal mode, it gives you the polyhedron faces nodal connectivity.
  - in descending mode, it gives you the polyhedron faces list.
  You must also get :
  - faces index and polyhedron index arrays in nodal mode.
  - polyhedron index array in descending mode.
 */
inline const int * MESH::getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const
{
  return _connectivity->getPolyhedronConnectivity(ConnectivityType);
}
/*!
  Returns the index array of polyhedron faces in nodal mode.
  You must also get the polyhedron index array.
 */
inline const int * MESH::getPolyhedronFacesIndex() const
{
  return _connectivity->getPolyhedronFacesIndex();
}
/*!
  Returns the required polyhedron index array.
 */
inline const int * MESH::getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const
{
  return _connectivity->getPolyhedronIndex(ConnectivityType);
}
/*!
  Returns the number of polyhedron faces.
 */
inline int MESH::getNumberOfPolyhedronFaces() const
{
  return _connectivity->getNumberOfPolyhedronFaces();
}
/*!
  Returns the number of polyhedron.
 */
inline int MESH::getNumberOfPolyhedron() const
{
  return _connectivity->getNumberOfPolyhedron();
}

/*!\if MEDMEM_ug
@}
\endif
*/

/*!
  Returns the corresponding length of the array returned by MESH::getReverseConnectivity with exactly the same arguments.
  Used particulary for wrapping CORBA and python.
 */

inline int MESH::getReverseConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                                    MED_EN::medEntityMesh Entity) const
{
  int spaceDim = getSpaceDimension();
  int nb;

  if (ConnectivityType == MED_EN::MED_NODAL)
    {
      nb = getNumberOfNodes();
    }
  else
    {
      if (spaceDim == 2)
        nb = getNumberOfElements(MED_EN::MED_EDGE,
                                        MED_EN::MED_ALL_ELEMENTS);
      else if (spaceDim == 3)
        nb = getNumberOfElements(MED_EN::MED_FACE,
                                        MED_EN::MED_ALL_ELEMENTS);
    }
  return getReverseConnectivityIndex(ConnectivityType,Entity)[nb]-1;
}
/*!
  Returns the reverse connectivity required by ConnectivityType :
  - If ConnectivityType=MED_NODAL : returns connectivity node-cell
  - If ConnectivityType=MED_DESCENDING : returns connectivity face-cell

  You must get ReverseConnectivityIndex array to use it.
 */

inline const int * MESH::getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivity : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivity(ConnectivityType,Entity);
}
/*!
  Returns the corresponding length of the array returned by MESH::getReverseConnectivityIndex with exactly the same arguments.
  Used particulary for wrapping CORBA and python.
 */
inline int MESH::getReverseConnectivityIndexLength(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity) const
{
  int spaceDim = getSpaceDimension();

  if (ConnectivityType == MED_EN::MED_NODAL)
    {
      return getNumberOfNodes()+1;
    }
  else
    {
      if (spaceDim == 2)
        return getNumberOfElements(MED_EN::MED_EDGE,MED_EN::MED_ALL_ELEMENTS)+1;
      else if (spaceDim == 3)
        return getNumberOfElements(MED_EN::MED_FACE,MED_EN::MED_ALL_ELEMENTS)+1;
      else
        throw MEDEXCEPTION("Invalid dimension");
    }
}
/*!
  Returns the index array required by ConnectivityType.

  This array allow to find reverse connectivity of each elements.

  Example : Reverse connectivity of i^{th} elements (1<=i<=NumberOfElement)
  begin at index ReverseConnectivityIndex[i-1] and end at index
  ReverseConnectivityIndex[i]-1
  in ReverseConnectivity array (
  ReverseConnectivity[ReverseConnectivityIndex[i-1]-1]
  is the first value)
 */
inline const int * MESH::getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivityIndex(ConnectivityType,Entity);
}


/*!
Retrieves the number of families in the mesh for entity type \a entity
*/
inline int MESH::getNumberOfFamilies (MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE :
    return _familyNode.size();
  case MED_EN::MED_CELL :
    return _familyCell.size();
  case MED_EN::MED_FACE :
    return _familyFace.size();
  case MED_EN::MED_EDGE :
    return _familyEdge.size();
  default :
    throw MEDEXCEPTION("MESH::getNumberOfFamilies : Unknown entity");
  }
}

/*! Retrieves the number of groups in the mesh for entity type \a entity
 */

inline int MESH::getNumberOfGroups (MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE :
    return _groupNode.size();
  case MED_EN::MED_CELL :
    return _groupCell.size();
  case MED_EN::MED_FACE :
    return _groupFace.size();
  case MED_EN::MED_EDGE :
    return _groupEdge.size();
  default :
    throw MEDEXCEPTION("MESH::getNumberOfGroups : Unknown entity");
  }
}
/*! Returns the families of type \a entity present in the mesh as a vector of pointers */
const vector<MEDMEM::FAMILY*> MESH::getFamilies(MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE :
    return _familyNode;
  case MED_EN::MED_CELL :
    return _familyCell;
  case MED_EN::MED_FACE :
    return _familyFace;
  case MED_EN::MED_EDGE :
    return _familyEdge;
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
}



const MEDMEM::FAMILY* MESH::getFamily(MED_EN::medEntityMesh entity, int i) const
{
  if (i<=0)
    throw MEDEXCEPTION("MESH::getFamily(i) : argument i must be > 0");
  vector<FAMILY*> Family;
  switch (entity) {
  case MED_EN::MED_NODE : {
    Family = _familyNode;
    break;
  }
  case MED_EN::MED_CELL : {
    Family = _familyCell;
    break;
  }
  case MED_EN::MED_FACE : {
    Family = _familyFace;
    break;
  }
  case MED_EN::MED_EDGE : {
    Family = _familyEdge;
    break;
  }
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
  if (i>(int)Family.size())
    throw MEDEXCEPTION("MESH::getFamily(entity,i) : argument i must be <= _numberOfFamilies");
  return Family[i-1];
}


//    int * get_() {
//      return ;
//    }

//inline void MESH::write(const string & driverName)  {
//  write(0,driverName);
//}

inline bool MESH::getIsAGrid()
{
  SCRUTE_MED(_isAGrid);

  return _isAGrid;
}

}

#include "MEDMEM_Support.hxx"

namespace MEDMEM {

//Create a new FIELD that should be deallocated based on a SUPPORT that should be deallocated too.
template<class T>
FIELD<T, FullInterlace> * MESH::mergeFields(const vector< FIELD<T, FullInterlace> * > & others,
                                            bool meshCompare)
{
  const char * LOC = "MESH::mergeFields(const vector< FIELD<T>* >& others,bool meshCompare): ";
  BEGIN_OF_MED(LOC);
  int i,j;
  if(others.size()==0)
    return 0;
  vector<SUPPORT *> sup;
  typename vector< FIELD<T, FullInterlace>* >::const_iterator iter;
  iter = others.begin();
  MED_EN::med_type_champ valueType = (*iter)->getValueType();
  for(iter=others.begin();iter!=others.end();iter++)
    {
      MED_EN::med_type_champ valueTypeIter = (*iter)->getValueType();
      if (valueTypeIter != valueType)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Fields vector have not the same value type"));

      sup.push_back((SUPPORT *)(*iter)->getSupport());
    }
  iter=others.begin();
  SUPPORT *retSup=mergeSupports(sup);
  int retNumberOfComponents=(*iter)->getNumberOfComponents();
  FIELD<T, FullInterlace> *ret=new FIELD<T, FullInterlace>(retSup, retNumberOfComponents);
  T* valuesToSet=(T*)ret->getValue();
  int nbOfEltsRetSup=retSup->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  T* tempValues=new T[retNumberOfComponents];
  if(retSup->isOnAllElements())
    {
      for(i=0;i<nbOfEltsRetSup;i++)
        {
          bool found=false;
          for(iter=others.begin();iter!=others.end() && !found;iter++)
            {
              found=(*iter)->getValueOnElement(i+1,tempValues);
              if(found)
                for(j=0;j<retNumberOfComponents;j++)
                  valuesToSet[i*retNumberOfComponents+j]=tempValues[j];
            }
        }
    }
  else
    {
      const int *eltsRetSup=retSup->getNumber(MED_EN::MED_ALL_ELEMENTS);
      for(i=0;i<nbOfEltsRetSup;i++)
        {
          bool found=false;
          for(iter=others.begin();iter!=others.end() && !found;iter++)
            {
              found=(*iter)->getValueOnElement(eltsRetSup[i],tempValues);
              if(found)
                for(j=0;j<retNumberOfComponents;j++)
                  valuesToSet[i*retNumberOfComponents+j]=tempValues[j];
            }
          if(!found)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Merging error due to an error in merging support"));
        }
    }
  delete [] tempValues;
  END_OF_MED(LOC);
  return ret;
}

}

#endif /* MESH_HXX */
