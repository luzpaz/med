using namespace std;
/*
  File MEDMEM_Meshing.cxx
  $Header$
*/

#include <string>

#include "utilities.h"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
using namespace MEDMEM;

/*! Create an empty %MESH. */
MESHING::MESHING(): MESH()
{
  MESSAGE("MESHING::MESHING()");
  SCRUTE(_coordinate);
  SCRUTE(_connectivity);
}

MESHING::~MESHING()
{
  MESSAGE("Deletinh MESHING !!");
}

/*! Set the dimension of the space */
void MESHING::setSpaceDimension(const int SpaceDimension)
{
  _spaceDimension = SpaceDimension ;
}

/* Set the dimension of the MESHING */
// void MESHING::setMeshDimension(const int MeshDimension)
// {
//    _meshDimension = MeshDimension ;
// }

/*! Set the number of nodes used in the %MESH */
void MESHING::setNumberOfNodes(const int NumberOfNodes)
{
  _numberOfNodes = NumberOfNodes ;
}

/*! 
  Set the whole coordinates array in a given system and interlacing mode.
  The system coordinates are :
    - "CARTESIAN"
    - "CYLINDRICAL"
    - "SPHERICAL"
  The interlacing mode are :
    - %MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
    - %MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2
*/
void MESHING::setCoordinates(const int SpaceDimension,
			     const int NumberOfNodes,
			     const double * Coordinates,
			     const string System,
			     const medModeSwitch Mode)
{
  setSpaceDimension(SpaceDimension);
  setNumberOfNodes(NumberOfNodes);

  SCRUTE(_coordinate);
  SCRUTE(_connectivity);
  //if (NULL != _coordinate) delete _coordinate;

  _coordinate = new COORDINATE(SpaceDimension,
			       NumberOfNodes,
			       Mode);
  _coordinate->setCoordinates(Mode,Coordinates);
  _coordinate->setCoordinatesSystem(System);
}

/*! Set the system in which coordinates are given
      - "CARTESIAN"
      - "CYLINDRICAL"
      - "SPHERICAL". */
void MESHING::setCoordinatesSystem(const string System)
  throw (MEDEXCEPTION)
{
  if (NULL == _coordinate)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setCoordinatesSystem : no coordinates defined"));
  _coordinate->setCoordinatesSystem(System);
}

/*! Set the coordinate names array ("x       ","y       ","z       ")
  of size n*%MED_TAILLE_PNOM
*/
void MESHING::setCoordinatesNames(const string * name)
{
//    int SpaceDimension = getSpaceDimension() ;
//    _coordinate->setCoordinatesNames(SpaceDimension,name);
  _coordinate->setCoordinatesNames(name);
}

/*!
  Set the (i+1)^th component of coordinate names array
  ("x       ","y       ","z       ") of size n*%MED_TAILLE_PNOM
*/
void MESHING::setCoordinateName(const string name, const int i)
{
  _coordinate->setCoordinateName(name,i);
}

/*! Set the coordinate unit names array ("cm       ","cm       ","cm       ")
  of size n*%MED_TAILLE_PNOM
*/
void MESHING::setCoordinatesUnits(const string * units)
{
//    int SpaceDimension = getSpaceDimension() ;
//    _coordinate->setCoordinatesUnits(SpaceDimension,units);
  _coordinate->setCoordinatesUnits(units);
}

/*!
  Set the (i+1)^th component of the coordinate unit names array
  ("cm       ","cm       ","cm       ") of size n*%MED_TAILLE_PNOM
*/
void MESHING::setCoordinateUnit(const string unit, const int i)
{
  _coordinate->setCoordinateUnit(unit,i);
}

/*!
  Create a new connectivity object with the given number of type and
  entity. If a connectivity already exist, delete it !

  For exemple setNumberOfTypes(3,MED_CELL) create a connectivity with 3 
  medGeometryElement in MESH for MED_CELL entity (like MED_TETRA4, 
  MED_PYRA5 and MED_HEXA6 for example).

  Return an exception if could not create the connectivity (as if we set 
  MED_FACE connectivity before MED_CELL).
*/
void MESHING::setNumberOfTypes(const int NumberOfTypes,
			       const medEntityMesh Entity) 
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setNumberOfTypes( medEntityMesh ) : ";

  // No defined for MED_NODE !
  if (Entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not required with MED_NODE !"));

  if (MED_CELL == Entity) {
    SCRUTE(_connectivity);
//     if (_connectivity != (CONNECTIVITY *) NULL)
//       delete _connectivity ;
    _connectivity = new CONNECTIVITY(NumberOfTypes,Entity) ;

  } else {

    if (_connectivity == NULL) // we must have defined MED_CELL connectivity
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_CELL defined !"));

    if (MED_FACE == Entity)
      if (3 != getSpaceDimension())
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_FACE could be defined in non 3D space !"));
    
    if (MED_EDGE == Entity)
      if (3 == getSpaceDimension()) {
	if (!_connectivity->existConnectivity(MED_NODAL,MED_FACE))
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_FACE defined !"));
      } else {
	if (2 != getSpaceDimension())
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Could not set connectivity on MED_EDGE !"));
      }
    // all rigth, we could create connectivity !
    CONNECTIVITY * myConnectivity = new CONNECTIVITY(NumberOfTypes,Entity) ;
    _connectivity->setConstituent(myConnectivity);
  }
}

