//  MED MEDMEM : MED files in memory
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : MEDMEM_Grid.hxx
//  Author : Edward AGAPOV (eap)
//  Module : MED
//  $Header$

#ifndef MEDMEM_Grid_HeaderFile
#define MEDMEM_Grid_HeaderFile

#include "MEDMEM_Mesh.hxx"

// class containing structured mesh data

class GRID: public MESH
{
 protected:
  //-----------------------//
  //   Fields
  //-----------------------//
  
  // 1. grid type: MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED
  med_grid_type     _gridType;
  
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

  CONNECTIVITY * makeConnectivity (const medEntityMesh      Entity,
                                   const medGeometryElement Geometry,
                                   const int                NbEntities,
                                   const int                NbNodes,
                                   int *                    NodeNumbers);
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
  GRID(const med_grid_type type);
  GRID(const GRID &m);
  GRID( driverTypes driverType, const string & fileName="",const string & meshName="");
  GRID & operator=(const GRID &m);
  virtual ~GRID();
  virtual void init();

  void fillCoordinates();
  void fillConnectivity();
  // fill _coordinates and _connectivity of MESH if not yet done

  inline void makeUnstructured();
  // fill both coordinates and connectivity of MESH

  void fillMeshAfterRead();
  // fill some fields (_numberOfNodes, etc.) after reading

  void writeUnstructured(int index=0, const string & driverName = "");
  // write a Grid as an Unstructured mesh

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

  inline int getNodeNumber(const int i, const int j=0, const int k=0);
  // return a NODE number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < getArrayLength( X_Axis )
  inline int getCellNumber(const int i, const int j=0, const int k=0) ;
  // return a CELL number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < (getArrayLength( X_Axis )-1)

  int getEdgeNumber(const int Axis, const int i, const int j=0, const int k=0) throw (MEDEXCEPTION) ;
  // return an EDGE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects an edge of ones having same (i,j,k):
  //  * an EDGE going along given Axis.
  // Exception for Axis out of range or when there is no edges in the grid (1D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( X_Axis )-1)
  // 0 <= X < getArrayLength( NOT_X_Axis )

  int getFaceNumber(const int Axis, const int i, const int j=0, const int k=0) throw (MEDEXCEPTION) ;
  // return a FACE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects a face of ones having same (i,j,k):
  //  * a FACE which is normal to given Axis
  // Exception for Axis out of range or when there is no faces in the grid (1,2D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( NOT_X_Axis )-1)
  // 0 <= X < getArrayLength( X_Axis )
  

  void getNodePosition(const int Number, int& i, int& j, int& k) throw (MEDEXCEPTION) ;
  void getCellPosition(const int Number, int& i, int& j, int& k) throw (MEDEXCEPTION) ;
  void getEdgePosition(const int Number, int& Axis, int& i, int& j, int& k) throw (MEDEXCEPTION) ;
  void getFacePosition(const int Number, int& Axis, int& i, int& j, int& k) throw (MEDEXCEPTION) ;
  // return position (i,j,k) of an entity #Number
  // Axis: [1,2,3], see get*Number for details
  // Exception for Number out of range


  //  Access to fields

  inline med_grid_type getGridType() const;
  // return MED_CARTESIAN, MED_POLAR or MED_BODY_FITTED

  int getArrayLength( const int Axis ) throw (MEDEXCEPTION);
  // return array length. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1-3] range

  const double getArrayValue (const int Axis, const int i) throw (MEDEXCEPTION) ;
  // return i-th array component. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1 - 3] range
  // exception if i is out of range [0 - (getArrayLength(Axis)-1)];

  //  Setting fields

  inline void setGridType(med_grid_type gridType);

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

};


  //----------------------------------//
  //   Inline Methods Implementation
  //----------------------------------//

inline med_grid_type GRID::getGridType() const
{
  return _gridType;
}
//=======================================================================
//function : getNodeNumber
//purpose  : 
//=======================================================================

inline int GRID::getNodeNumber(const int i, const int j, const int k)
{
  return 1 + i + _iArrayLength * j + _iArrayLength * _jArrayLength * k;
}

//=======================================================================
//function : getCellNumber
//purpose  : 
//=======================================================================

inline int GRID::getCellNumber(const int i, const int j, const int k)
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

inline void GRID::setGridType(med_grid_type gridType)
{
  _gridType = gridType;
}

#endif
