//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
/*
  File MEDMEM_Meshing.cxx
  $Header$
*/

#include <string>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

namespace MEDMEM {
/*!

\defgroup MESHING_constructors Constructors

\defgroup MESHING_general General information settings

\defgroup MESHING_nodes Node coordinates settings

\defgroup MESHING_connectivity Connectivity settings

When defining the connectivity, MED_CELL elements connectivity
should be defined first. If necessary, constituent connectivities (MED_FACE and/or MED_EDGE) can be defined afterwards. 

\warning It should be kept in mind that when defining connectivities,
elements should be sorted in ascending type order (the type order 
being defined by the number of nodes).


\defgroup MESHING_group Group creation

\defgroup MESHING_poly Polygons and Polyhedra creation

These methods belong to the meshing class and are necessary for
creating the connectivities of MED_POLYHEDRON and MED_POLYGON
elements.

*/

/*!  
\addtogroup MESHING_constructors
 @{  
*/

/*! Creates an empty MESH. */
MESHING::MESHING(): MESH()
{
  MESSAGE_MED("MESHING::MESHING()");
  SCRUTE_MED(_coordinate);
  SCRUTE_MED(_connectivity);
}
/*!  @}  */

MESHING::~MESHING()
{
  MESSAGE_MED("Deleting MESHING !!");
}

/*!  
\addtogroup MESHING_general
 @{  */

/*! Sets the dimension of the space. */
void MESHING::setSpaceDimension(int SpaceDimension)
{
  _spaceDimension = SpaceDimension ;
}

/*! Sets the dimension of the mesh. */
void MESHING::setMeshDimension(const int MeshDimension)
{
   _meshDimension = MeshDimension ;
   if (_connectivity)
     _connectivity->setEntityDimension(MeshDimension);
}
/*!  @}  */

/*!  
\addtogroup MESHING_nodes
 @{  */

/*! Sets the number of nodes used in the mesh. */
void MESHING::setNumberOfNodes(const int NumberOfNodes)
{
  _numberOfNodes = NumberOfNodes ;
  if (_connectivity)
    _connectivity->setNumberOfNodes(NumberOfNodes);
}

/*! 
  Sets the whole coordinates array in a given system and interlacing mode.
  The system coordinates are :
    - "MED_CART"
    - "MED_CYL"
    - "MED_SPHER"
  The interlacing mode are :
    - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
    - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2

Example :
\verbatim
MESHING myMeshing ;
const int SpaceDimension=2;
const int NumberOfNodes=6;
int * Coordinates = new int[SpaceDimension*NumberOfNodes] ;
string System="CARTESIAN";
medModeSwitch Mode = MED_FULL_INTERLACE ;
myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,System,Mode);
\endverbatim

*/
void MESHING::setCoordinates(const int SpaceDimension,
														 const int NumberOfNodes,
														 const double * Coordinates,
														 const string System,
														 const MED_EN::medModeSwitch Mode)
{
  setSpaceDimension(SpaceDimension);
  setNumberOfNodes(NumberOfNodes);

  SCRUTE_MED(_coordinate);
  SCRUTE_MED(_connectivity);
  //if (NULL != _coordinate) delete _coordinate;

  _coordinate = new COORDINATE(SpaceDimension,
			       NumberOfNodes,
			       Mode);
  _coordinate->setCoordinates(Mode,Coordinates);
  _coordinate->setCoordinatesSystem(System);
}
/*!  @}  */


/*! Sets the system in which coordinates are given (MED_CART,MED_CYL,MED_SPHER). */
void MESHING::setCoordinatesSystem(const string& System)
  throw (MEDEXCEPTION)
{
  if (NULL == _coordinate)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setCoordinatesSystem : no coordinates defined"));
  _coordinate->setCoordinatesSystem(System);
}

/*! 
 \addtogroup MESHING_general
 @{  */
/*! Sets the coordinate names array.
Coordinates names must not exceed the storage length 
defined in MED-file : MED_TAILLE_PNOM (8).

Example:
\verbatim
string coord[3]={"x","y","z"};
meshing.setCoordinatesNames(coord);
\endverbatim
*/
void MESHING::setCoordinatesNames(const string * name)
{
//    int SpaceDimension = getSpaceDimension() ;
//    _coordinate->setCoordinatesNames(SpaceDimension,name);
  _coordinate->setCoordinatesNames(name);
}



/*! \ifnot  MEDMEM_ug
  Sets the (i+1)-th component of coordinate names array.
 Coordinates names must not exceed the storage length 
defined in MED-file : MED_TAILLE_PNOM (8).
\endif

*/
void MESHING::setCoordinateName(const string name, const int i)
{
  _coordinate->setCoordinateName(name,i);
}

/*! Sets the coordinate unit names array
  of size n*MED_TAILLE_PNOM. Coordinates units must not exceed the storage length 
defined in MED-file : MED_TAILLE_PNOM (8).

Example:
\verbatim
string coord[3]={"cm","cm","cm"};
meshing.setCoordinatesUnits(coord);
\endverbatim
*/
void MESHING::setCoordinatesUnits(const string * units)
{
//    int SpaceDimension = getSpaceDimension() ;
//    _coordinate->setCoordinatesUnits(SpaceDimension,units);
  _coordinate->setCoordinatesUnits(units);
}

/*!\ifnot MEDMEM_ug
  Sets the \f$ (i+1)^th \f$  component of the coordinate unit names array
  ("cm       ","cm       ","cm       ") of size n*MED_TAILLE_PNOM
 \endif
*/
void MESHING::setCoordinateUnit(const string unit, const int i)
{
  _coordinate->setCoordinateUnit(unit,i);
}
/*!  @}  */

/*! 
 \addtogroup MESHING_connectivity
 @{ 
*/

/*!
  Creates a new connectivity object with the given number of type and
  entity. If a connectivity already exist, it is deleted by the call.

  For exemple setNumberOfTypes(3,MED_CELL) creates a connectivity with 3 
  medGeometryElement in MESH for MED_CELL entity (like MED_TETRA4, 
  MED_PYRA5 and MED_HEXA6 for example).

  Returns an exception if it could not create the connectivity (as if we set 
  MED_FACE connectivity before MED_CELL).
*/
void MESHING::setNumberOfTypes(const int NumberOfTypes,
                               const MED_EN::medEntityMesh Entity) 
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setNumberOfTypes( medEntityMesh ) : ";

