#ifndef MESH_HXX
#define MESH_HXX

#include <string>
#include <vector>

#include "utilities.h"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

//#include "MEDMEM_Support.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"

// Add your own driver header (step 2)
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"

#include "MEDMEM_VtkMeshDriver.hxx"



//class GENDRIVER;
//class MED_MESH_RDONLY_DRIVER;
//class MED_MESH_WRONLY_DRIVER;

using namespace MED_EN;

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

namespace MEDMEM {

template <class T> class FIELD;

class CELLMODEL;
class FAMILY;
class GROUP;
class SUPPORT;
class MESH
{


public :

  // ------- Drivers Management Part
protected:

  //-----------------------//
  class INSTANCE
  //-----------------------//
  {
  public:
    virtual GENDRIVER * run(const string & fileName, MESH * ptrMesh) const = 0;
  };

  //-------------------------------------------------------//
  template <class T> class INSTANCE_DE : public INSTANCE
  //-------------------------------------------------------//
  {
  public :
    GENDRIVER * run(const string & fileName, MESH * ptrMesh) const
    { return new T(fileName,ptrMesh); }
  };

  // Add a similar line for your personnal driver (step 3)

  static INSTANCE_DE<MED_MESH_RDWR_DRIVER>  inst_med;
  static INSTANCE_DE<GIBI_MESH_RDWR_DRIVER> inst_gibi;
  static INSTANCE_DE<VTK_MESH_DRIVER> inst_vtk;

  //static INSTANCE_DE<VTK_DRIVER>   inst_vtk  ;
  static const INSTANCE * const instances[];

  // ------ End of Drivers Management Part


  //-----------------------//
  //   Attributes
  //-----------------------//

protected :

  string        _name; // A POSITIONNER EN FCT DES IOS ?

  mutable COORDINATE *   _coordinate;
  mutable CONNECTIVITY * _connectivity;

  int   _spaceDimension;
  int   _meshDimension;
  int   _numberOfNodes;

  int             _numberOfNodesFamilies;  //INUTILE ? -> _familyNode.size()
  vector<FAMILY*> _familyNode;  // array of size _numberOfNodesFamilies;
  int             _numberOfCellsFamilies;
  vector<FAMILY*> _familyCell;  // array of size _numberOfCellsFamilies;
  int             _numberOfFacesFamilies;
  vector<FAMILY*> _familyFace;  // array of size _numberOfFacesFamilies;
  int             _numberOfEdgesFamilies;
  vector<FAMILY*> _familyEdge;  // array of size _numberOfEdgesFamilies;

  int   	 _numberOfNodesGroups; //INUTILE ?
  vector<GROUP*> _groupNode;    // array of size _numberOfNodesGroups;
  int   	 _numberOfCellsGroups; //INUTILE ?
  vector<GROUP*> _groupCell;    // array of size _numberOfCellsGroups;
  int   	 _numberOfFacesGroups; //INUTILE ?
  vector<GROUP*> _groupFace;    // array of size _numberOfFacesGroups;
  int   	 _numberOfEdgesGroups; //INUTILE ?
  vector<GROUP*> _groupEdge;    // array of size _numberOfEdgesGroups;
  // list of all Group

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use

  bool           _isAGrid;      // am I a GRID or not

  //-----------------------//
  //   Methods
  //-----------------------//

//   inline void checkGridFillCoords() const;
//   inline void checkGridFillConnectivity() const;
  bool isEmpty() const;
  // if this->_isAGrid, assure that _coordinate and _connectivity are filled

public :

  // Add your personnal driver line (step 2)
  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

  friend class MED_MED_RDONLY_DRIVER;
  friend class MED_MED_WRONLY_DRIVER;
  friend class MED_MED_RDWR_DRIVER;

  friend class GIBI_MESH_RDONLY_DRIVER;
  friend class GIBI_MESH_WRONLY_DRIVER;
  friend class GIBI_MESH_RDWR_DRIVER;

