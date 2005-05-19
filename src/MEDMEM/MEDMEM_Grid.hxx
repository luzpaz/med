// File      : MEDMEM_Grid.hxx
// Created   : Wed Dec 18 08:35:26 2002
// Descr     : class containing structured mesh data

// Author    : Edward AGAPOV (eap)
// Project   : SALOME Pro
// Module    : MED 
// Copyright : Open CASCADE
// $Header$

#ifndef MEDMEM_Grid_HeaderFile
#define MEDMEM_Grid_HeaderFile

#include "MEDMEM_Mesh.hxx"

// class containing structured mesh data

namespace MEDMEM {
class GRID: public MESH
{
 protected:
  //-----------------------//
  //   Fields
  //-----------------------//
  
  // 1. grid type: MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED
  MED_EN::med_grid_type     _gridType;
  
  // 2. node coordinates
  // For MED_BODY_FITTED MESH::_coordinate is used
  
  // 2.1. i component array: X for MED_CARTESIAN, R for MED_POLAR
  int               _iArrayLength;
  double*           _iArray;
  // 2.2. j component array: Y for MED_CARTESIAN, Theta for MED_POLAR
  int               _jArrayLength;
  double*           _jArray;
  // 2.3. k component array: Z for both MED_CARTESIAN and MED_POLAR
  int               _kArrayLength;
  double*           _kArray;

  // 3. flags to know that _coordinates and _connectivity of MESH are filled
  bool              _is_coordinates_filled;
  bool              _is_connectivity_filled;

  //-----------------------//
  //   Protected Methods
  //-----------------------//

  CONNECTIVITY * makeConnectivity (const MED_EN::medEntityMesh Entity, const MED_EN::medGeometryElement Geometry,
				   const int NbEntities, int NbNodes, int nbMeshNodes,
				   const int * NodeNumbers) const ;
  // creates nodal connectivity

  
  // Suppose a READ Driver to do the following except filling _[ijk]Array's
  // 1) fill MESH fields:
  //    * _name
  //    * _spaceDimension
  //    * _meshDimension
  //    * _numberOfNodesFamilies, _numberOfCellsFamilies, ect
  //    * _familyNode, _familyCell, etc
  //    * _numberOfNodesGroups, _numberOfCellsGroups, etc
  //    * _groupNode, _groupCell, etc
  //    * _
  // 2) create MESH::_coordinate without setting _coordinate->_coordinate and set: 
  //    * _coordinate->_coordinateSystem
  //    * _coordinate->_coordinateName
  //    * _coordinate->_coordinateUnit
  

 public:
  //-----------------------//
  //   Methods
  //-----------------------//

  GRID();
  GRID(const MED_EN::med_grid_type type);
  GRID(const GRID &m);
  GRID( driverTypes driverType, const string & fileName="",const string & meshName="");
  GRID(const std::vector<std::vector<double> >& xyz_array,const std::vector<std::string>& coord_name,
       const std::vector<std::string>& coord_unit, const MED_EN::med_grid_type type=MED_EN::MED_CARTESIAN);
  GRID & operator=(const GRID &m);
  virtual ~GRID();
  virtual void init();

  void fillCoordinates() const ;
  void fillConnectivity() const ;
  // fill _coordinates and _connectivity of MESH if not yet done

  inline void makeUnstructured();
  // fill both coordinates and connectivity of MESH

  void fillMeshAfterRead();
  // fill some fields (_numberOfNodes, etc.) after reading

  void writeUnstructured(int index=0, const string & driverName = "");
  // write a Grid as an Unstructured mesh

  void read(int index=0);