  // No defined for MED_NODE !
  if (Entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not required with MED_NODE !"));

  if (MED_CELL == Entity) {
    SCRUTE_MED(_connectivity);
    //     if (_connectivity != (CONNECTIVITY *) NULL)
    //       delete _connectivity ;
    _connectivity = new CONNECTIVITY(NumberOfTypes,Entity) ;

    _connectivity->setNumberOfNodes( _numberOfNodes );

  } else {

    if (_connectivity == NULL) // we must have defined MED_CELL connectivity
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_CELL defined !"));

    if (MED_FACE == Entity)
      if (3 != getSpaceDimension())
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_FACE could be defined in non 3D space !"));

    if (MED_EDGE == Entity)
      if (3 == getMeshDimension()) {
        if (!_connectivity->existConnectivity(MED_NODAL,MED_FACE))
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity on MED_FACE defined !"));
      } else {
        if (2 != getMeshDimension())
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Could not set connectivity on MED_EDGE !"));
      }
    // all right, we could create connectivity !
    CONNECTIVITY * myConnectivity = new CONNECTIVITY(NumberOfTypes,Entity) ;
    myConnectivity->setEntityDimension(_connectivity->getEntityDimension()-1);
    myConnectivity->setNumberOfNodes(_connectivity->getNumberOfNodes());
    _connectivity->setConstituent(myConnectivity);
  }
}

/*!
  Sets the list of geometric types used by a given entity.
  medEntityMesh entity could be : MED_CELL, MED_FACE, MED_EDGE.
  This method is used to set the differents geometrics types 
({MED_TETRA4,MED_PYRA5,MED_HEXA8} for example). Geometric types should be given in increasing order of number of nodes for entity type \a entity.

  Remark : Don't use MED_NODE and MED_ALL_ENTITIES.

If \a entity is not defined, the method will throw an exception.

*/

void MESHING::setTypes(const MED_EN::medGeometryElement * Types,
                       const MED_EN::medEntityMesh        entity)
  throw (MEDEXCEPTION)
{
  if (entity == MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setTypes : Not defined with MED_NODE entity !"));

  if (_connectivity == NULL)
    throw MEDEXCEPTION(LOCALIZED("MESHING::setTypes : No connectivity defined !"));

  if ( entity == MED_EN::MED_CELL && getNumberOfTypes( entity ) > 0 )
    setMeshDimension( Types[0] / 100 );

  _connectivity->setGeometricTypes(Types,entity) ;
}

/*!
  Sets the number of elements for each geometric type of given entity.

  Example : setNumberOfElements(\{12,23\},MED_FACE);
  If there are two types of face (MED_TRIA3 and MED_QUAD4), 
  this sets 12 triangles and 23 quadrangles.
*/
void MESHING::setNumberOfElements(const int * NumberOfElements,
				  const MED_EN::medEntityMesh Entity)
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
  Sets the nodal connectivity for geometric type \a Type of  entity \a Entity.
  The nodal connectivity must be defined one element type at a time :
  \a MED_ALL_ELEMENTS is not a valid \a Type argument.

  Example :

\verbatim
MESHING myMeshing ;
myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,System,Mode);

myMeshing.setNumberOfTypes(2,MED_CELL);
myMeshing.setTypes({MED_TRIA3,MED_QUAD4},MED_CELL);
myMeshing.setNumberOfElements({3,2},MED_CELL); // 3 MED_TRIA3 and 2 MED_QUAD4
myMeshing.setConnectivity({1,2,3,6,8,9,4,5,6},MED_CELL,MED_TRIA3);
myMeshing.setConnectivity({1,3,4,5,4,5,7,8},MED_CELL,MED_QUAD4);
\endverbatim