  friend class VTK_MESH_DRIVER;

  void init();
  MESH();
  MESH(MESH &m);
  MESH & operator=(const MESH &m);
  MESH( driverTypes driverType, const string & fileName="",
	const string & meshName="") throw (MEDEXCEPTION);
  virtual ~MESH();
  friend ostream & operator<<(ostream &os, const MESH &my);

  int  addDriver(driverTypes driverType,
		 const string & fileName  ="Default File Name.med",
		 const string & driverName="Default Mesh Name");
  int  addDriver(GENDRIVER & driver);
  void rmDriver(int index=0);

  virtual void read(int index=0);
  inline void read(const MED_MED_DRIVER & genDriver);
  inline void write(int index=0, const string & driverName = "");
  inline void write(const MED_MED_DRIVER & genDriver);

  //  void calculateReverseConnectivity();
  //  void createFaces(); 	//Faces creation => full constituent informations
  //  void buildConstituent(); // calculate descendent connectivity + face-cell connectivity


  inline void 	      setName(string name);

  inline string       getName() const;
  inline int 	      getSpaceDimension() const;
  inline int 	      getMeshDimension() const;
  inline bool	      getIsAGrid();

  inline int 	                    getNumberOfNodes() const;
  virtual inline const COORDINATE * getCoordinateptr() const;
  inline string                     getCoordinatesSystem() const;
  virtual inline const double *     getCoordinates(medModeSwitch Mode) const;
  virtual inline const double       getCoordinate(int Number,int Axis) const;
  inline const string *             getCoordinatesNames() const;
  inline const string *             getCoordinatesUnits() const;
  //inline int *                    getNodesNumbers();

  virtual inline int             getNumberOfTypes(medEntityMesh Entity) const;
  virtual inline const medGeometryElement * getTypes(medEntityMesh Entity) const;
  virtual inline const CELLMODEL * getCellsTypes(medEntityMesh Entity) const;
  virtual const int * getGlobalNumberingIndex(medEntityMesh Entity) const;
  virtual inline int getNumberOfElements(medEntityMesh Entity,
					 medGeometryElement Type) const;
  virtual inline bool existConnectivity(medConnectivity ConnectivityType,
					medEntityMesh Entity) const;

  virtual inline medGeometryElement getElementType(medEntityMesh Entity,
						   int Number) const;
  virtual inline void calculateConnectivity(medModeSwitch Mode,
					    medConnectivity ConnectivityType,
					    medEntityMesh Entity) const ;
  virtual inline const int * getConnectivity(medModeSwitch Mode,
					     medConnectivity ConnectivityType,
					     medEntityMesh Entity, 
					     medGeometryElement Type) const;
  virtual inline const int * getConnectivityIndex(medConnectivity ConnectivityType,
						  medEntityMesh Entity) const;
  virtual int                 getElementNumber(medConnectivity ConnectivityType, 
                                               medEntityMesh Entity, 
                                               medGeometryElement Type, 
                                               int * connectivity) const;

  virtual inline const int * getReverseConnectivity(medConnectivity ConnectivityType,
						    medEntityMesh Entity=MED_CELL) const;
  virtual inline const int * getReverseConnectivityIndex(medConnectivity ConnectivityType,
							 medEntityMesh Entity=MED_CELL) const;

  virtual int                          getNumberOfFamilies(medEntityMesh Entity) const;
  virtual inline const vector<FAMILY*> getFamilies(medEntityMesh Entity) const;
  virtual inline const FAMILY*         getFamily(medEntityMesh Entity,int i) const;
  virtual int          		       getNumberOfGroups(medEntityMesh Entity) const;
  virtual inline const vector<GROUP*>  getGroups(medEntityMesh Entity) const;
  virtual inline const GROUP*          getGroup(medEntityMesh Entity,int i) const;
  virtual inline const CONNECTIVITY* getConnectivityptr() const;
  virtual SUPPORT *                    getBoundaryElements(medEntityMesh Entity) 
						throw (MEDEXCEPTION);
  // problème avec le maillage dans le support : 
  //            le pointeur n'est pas const, mais sa valeur oui. A voir !!! PG