  //   API Methods returning fields of MESH that are filled while reading.
  //   So they need not to be redefined

//   string                getName() const;
//   int                   getSpaceDimension();
//   int                   getMeshDimension();
//   string                getCoordinatesSystem();
//   int                   getNumberOfNodes();
//   string *              getCoordinatesNames();
//   string *              getCoordinatesUnits();
//   int                   getNumberOfFamilies(medEntityMesh Entity);
//   vector<FAMILY*>       getFamilies(medEntityMesh Entity);
//   FAMILY*               getFamily(medEntityMesh Entity,int i);
//   int                   getNumberOfGroups(medEntityMesh Entity);
//   vector<GROUP*>        getGroups(medEntityMesh Entity);
//   GROUP*                getGroup(medEntityMesh Entity,int i);


// Since a MESH itself knows if it is a GRID, it calls fillConnectivity()
// or fillCoordinates() whenever needed. So no redifinition of the following methods

//   const double *        getCoordinates(medModeSwitch Mode);
//   COORDINATE *          getCoordinateptr();
//   const double          getCoordinate(int Number,int Axis);
//   int                   getNumberOfTypes(medEntityMesh Entity);
//   medGeometryElement *  getTypes(medEntityMesh Entity);
//   CELLMODEL *           getCellsTypes(medEntityMesh Entity);
//   medGeometryElement    getElementType(medEntityMesh Entity,int Number) ;
//   int                   getNumberOfElements(medEntityMesh Entity,medGeometryElement Type);
//   int                   getElementNumber(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type, int * connectivity) ;
//   int *                 getGlobalNumberingIndex(medEntityMesh Entity);
//   int *                 getConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh Entity, medGeometryElement Type);
//   int *                 getConnectivityIndex(medConnectivity ConnectivityType,medEntityMesh Entity);
//   int *                 getReverseConnectivity(medConnectivity ConnectivityType);
//   int *                 getReverseConnectivityIndex(medConnectivity ConnectivityType);
//   bool                  existConnectivity(medConnectivity ConnectivityType,medEntityMesh Entity);
//   void                  calculateConnectivity(medModeSwitch Mode,medConnectivity ConnectivityType,medEntityMesh Entity);
//   FIELD<double>*        getVolume(const SUPPORT * Support) throw (MEDEXCEPTION) ;
//   FIELD<double>*        getArea(const SUPPORT * Support) throw (MEDEXCEPTION) ;
//   FIELD<double>*        getLength(const SUPPORT * Support) throw (MEDEXCEPTION) ;
//   FIELD<double>*        getNormal(const SUPPORT * Support) throw (MEDEXCEPTION) ;
//   FIELD<double>*        getBarycenter(const SUPPORT * Support) throw (MEDEXCEPTION) ;


  // Specific GRID methods

  inline int getNodeNumber(const int i, const int j=0, const int k=0) const;
  // return a NODE number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < getArrayLength( X_Axis )
  inline int getCellNumber(const int i, const int j=0, const int k=0) const ;
  // return a CELL number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < (getArrayLength( X_Axis )-1)

  int getEdgeNumber(const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION) ;
  // return an EDGE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects an edge of ones having same (i,j,k):
  //  * an EDGE going along given Axis.
  // Exception for Axis out of range or when there is no edges in the grid (1D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( X_Axis )-1)
  // 0 <= X < getArrayLength( NOT_X_Axis )

  int getFaceNumber(const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION) ;
  // return a FACE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects a face of ones having same (i,j,k):
  //  * a FACE which is normal to given Axis
  // Exception for Axis out of range or when there is no faces in the grid (1,2D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( NOT_X_Axis )-1)
  // 0 <= X < getArrayLength( X_Axis )
  

  void getNodePosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getCellPosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getEdgePosition(const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getFacePosition(const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  // return position (i,j,k) of an entity #Number
  // Axis: [1,2,3], see get*Number for details
  // Exception for Number out of range


  //  Access to fields

  inline MED_EN::med_grid_type getGridType() const;
  // return MED_CARTESIAN, MED_POLAR or MED_BODY_FITTED

  int getArrayLength( const int Axis ) const throw (MEDEXCEPTION);
  // return array length. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1-3] range

  const double getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION) ;
  // return i-th array component. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1 - 3] range
  // exception if i is out of range [0 - (getArrayLength(Axis)-1)];

  inline const COORDINATE * getCoordinateptr() const;

  inline const double * getCoordinates(MED_EN::medModeSwitch Mode) const;

  inline const double getCoordinate(int Number,int Axis) const;

  inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;

  inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;

  inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;

  const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;

  inline int getNumberOfElements(MED_EN::medEntityMesh Entity,
				 MED_EN::medGeometryElement Type) const;

  inline bool existConnectivity(MED_EN::medConnectivity ConnectivityType,
				MED_EN::medEntityMesh Entity) const;

  inline MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh Entity,
					   int Number) const;

  inline void calculateConnectivity(MED_EN::medModeSwitch Mode,
				    MED_EN::medConnectivity ConnectivityType,
				    MED_EN::medEntityMesh Entity) const ;

  inline const CONNECTIVITY* getConnectivityptr() const;