  Example : setConnectivity({1,2,3,1,4,2},MED_FACE,MED_TRIA3)
  Define 2 triangles face defined with nodes 1,2,3 and 1,4,2.
*/
void MESHING::setConnectivity(const int * Connectivity,
			      const MED_EN::medEntityMesh Entity,
			      const MED_EN::medGeometryElement Type)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setConnectivity : " ;

  if (Entity==MED_NODE)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined with MED_NODE entity !"));

  if (_connectivity == (CONNECTIVITY*)NULL)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity defined !"));

  _connectivity->setNumberOfNodes(_numberOfNodes);
  _connectivity->setNodal(Connectivity,Entity,Type) ;
}
/*!  @}  */

/*!  
\addtogroup MESHING_poly
 @{
  */

/*! Method setting the connectivity for MED_POLYGON
elements
\param ConnectivityIndex polygon connectivity index
\param Connectivity polygon connectivity
\param nbOfPolygons number of polygons defined
\param Entity entity of the polygon

The \a Entity parameter specifies whether the polygon is the highest order
 element (MED_CELL) or it is a 2D element in a 3D mesh (MED_FACE).

This method is fully compatible with the MESHING::setConnectivity method.
The following code excerpt creates two polygons with 5 and 4 nodes respectively.
\verbatim
MESHING myMeshing ;
myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,System,Mode);

int conn_index[3]={1,6,10};
int conn[9]={1,2,3,4,5,5,4,6,7};
int nb_poly=2;
meshing.setPolygonsConnectivity(conn_index, conn, nb_poly, MED_CELL)
\endverbatim

*/

void MESHING::setPolygonsConnectivity     (const int * ConnectivityIndex,
					   const int * Connectivity,
					   int nbOfPolygons,
					   const MED_EN::medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  if (_connectivity == (CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("No connectivity defined !");

  setMeshDimension( Entity == MED_EN::MED_CELL ? 2 : 3 );

  _connectivity->setPolygonsConnectivity(MED_NODAL, Entity, Connectivity, ConnectivityIndex,ConnectivityIndex[nbOfPolygons]-1,nbOfPolygons) ;
}


/*! Method setting the connectivity for MED_POLYHEDRON
elements
\param PolyhedronIndex polyhedra connectivity index
\param FacesIndex polyhedra face connectivity index
\param Nodes polyhedra connectivity
\param nbOfPolyhedra number of polyhedra defined
\param Entity deprecated parameter 
*/
void MESHING::setPolyhedraConnectivity     (const int * PolyhedronIndex,
					    const int * FacesIndex,
					    const int * Nodes,
					    int nbOfPolyhedra,
					    const MED_EN::medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  if (_connectivity == (CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("No connectivity defined !");
  if(_connectivity->getPolyTypeRelativeTo()==MED_EN::MED_POLYHEDRA)
    {
      int nbOfFacesOfAllPolyhedra=PolyhedronIndex[nbOfPolyhedra]-1;
      _connectivity->setPolyhedronConnectivity(MED_NODAL, Nodes, PolyhedronIndex, FacesIndex[nbOfFacesOfAllPolyhedra]-1 , nbOfPolyhedra, FacesIndex, nbOfFacesOfAllPolyhedra) ;
    }
  else
    throw MEDEXCEPTION("Invalid connectivity for polyhedra !!!");
}
/*!  @}  */

/*!
  NOT YET IMPLEMENTED !! WARNING
*/
void MESHING::setConnectivities (const int * ConnectivityIndex,
				 const int * ConnectivityValue,
				 const MED_EN::medConnectivity ConnectivityType,
				 const MED_EN::medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESHING::setConnectivities : " ;
  SCRUTE_MED(Entity);
  SCRUTE_MED(ConnectivityType);
  SCRUTE_MED(ConnectivityValue);
  SCRUTE_MED(ConnectivityIndex);

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

/*!  
\addtogroup MESHING_group
 @{  */

/*!Adds group \a Group to the mesh.
This function registers the group in the list
of groups contained in the mesh, so that
when the mesh is used for file writing, the group
is written in the corresponding MED-file.
*/

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

};
/*!  @}  */