  SUPPORT *                            getSkin(const SUPPORT * Support3D) 
						throw (MEDEXCEPTION);

  //  Node DonneBarycentre(const Cell &m) const;
  virtual FIELD<double>* getVolume (const SUPPORT * Support) const 
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
  //				throw (MEDEXCEPTION); // Il faut preciser !

  /*!
    return a SUPPORT pointer on the union of all SUPPORTs in Supports.
    You should delete this pointer after use to avois memory leaks.
  */
  SUPPORT * mergeSupports(const vector<SUPPORT *> Supports) const throw (MEDEXCEPTION) ;

  /*!
    return a SUPPORT pointer on the intersection of all SUPPORTs in Supports.
    The (SUPPORT *) NULL pointer is returned if the intersection is empty.
    You should delete this pointer after use to avois memory leaks.
   */
  SUPPORT * intersectSupports(const vector<SUPPORT *> Supports) const throw (MEDEXCEPTION) ;
};
};

using namespace MEDMEM;
// ---------------------------------------
// 		Methodes Inline
// ---------------------------------------

inline const CONNECTIVITY* MESH::getConnectivityptr() const
{
//   checkGridFillConnectivity();
  return _connectivity;
}

// inline void MESH::read(int index/*=0*/)
// {
//   const char * LOC = "MESH::read(int index=0) : ";
//   BEGIN_OF(LOC);

//   if (_drivers[index]) {
//     _drivers[index]->open();
//     _drivers[index]->read();
//     _drivers[index]->close();
//   }
//   else
//     throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
//                                      << "The index given is invalid, index must be between  0 and |"
//                                      << _drivers.size()
//                                      )
//                           );
//   END_OF(LOC);
// }

/*! Write all the content of the MESH using driver referenced by the handler <index>*/
inline void MESH::write(int index/*=0*/, const string & driverName/* = ""*/)
{
  const char * LOC = "MESH::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF(LOC);

  if ( _drivers[index] ) {
    _drivers[index]->open();
    if (driverName != "") _drivers[index]->setMeshName(driverName);
    _drivers[index]->write();
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     )
                          );
  END_OF(LOC);
}

// This method is MED specific : don't use it
// must be private.
inline void MESH::write(const MED_MED_DRIVER & genDriver)
{
  const char * LOC = "MESH::write(const MED_MED_DRIVER & genDriver): ";
  BEGIN_OF(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->open();
      _drivers[index]->write();
      _drivers[index]->close();
      // ? FINALEMENT PAS BESOIN DE L'EXCEPTION ?
    }

  END_OF(LOC);

}

// This method is MED specific : don't use it
// must be private.
inline void MESH::read(const MED_MED_DRIVER & genDriver)
{
  const char * LOC = "MESH::read(const MED_MED_DRIVER & genDriver): ";
  BEGIN_OF(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->open();
      _drivers[index]->read();
      _drivers[index]->close();
      // ? FINALEMENT PAS BESOIN DE L'EXCEPTION ?
    }

  END_OF(LOC);

}

/*! Set the MESH name */
inline void MESH::setName(string name)
{
  _name=name; //NOM interne à la classe
}

/*! Get the MESH name */
inline string MESH::getName() const
{
  return _name;
}

/*! Get the dimension of the space */
inline int MESH::getSpaceDimension() const
{
  return _spaceDimension;
}

/*! Get the dimension of the MESH */
inline int MESH::getMeshDimension() const
{
  return _meshDimension;
}

/*! Get the number of nodes used in the MESH */
inline int MESH::getNumberOfNodes() const
{
  return _numberOfNodes;
}