  inline const int * getConnectivity(MED_EN::medModeSwitch Mode,
				     MED_EN::medConnectivity ConnectivityType,
				     MED_EN::medEntityMesh Entity, 
				     MED_EN::medGeometryElement Type) const;

  inline const int * getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
					  MED_EN::medEntityMesh Entity) const;

  inline const int * getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,
					    MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;

  inline const int * getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
						 MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;

  //  Setting fields

  inline void setGridType(MED_EN::med_grid_type gridType);

  friend class IMED_MESH_RDONLY_DRIVER;
  friend class IMED_MESH_WRONLY_DRIVER;
  friend class MED_MESH_RDONLY_DRIVER21;
  friend class MED_MESH_WRONLY_DRIVER21;
  friend class MED_MESH_RDONLY_DRIVER22;
  friend class MED_MESH_WRONLY_DRIVER22;
};


  //----------------------------------//
  //   Inline Methods Implementation
  //----------------------------------//

inline MED_EN::med_grid_type GRID::getGridType() const
{
  return _gridType;
}
//=======================================================================
//function : getNodeNumber
//purpose  : 
//=======================================================================

inline int GRID::getNodeNumber(const int i, const int j, const int k) const
{
  return 1 + i + _iArrayLength * j + _iArrayLength * _jArrayLength * k;
}

//=======================================================================
//function : getCellNumber
//purpose  : 
//=======================================================================

inline int GRID::getCellNumber(const int i, const int j, const int k) const
{
  return 1 + i + (_iArrayLength-1) * j + (_iArrayLength-1) * (_jArrayLength-1) * k;
}

//=======================================================================
//function : makeUnstructured
//purpose : fill coordinates and connectivity of MESH
//=======================================================================

inline void GRID::makeUnstructured()
{
  fillCoordinates();
  fillConnectivity();
}

//=======================================================================
//function : setGridType
//purpose : set the _gridType field od the class GRID
//=======================================================================

inline void GRID::setGridType(MED_EN::med_grid_type gridType)
{
  _gridType = gridType;
}

/*! Get the COORDINATES object. Use it only if you need COORDINATES informations not provided by the GRID class via the MESH class.*/
inline const COORDINATE * GRID::getCoordinateptr() const
{
  fillCoordinates();
  return _coordinate;
}

/*! Get the whole coordinates array in a given interlacing mode. The interlacing mode are :
  - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
  - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2
 */
inline const double * GRID::getCoordinates(MED_EN::medModeSwitch Mode) const
{
  fillCoordinates();
  return _coordinate->getCoordinates(Mode);
}

/*! Get the coordinate n° number on axis n°axis*/
inline const double GRID::getCoordinate(int number, int axis) const
{
  fillCoordinates();
  return _coordinate->getCoordinate(number,axis);
}

/*! Get the number of different geometric types for a given entity type.

    medEntityMesh entity : MED_CELL, MED_FACE, MED_EDGE, MED_NODE,
    MED_ALL_ENTITIES

*/
inline int GRID::getNumberOfTypes(MED_EN::medEntityMesh entity) const
{
  MESSAGE("GRID::getNumberOfTypes(medEntityMesh entity) : "<<entity);
    return 1; // a grid has one type
}