/*!
  Set the list of geometric types used by a given entity.
  medEntityMesh entity could be : MED_CELL, MED_FACE, MED_EDGE

  REM : Don't use MED_NODE and MED_ALL_ENTITIES

  If entity is not defined, throws an exception.
*/
void MESHING::setTypes(const medGeometryElement * Types,
		       const medEntityMesh entity)
  throw (MEDEXCEPTION)
{
  if (entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setTypes : Not defined with MED_NODE entity !"));

  if (_connectivity == NULL)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setTypes : No connectivity defined !"));

  _connectivity->setGeometricTypes(Types,entity) ;
}

/*!
  Set the number of elements for each geometric type of given entity.

  Example : setNumberOfElements({12,23},MED_FACE)
  if we have two type of face (MED_TRIA3 and MED_QUAD4), 
  we set 12 triangles and 23 quadrangles.
*/
void MESHING::setNumberOfElements(const int * NumberOfElements,
				  medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setNumberOfElements(const int *, medEntityMesh) : " ;

  if (Entity==MED_NODE)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined with MED_NODE entity !"));

  if (_connectivity == (CONNECTIVITY*)NULL)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity defined !"));

  int NumberOfTypes = getNumberOfTypes(Entity) ;
  int * Count = new int[NumberOfTypes+1] ;
  Count[0]=1 ;
  for (int i=0; i<NumberOfTypes; i++)
    Count[i+1]=Count[i]+NumberOfElements[i] ;
  _connectivity->setCount(Count,Entity) ;
  delete[] Count ;
}

/*!
  Set the nodal connectivity for one geometric type of the given entity.

  Example : setConnectivity({1,2,3,1,4,2},MED_FACE,MED_TRIA3)
  Define 2 triangles face defined with nodes 1,2,3 and 1,4,2.
*/
void MESHING::setConnectivity(const int * Connectivity,
			      const medEntityMesh Entity,
			      const medGeometryElement Type)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setConnectivity : " ;

  if (Entity==MED_NODE)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined with MED_NODE entity !"));

  if (_connectivity == (CONNECTIVITY*)NULL)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity defined !"));

  _connectivity->setNodal(Connectivity,Entity,Type) ;
}

/*!
  NOT YET IMPLEMENTED !! WARNING
*/
void MESHING::setConnectivities (const int * ConnectivityIndex,
				 const int * ConnectivityValue,
				 const medConnectivity ConnectivityType,
				 const medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setConnectivities : " ;
  SCRUTE(Entity);
  SCRUTE(ConnectivityType);
  SCRUTE(ConnectivityValue);
  SCRUTE(ConnectivityIndex);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not Yet Implemented :: Warning !"));
}

/*!
*/

// void MESHING::setGroup(const string name,
// 		       const string description,
// 		       const int NumberOfElements,
// 		       const int * ElementsNumbers,
// 		       const medEntityMesh Entity)
// {
//   GROUP * myGroup = new GROUP() ;
//   myGroup->setMesh(*this) ;
//   myGroup->setName(name) ;
//   myGroup->setDescription(description) ;
//   myGroup->setEntity(Entity) ;
//   // medEntityMesh and medGeometryElement ???
//   myGroup->setNumberOfGeometricType(NumberOfType) ;
//   myGroup->setGeometricType(Type) ;
//   myGroup->setNumberOfGaussPoint(NumberOfGaussPoint) ;
//   myGroup->setNumberOfElements(NumberOfElements) ;
//   myGroup->setNumber(Number) ;
// }

void MESHING::addGroup(const GROUP & Group)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::addGroup : " ;

  GROUP * myGroup = new GROUP(Group) ;
  switch(Group.getEntity()){
  case MED_CELL : {
    _groupCell.push_back(myGroup);
    break;
  }
  case MED_FACE : {
     _groupFace.push_back(myGroup);
    break;
  }
  case MED_EDGE : {
     _groupEdge.push_back(myGroup);
    break;
  }
  case MED_NODE : {
     _groupNode.push_back(myGroup);
    break;
  }
  default :
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Entity !"));
  }
}