/*! Get the COORDINATES object. Use it only if you need COORDINATES informations not provided by the MESH class.*/
inline const COORDINATE * MESH::getCoordinateptr() const
{
//   checkGridFillCoords();
  return _coordinate;
}

/*! Get the system in which coordinates are given (CARTESIAN,CYLINDRICAL,SPHERICAL) __??MED_CART??__. */
inline string MESH::getCoordinatesSystem() const
{
  return _coordinate->getCoordinatesSystem();
}

/*! Get the whole coordinates array in a given interlacing mode. The interlacing mode are :
  - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
  - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2
 */
inline const double * MESH::getCoordinates(medModeSwitch Mode) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinates(Mode);
}

/*! Get the coordinate n° number on axis n°axis*/
inline const double MESH::getCoordinate(int number, int axis) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinate(number,axis);
}

/*! Get the coordinate names array ("x       ","y       ","z       ")
  of size n*MED_TAILLE_PNOM
*/
inline const string * MESH::getCoordinatesNames() const
{
  return _coordinate->getCoordinatesNames();
}

/*! Get the coordinate unit names array ("cm       ","cm       ","cm       ")
  of size n*MED_TAILLE_PNOM
*/
inline const string * MESH::getCoordinatesUnits() const
{
  return _coordinate->getCoordinatesUnits();
}
//  int * MESH::getNodesNumbers() const
//  {
//    return nodesNumbers;
//  }