/*!
  Get the whole list of CELLMODEL used by cells of given type (medEntityMesh).

  REMARK : Don't use MED_NODE as medEntityMesh
*/
inline const CELLMODEL * GRID::getCellsTypes(MED_EN::medEntityMesh Entity) const
{
  fillConnectivity();

  if (_connectivity != NULL)
    return _connectivity->getCellsTypes(Entity);
  throw MEDEXCEPTION(LOCALIZED("GRID::getCellsTypes( medEntityMesh ) : Connectivity not defined !"));
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
inline const int * GRID::getGlobalNumberingIndex(MED_EN::medEntityMesh entity) const
{
  fillConnectivity();

  if (_connectivity != NULL)
    return _connectivity->getGlobalNumberingIndex(entity);
  throw MEDEXCEPTION(LOCALIZED("GRID::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Return the number of element of given geometric type of given entity. Return 0 if query is not defined.
*/
inline int GRID::getNumberOfElements(MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  int numberOfElements=0;
    
    // Cas où le nombre d'éléments n'est pas nul
    if (entity==MED_EN::MED_FACE && (Type==MED_EN::MED_QUAD4 || Type==MED_EN::MED_ALL_ELEMENTS) && _spaceDimension>2)
	numberOfElements=(_iArrayLength-1)*(_jArrayLength-1);
    
    else if (entity==MED_EN::MED_EDGE && (Type==MED_EN::MED_SEG2 || Type==MED_EN::MED_ALL_ELEMENTS) && _spaceDimension>1)
	numberOfElements=_iArrayLength-1;
    
    else if (entity==MED_EN::MED_NODE && (Type==MED_EN::MED_NONE || Type==MED_EN::MED_ALL_ELEMENTS) && _spaceDimension>0)
	numberOfElements=_numberOfNodes;
    
    else if (entity==MED_EN::MED_CELL && _spaceDimension==3 && (Type==MED_EN::MED_HEXA8 || Type==MED_EN::MED_ALL_ELEMENTS) )
	numberOfElements=(_iArrayLength-1)*(_jArrayLength-1)*(_kArrayLength-1);
    
    else if (entity==MED_EN::MED_CELL && _spaceDimension==2 && (Type==MED_EN::MED_QUAD4 || Type==MED_EN::MED_ALL_ELEMENTS))
	numberOfElements=(_iArrayLength-1)*(_jArrayLength-1);
    
    else if (entity==MED_EN::MED_CELL && _spaceDimension==1 && (Type==MED_EN::MED_SEG2 || Type==MED_EN::MED_ALL_ELEMENTS) )
	numberOfElements=_iArrayLength-1;

    MESSAGE("GRID::getNumberOfElements - entity=" << entity << " Type=" << Type);
    MESSAGE("_spaceDimension=" << _spaceDimension << "  numberOfElements=" << numberOfElements);

    return numberOfElements;
}


/*!
  Return true if the wanted connectivity exist, else return false
  (to use before a getSomething method).
 */
inline bool GRID::existConnectivity(MED_EN::medConnectivity connectivityType, MED_EN::medEntityMesh entity) const
{
  fillConnectivity();

  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("GRID::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivity(connectivityType,entity);
}

/*!
  Return the geometric type of global element Number of entity Entity.

  Throw an exception if Entity is not defined or Number are wrong (too big).
 */
inline MED_EN::medGeometryElement GRID::getElementType(MED_EN::medEntityMesh Entity,int Number) const
{
  fillConnectivity();

  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("GRID::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementType(Entity,Number);
}

/*!
  Calculate the ask connectivity. Return an exception if this could not be
  done. Do nothing if connectivity already exist.
 */

inline void GRID::calculateConnectivity(MED_EN::medModeSwitch Mode,MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity) const
{
  fillConnectivity();

  if (Mode==MED_EN::MED_FULL_INTERLACE)
    _connectivity->calculateConnectivity(ConnectivityType,entity);
  else
    throw MEDEXCEPTION(LOCALIZED("GRID::calculateConnectivity : only for MED_FULL_INTERLACE mode"));
}

inline const CONNECTIVITY* GRID::getConnectivityptr() const
{
  fillConnectivity();

  return _connectivity;
}

/*!
  Return the required connectivity in the right mode for the given
  geometric type of the given entity.

  To get connectivity for all geometric type, use Mode=MED_FULL_INTERLACE
  and Type=MED_ALL_ELEMENTS.
  You must also get the corresponding index array.
 */
inline const int * GRID::getConnectivity(MED_EN::medModeSwitch Mode,MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  fillConnectivity();

  if (Mode==MED_EN::MED_FULL_INTERLACE)
    return _connectivity->getConnectivity(ConnectivityType,entity,Type);
  throw MEDEXCEPTION(LOCALIZED("GRID::getConnectivity : only for MED_FULL_INTERLACE mode"));
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
inline const int * GRID::getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity) const
{
  fillConnectivity();

  return _connectivity->getConnectivityIndex(ConnectivityType, entity);
}

/*!
  Return the reverse connectivity required by ConnectivityType :
  - If ConnectivityType=MED_NODAL : return connectivity node-cell
  - If ConnectivityType=MED_DESCENDING : return connectivity face-cell

  You must get ReverseConnectivityIndex array to use it.
 */
inline const int * GRID::getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  fillConnectivity();

  if (NULL==_connectivity)
    throw MEDEXCEPTION("GRID::getReverseConnectivity : no connectivity defined in MESH !");

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
inline const int * GRID::getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  fillConnectivity();

  if (NULL==_connectivity)
    throw MEDEXCEPTION("GRID::getReverseConnectivityIndex : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivityIndex(ConnectivityType,Entity);
}

}

#endif