/*! Get the number of different geometric types for a given entity type.

    For exemple getNumberOfTypes(MED_CELL) would return 3 if the MESH
    have some MED_TETRA4, MED_PYRA5 and MED_HEXA6 in it.

    medEntityMesh entity : MED_CELL, MED_FACE, MED_EDGE, MED_NODE, MED_ALL_ENTITIES

    If entity is not defined, return 0.

    If there is no connectivity, return an exception.
*/
inline int MESH::getNumberOfTypes(medEntityMesh entity) const
{
  MESSAGE("MESH::getNumberOfTypes(medEntityMesh entity) : "<<entity);
  if (entity == MED_NODE)
    return 1;
//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getNumberOfTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Get the list of geometric types used by a given entity.
  medEntityMesh entity : MED_CELL, MED_FACE, MED_EDGE, MED_ALL_ENTITIES

  REM : Don't use MED_NODE

  If entity is not defined, return an exception.
*/
inline const medGeometryElement * MESH::getTypes(medEntityMesh entity) const
{
  if (entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : No medGeometryElement with MED_NODE entity !"));
  // return un tableau de taille 1 contenant MED_NONE, comme les supports pour etre coherent avec getNumberOfTypes ???? PG

//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGeometricTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Get the whole list of CELLMODEL used by cells of given type (medEntityMesh).

  REMARK : Don't use MED_NODE as medEntityMesh
*/
inline const CELLMODEL * MESH::getCellsTypes(medEntityMesh Entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getCellsTypes(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellsTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*! Return an array of size NumbreOfTypes+1 which contains, for each
    geometric type of the given entity, the first global element number
    of this type.

    For exemple, if we have a mesh with 5 triangles and 4 quadrangle :
    - size of GlobalNumberingIndex is 3
    - GlobalNumberingIndex[0]=1 (the first type)
    - GlobalNumberingIndex[1]=6 (the second type)
    - GlobalNumberingIndex[2]=10
*/
inline const int * MESH::getGlobalNumberingIndex(medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGlobalNumberingIndex(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}
/*!
  Return the number of element of given geometric type of given entity. Return 0 if query is not defined.

  Example :
  - getNumberOfElements(MED_NODE,MED_NONE) : number of node
  - getNumberOfElements(MED_NODE,MED_TRIA3) : return 0 (not defined)
  - getNumberOfElements(MED_FACE,MED_TRIA3) : return number of triangles
  elements defined in face entity (0 if not defined)
  - getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) : return total number
  of elements defined in cell entity
 */
inline int MESH::getNumberOfElements(medEntityMesh entity, medGeometryElement Type) const
{
  //  const char * LOC = "MESH::getNumberOfElements(medEntityMesh,medGeometryElement) : ";
  if (entity==MED_NODE)
    if ((Type==MED_NONE)|(Type==MED_ALL_ELEMENTS))
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
/*!
  Return true if the wanted connectivity exist, else return false
  (to use before a getSomething method).
 */
inline bool MESH::existConnectivity(medConnectivity connectivityType, medEntityMesh entity) const
{
//   checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivity(connectivityType,entity);
}
/*!
  Return the geometric type of global element Number of entity Entity.

  Throw an exception if Entity is not defined or Number are wrong (too big).
 */
inline medGeometryElement MESH::getElementType(medEntityMesh Entity,int Number) const
{
  //  checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementType(Entity,Number);
}
/*!
  Calculate the ask connectivity. Return an exception if this could not be
  done. Do nothing if connectivity already exist.
 */

inline void MESH::calculateConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  if (Mode==MED_FULL_INTERLACE)
    _connectivity->calculateConnectivity(ConnectivityType,entity);
  else
    throw MEDEXCEPTION(LOCALIZED("MESH::calculateConnectivity : only for MED_FULL_INTERLACE mode"));
}
/*!
  Return the required connectivity in the right mode for the given
  geometric type of the given entity.

  To get connectivity for all geometric type, use Mode=MED_FULL_INTERLACE
  and Type=MED_ALL_ELEMENTS.
  You must also get the corresponding index array.
 */
inline const int * MESH::getConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh entity, medGeometryElement Type) const
{
  //  checkGridFillConnectivity();
  if (Mode==MED_FULL_INTERLACE)
    return _connectivity->getConnectivity(ConnectivityType,entity,Type);
  throw MEDEXCEPTION(LOCALIZED("MESH::getConnectivity : only for MED_FULL_INTERLACE mode"));
}
/*!
  Return the required index array for a connectivity received in
  MED_FULL_ENTERLACE mode and MED_ALL_ELEMENTS type.

  This array allow to find connectivity of each elements.

  Example : Connectivity of i^{th} elements (1<=i<=NumberOfElement) begin
  at index ConnectivityIndex[i-1] and end at index ConnectivityIndex[i]-1
  in Connectivity array (Connectivity[ConnectivityIndex[i-1]-1] is the
  first value)
 */
inline const int * MESH::getConnectivityIndex(medConnectivity ConnectivityType,medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  return _connectivity->getConnectivityIndex(ConnectivityType, entity);
}
/*!
  Return the reverse connectivity required by ConnectivityType :
  - If ConnectivityType=MED_NODAL : return connectivity node-cell
  - If ConnectivityType=MED_DESCENDING : return connectivity face-cell

  You must get ReverseConnectivityIndex array to use it.
 */
inline const int * MESH::getReverseConnectivity(medConnectivity ConnectivityType,medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivity : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivity(ConnectivityType,Entity);
}
/*!
  Return the index array required by ConnectivityType.

  This array allow to find reverse connectivity of each elements.

  Example : Reverse connectivity of i^{th} elements (1<=i<=NumberOfElement)
  begin at index ReverseConnectivityIndex[i-1] and end at index
  ReverseConnectivityIndex[i]-1
  in ReverseConnectivity array (
  ReverseConnectivity[ReverseConnectivityIndex[i-1]-1]
  is the first value)
 */
inline const int * MESH::getReverseConnectivityIndex(medConnectivity ConnectivityType,medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivityIndex(ConnectivityType,Entity);
}


inline int MESH::getNumberOfFamilies (medEntityMesh entity) const
{
  switch (entity) {
  case MED_NODE :
    return _numberOfNodesFamilies;
  case MED_CELL :
    return _numberOfCellsFamilies;
  case MED_FACE :
    return _numberOfFacesFamilies;
  case MED_EDGE :
    return _numberOfEdgesFamilies;
  default :
    throw MEDEXCEPTION("MESH::getNumberOfFamilies : Unknown entity");
  }
}
inline int MESH::getNumberOfGroups (medEntityMesh entity) const
{
  switch (entity) {
  case MED_NODE :
    return _numberOfNodesGroups;
  case MED_CELL :
    return _numberOfCellsGroups;
  case MED_FACE :
    return _numberOfFacesGroups;
  case MED_EDGE :
    return _numberOfEdgesGroups;
  default :
    throw MEDEXCEPTION("MESH::getNumberOfGroups : Unknown entity");
  }
}
const vector<MEDMEM::FAMILY*> MESH::getFamilies(medEntityMesh entity) const
{
  switch (entity) {
  case MED_NODE :
    return _familyNode;
  case MED_CELL :
    return _familyCell;
  case MED_FACE :
    return _familyFace;
  case MED_EDGE :
    return _familyEdge;
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
}

const vector<GROUP*> MESH::getGroups(medEntityMesh entity) const
{
  switch (entity) {
  case MED_NODE :
    return _groupNode;
  case MED_CELL :
    return _groupCell;
  case MED_FACE :
    return _groupFace;
  case MED_EDGE :
    return _groupEdge;
  default :
    throw MEDEXCEPTION("MESH::getGroups : Unknown entity");
  }
}

const MEDMEM::FAMILY* MESH::getFamily(medEntityMesh entity, int i) const
{
  if (i<=0)
    throw MEDEXCEPTION("MESH::getFamily(i) : argument i must be > 0");
  int NumberOfFamilies = 0;
  vector<FAMILY*> Family;
  switch (entity) {
  case MED_NODE : {
    NumberOfFamilies = _numberOfNodesFamilies;
    Family = _familyNode;
    break;
  }
  case MED_CELL : {
    NumberOfFamilies = _numberOfCellsFamilies;
    Family = _familyCell;
    break;
  }
  case MED_FACE : {
    NumberOfFamilies = _numberOfFacesFamilies;
    Family = _familyFace;
    break;
  }
  case MED_EDGE : {
    NumberOfFamilies = _numberOfEdgesFamilies;
    Family = _familyEdge;
    break;
  }
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
  if (i>NumberOfFamilies)
    throw MEDEXCEPTION("MESH::getFamily(entity,i) : argument i must be <= _numberOfFamilies");
  return Family[i-1];
}

const GROUP* MESH::getGroup(medEntityMesh entity, int i) const
{
  const char * LOC = "MESH::getGroup(medEntityMesh entity, int i) : ";
  if (i<=0)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i must be > 0"));
  int NumberOfGroups = 0;
  vector<GROUP*> Group;
  switch (entity) {
  case MED_NODE : {
    NumberOfGroups = _numberOfNodesGroups;
    Group = _groupNode;
    break;
  }
  case MED_CELL : {
    NumberOfGroups = _numberOfCellsGroups;
    Group = _groupCell;
    break;
  }
  case MED_FACE : {
    NumberOfGroups = _numberOfFacesGroups;
    Group = _groupFace;
    break;
  }
  case MED_EDGE : {
    NumberOfGroups = _numberOfEdgesGroups;
    Group = _groupEdge;
    break;
  }
  default :
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Unknown entity"));
  }
  if (i>NumberOfGroups)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i="<<i<<" must be <= _numberOfGroups="<<NumberOfGroups));
  return Group[i-1];
}


//    int * get_() {
//      return ;
//    }

//inline void MESH::write(const string & driverName)  {
//  write(0,driverName);
//}

inline bool MESH::getIsAGrid()
{
  SCRUTE(_isAGrid);

  return _isAGrid;
}

#endif /* MESH_HXX */
