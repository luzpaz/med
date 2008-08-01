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
/*
 File Mesh.cxx
 $Header$
*/

#include <math.h>
#include <map>
#include <sstream>

#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Formulae.hxx"
#include "MEDMEM_InterpolationHighLevelObjects.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NOPDT -1
#define MED_NONOR -1

// ------- Drivers Management Part

/*! Add a %MESH driver of type %driverTypes (MED_DRIVER, ....) associated with file fileName. The meshname used in the file
    is  driverName. addDriver returns an integer handler. */
int MESH::addDriver(driverTypes driverType,
                    const string & fileName/*="Default File Name.med"*/,
		    const string & driverName/*="Default Mesh Name"*/,
		    med_mode_acces access)
{
  //const char * LOC = "MESH::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Mesh Name\",MED_EN::med_mode_acces access) : ";

  GENDRIVER * driver;

  BEGIN_OF("MESH::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Mesh Name\",MED_EN::med_mode_acces access) : ");

  SCRUTE(driverType);

  driver = DRIVERFACTORY::buildDriverForMesh(driverType,fileName,this,
					     driverName,access) ;

  _drivers.push_back(driver);

  int current = _drivers.size()-1;

  _drivers[current]->setMeshName(driverName);

  END_OF();

  return current;
}

/*! Add an existing MESH driver. */
int  MESH::addDriver(GENDRIVER & driver)
{
  //const char * LOC = "MESH::addDriver(GENDRIVER &) : ";
  BEGIN_OF("MESH::addDriver(GENDRIVER &) : ");

  // A faire : Vérifier que le driver est de type MESH.

  // For the case where driver does not know about me, i.e. has been created through
  // constructor witout parameters: create newDriver knowing me and get missing data
  // from driver using merge()
  //GENDRIVER * newDriver = driver.copy() ;
  GENDRIVER* newDriver = DRIVERFACTORY::buildDriverForMesh(driver.getDriverType(),
                                                           driver.getFileName(), this,
                                                           driver.getMeshName(),
                                                           driver.getAccessMode());
  _drivers.push_back(newDriver);

  int current = _drivers.size()-1;
  driver.setId(current);

  newDriver->merge( driver );
  newDriver->setId( current );

  return current;

  END_OF();
}

/*! Remove an existing MESH driver. */
void MESH::rmDriver (int index/*=0*/) {
  const char * LOC = "MESH::rmDriver (int index=0): ";
  BEGIN_OF(LOC);

  if (index >= 0 && index < _drivers.size() && _drivers[index]) {
    delete _drivers[index];
    _drivers[index] = 0;
    MESSAGE ("detruire");
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and  |"
                                     << _drivers.size()
                                     )
                          );

  END_OF();

};

// ------ End of Drivers Management Part


void MESH::init()
{
  //const char * LOC = "MESH::init(): ";

  BEGIN_OF("MESH::init(): ");

  _name = "NOT DEFINED"; // A POSITIONNER EN FCT DES IOS ?

  _coordinate   = (COORDINATE   *) NULL;
  _connectivity = (CONNECTIVITY *) NULL;

  _spaceDimension        =          MED_INVALID; // 0 ?!?
  _meshDimension         =          MED_INVALID;
  _numberOfNodes         =          MED_INVALID;

  _isAGrid = false;

  _arePresentOptionnalNodesNumbers = 0;

  END_OF();
};

/*! Create an empty MESH. */
MESH::MESH():_coordinate(NULL),_connectivity(NULL), _isAGrid(false) {
  init();
};

MESH::MESH(MESH &m)
{
  _name=m._name;
  _isAGrid = m._isAGrid;

  if (m._coordinate != NULL)
    _coordinate = new COORDINATE(* m._coordinate);
  else
    _coordinate = (COORDINATE *) NULL;

  if (m._connectivity != NULL)
    _connectivity = new CONNECTIVITY(* m._connectivity);
  else
    _connectivity = (CONNECTIVITY *) NULL;

  _spaceDimension = m._spaceDimension;
  _meshDimension = m._meshDimension;
  _numberOfNodes = m._numberOfNodes;

  _familyNode = m._familyNode;
  for (int i=0; i<(int)m._familyNode.size(); i++)
    {
      _familyNode[i] = new FAMILY(* m._familyNode[i]);
      _familyNode[i]->setMeshDirectly(this);
    }

  _familyCell = m._familyCell;
  for (int i=0; i<(int)m._familyCell.size(); i++)
    {
      _familyCell[i] = new FAMILY(* m._familyCell[i]);
      _familyCell[i]->setMeshDirectly(this);
    }

  _familyFace = m._familyFace;
  for (int i=0; i<(int)m._familyFace.size(); i++)
    {
      _familyFace[i] = new FAMILY(* m._familyFace[i]);
      _familyFace[i]->setMeshDirectly(this);
    }

  _familyEdge = m._familyEdge;
  for (int i=0; i<(int)m._familyEdge.size(); i++)
    {
      _familyEdge[i] = new FAMILY(* m._familyEdge[i]);
      _familyEdge[i]->setMeshDirectly(this);
    }

  _groupNode = m._groupNode;
  for (int i=0; i<(int)m._groupNode.size(); i++)
    {
      _groupNode[i] = new GROUP(* m._groupNode[i]);
      _groupNode[i]->setMeshDirectly(this);
    }

  _groupCell = m._groupCell;
  for (int i=0; i<(int)m._groupCell.size(); i++)
    {
      _groupCell[i] = new GROUP(* m._groupCell[i]);
      _groupCell[i]->setMeshDirectly(this);
    }

  _groupFace = m._groupFace;
  for (int i=0; i<(int)m._groupFace.size(); i++)
    {
      _groupFace[i] = new GROUP(* m._groupFace[i]);
      _groupFace[i]->setMeshDirectly(this);
    }

  _groupEdge = m._groupEdge;
  for (int i=0; i<(int)m._groupEdge.size(); i++)
    {
      _groupEdge[i] = new GROUP(* m._groupEdge[i]);
      _groupEdge[i]->setMeshDirectly(this);
    }

  //_drivers = m._drivers;  //Recopie des drivers?
}

MESH::~MESH() {

  MESSAGE("MESH::~MESH() : Destroying the Mesh");
  if (_coordinate != ((COORDINATE *) NULL)) delete _coordinate ;
  if (_connectivity != ((CONNECTIVITY *) NULL)) delete _connectivity ;
  int size ;
  size = _familyNode.size() ;
  for (int i=0;i<size;i++)
    delete _familyNode[i] ;
  size = _familyCell.size() ;
  for (int i=0;i<size;i++)
    delete _familyCell[i] ;
  size = _familyFace.size() ;
  for (int i=0;i<size;i++)
    delete _familyFace[i] ;
  size = _familyEdge.size() ;
  for (int i=0;i<size;i++)
    delete _familyEdge[i] ;
  size = _groupNode.size() ;
  for (int i=0;i<size;i++)
    delete _groupNode[i] ;
  size = _groupCell.size() ;
  for (int i=0;i<size;i++)
    delete _groupCell[i] ;
  size = _groupFace.size() ;
  for (int i=0;i<size;i++)
    delete _groupFace[i] ;
  size = _groupEdge.size() ;
  for (int i=0;i<size;i++)
    delete _groupEdge[i] ;

  MESSAGE("In this object MESH there is(are) " << _drivers.size() << " driver(s)");

  for (unsigned int index=0; index < _drivers.size(); index++ )
    {
      SCRUTE(_drivers[index]);
      if ( _drivers[index] != NULL) delete _drivers[index];
    }

}

/*
  Method equivalent to getNumberOfTypes except that it includes not only classical Types but polygons/polyhedra also.
 */
int MESH::getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const
{
  if(_connectivity!= NULL)
    return _connectivity->getNumberOfTypesWithPoly(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypesWithPoly( medEntityMesh ) : Connectivity not defined !"));
}

/*
  Method equivalent to getTypesWithPoly except that it includes not only classical Types but polygons/polyhedra also.
  WARNING the returned array MUST be deallocated.
 */
MED_EN::medGeometryElement * MESH::getTypesWithPoly(MED_EN::medEntityMesh Entity) const
{
  if (Entity == MED_EN::MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : No medGeometryElement with MED_NODE entity !"));
  if (_connectivity != NULL)
    return _connectivity->getGeometricTypesWithPoly(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*
  Method equivalent to getNumberOfElementsWithPoly except that it includes not only classical Types but polygons/polyhedra also.
 */
int MESH::getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const
{
  if(Type==MED_POLYGON || Type==MED_POLYHEDRA)
    {
      int nbOfPolygs=_connectivity->getNumberOfElementOfPolyType(Entity);
      return nbOfPolygs;
    }
  else if(Type==MED_ALL_ELEMENTS)
    {
      int nbOfClassicalTypes=getNumberOfElements(Entity,MED_ALL_ELEMENTS);
      int nbOfClassicalTypes2=_connectivity->getNumberOfElementOfPolyType(Entity);
      return nbOfClassicalTypes+nbOfClassicalTypes2;
    }
  else
    return getNumberOfElements(Entity,Type);
}

bool MESH::existConnectivityWithPoly(MED_EN::medConnectivity ConnectivityType,
                                     MED_EN::medEntityMesh Entity) const
{
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivityWithPoly(ConnectivityType,Entity);
}

MESH & MESH::operator=(const MESH &m)
{
  //const char * LOC = "MESH & MESH::operator=(const MESH &m) : ";
  BEGIN_OF("MESH & MESH::operator=(const MESH &m) : ");

  MESSAGE(__LOC <<"Not yet implemented, operating on the object " << m);
  //  A FAIRE.........

  // ATTENTION CET OPERATEUR DE RECOPIE EST DANGEREUX POUR LES
  // POINTEURS : ex : nodal_connectivity ???? EXPRES ????
//    _drivers = m._drivers;

//    space_dimension=m.space_dimension;
//    mesh_dimension=m.mesh_dimension;

//    nodes_count=m.nodes_count;
//    coordinates=m.coordinates;
//    coordinates_name=m.coordinates_name ;
//    coordinates_unit=m.coordinates_unit ;
//    nodes_numbers=m.nodes_numbers ;

//    cells_types_count=m.cells_types_count;
//    cells_type=m.cells_type;
//    cells_count=m.cells_count;
//    nodal_connectivity=m.nodal_connectivity;

//    nodes_families_count=m.nodes_families_count;
//    nodes_Families=m.nodes_Families;

//    cells_families_count=m.cells_families_count;
//    cells_Families=m.cells_Families;

//    maximum_cell_number_by_node = m.maximum_cell_number_by_node;
//    if (maximum_cell_number_by_node > 0)
//      {
//        reverse_nodal_connectivity = m.reverse_nodal_connectivity;
//        reverse_nodal_connectivity_index = m.reverse_nodal_connectivity_index ;
//      }
  END_OF();

  return *this;
}

bool MESH::operator==(const MESH& other) const
{
  BEGIN_OF("MESH::operator==");
  return this==&other;
}

/*! Create a %MESH object using a %MESH driver of type %driverTypes (MED_DRIVER, ....) associated with file fileName.
  The meshname driverName must already exists in the file.*/
MESH::MESH(driverTypes driverType, const string &  fileName/*=""*/, const string &  driverName/*=""*/) throw (MEDEXCEPTION)
{
  //const char * LOC ="MESH::MESH(driverTypes driverType, const string &  fileName="", const string &  driverName="") : ";
  int current;

  BEGIN_OF("MESH::MESH(driverTypes driverType, const string &  fileName="", const string &  driverName="") : ");

  init();
  GENDRIVER *myDriver=DRIVERFACTORY::buildDriverForMesh(driverType,fileName,this,driverName,RDONLY);
  current = addDriver(*myDriver);
  delete myDriver;
  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF();
};

/*
  for a deep comparison of 2 meshes.
*/
bool MESH::deepCompare(const MESH& other) const
{
  int size1=getSpaceDimension()*getNumberOfNodes();
  int size2=other.getSpaceDimension()*other.getNumberOfNodes();
  if(size1!=size2)
    return false;

  const COORDINATE* CRD = other.getCoordinateptr();
  if( (!CRD && _coordinate) || (CRD && !(_coordinate)) ) {
    return false;
  }

  bool ret=true;
  if( _coordinate ) {
    const double* coord1=getCoordinates(MED_FULL_INTERLACE);
    const double* coord2=other.getCoordinates(MED_FULL_INTERLACE);
    for(int i=0;i<size1 && ret;i++) {
      ret=(fabs(coord1[i]-coord2[i])<1e-15);
    }
  }
  if(ret) {
    const CONNECTIVITY* CNV = other.getConnectivityptr();
    if( (!CNV && _connectivity) || (CNV && !(_connectivity)) ) {
      return false;
    }
    if(_connectivity) {
      return _connectivity->deepCompare(*other._connectivity);
    }
  }
  return ret;
}

/*!
 * \brief print my contents
 */
ostream & ::MEDMEM::operator<<(ostream &os, const MESH &myMesh)
{
  myMesh.printMySelf(os);
  return os;
}
void MESH::printMySelf(ostream &os) const
{
  const MESH &myMesh = *this;
  int spacedimension = myMesh.getSpaceDimension();
  int meshdimension  = myMesh.getMeshDimension();
  int numberofnodes  = myMesh.getNumberOfNodes();

  if ( spacedimension == MED_INVALID ) {
    os << " Empty mesh ...";
    return;
  }

  os << "Space Dimension : " << spacedimension << endl << endl;

  os << "Mesh Dimension : " << meshdimension << endl << endl;
  
  if(myMesh.getCoordinateptr()) {
    const double * coordinates = myMesh.getCoordinates(MED_FULL_INTERLACE);
    
    os << "SHOW NODES COORDINATES : " << endl;
    os << "Name :" << endl;
    const string * coordinatesnames = myMesh.getCoordinatesNames();
    if ( coordinatesnames ) {
      for(int i=0; i<spacedimension ; i++)
      {
        os << " - " << coordinatesnames[i] << endl;
      }
    }
    os << "Unit :" << endl;
    const string * coordinatesunits = myMesh.getCoordinatesUnits();
    if ( coordinatesunits ) {
      for(int i=0; i<spacedimension ; i++)
      {
        os << " - " << coordinatesunits[i] << endl;
      }
    }
    for(int i=0; i<numberofnodes ; i++)
    {
      os << "Nodes " << i+1 << " : ";
      for (int j=0; j<spacedimension ; j++)
	os << coordinates[i*spacedimension+j] << " ";
      os << endl;
    }
  }
  
  if(myMesh.getConnectivityptr()) {
    os << endl << "SHOW CONNECTIVITY  :" << endl;
    os << *myMesh._connectivity << endl;
  }
  
  medEntityMesh entity;
  os << endl << "SHOW FAMILIES :" << endl << endl;
  for (int k=1; k<=4; k++)
    {
      if (k==1) entity = MED_NODE;
      if (k==2) entity = MED_CELL;
      if (k==3) entity = MED_FACE;
      if (k==4) entity = MED_EDGE;
      int numberoffamilies = myMesh.getNumberOfFamilies(entity);
      os << "NumberOfFamilies on "<< entNames[entity] <<" : "<<numberoffamilies<<endl;
      using namespace MED_EN;
      for (int i=1; i<numberoffamilies+1;i++)
	{
	  os << * myMesh.getFamily(entity,i) << endl;
	}
    }

  os << endl << "SHOW GROUPS :" << endl << endl;
  for (int k=1; k<=4; k++)
    {
      if (k==1) entity = MED_NODE;
      if (k==2) entity = MED_CELL;
      if (k==3) entity = MED_FACE;
      if (k==4) entity = MED_EDGE;
      int numberofgroups = myMesh.getNumberOfGroups(entity);
      os << "NumberOfGroups on "<< entNames[entity] <<" : "<<numberofgroups<<endl;
      using namespace MED_EN;
      for (int i=1; i<numberofgroups+1;i++)
	{
	  os << * myMesh.getGroup(entity,i) << endl;
	}
    }
}

/*!
  Get global number of element which have same connectivity than connectivity argument.

  It do not take care of connectivity order (3,4,7,10 is same as 7,3,10,4).

  Return -1 if not found.
*/
int MESH::getElementNumber(medConnectivity ConnectivityType, medEntityMesh Entity, medGeometryElement Type, int * connectivity) const
{
  const char* LOC="MESH::getElementNumber " ;
  BEGIN_OF(LOC) ;

  int numberOfValue ; // size of connectivity array
  CELLMODEL myType(Type) ;
  if (ConnectivityType==MED_DESCENDING)
    numberOfValue = myType.getNumberOfConstituents(1) ; // faces (3D) or edges (2D)
  else
    numberOfValue = myType.getNumberOfNodes() ; // nodes

  const int * myReverseConnectivityValue = getReverseConnectivity(ConnectivityType,Entity) ;
  const int * myReverseConnectivityIndex = getReverseConnectivityIndex(ConnectivityType,Entity) ;

  // First node or face/edge
  int indexBegin = myReverseConnectivityIndex[connectivity[0]-1] ;
  int indexEnd = myReverseConnectivityIndex[connectivity[0]] ;

  // list to put cells numbers
  list<int> cellsList ;
  list<int>::iterator itList ;
  for (int i=indexBegin; i<indexEnd; i++)
    cellsList.push_back(myReverseConnectivityValue[i-1]) ;

  // Foreach node or face/edge in cell, we search cells which are in common.
  // At the end, we must have only one !

  for (int i=1; i<numberOfValue; i++) {
    int connectivity_i = connectivity[i] ;
    for (itList=cellsList.begin();itList!=cellsList.end();itList++) {
      bool find = false ;
      for (int j=myReverseConnectivityIndex[connectivity_i-1]; j<myReverseConnectivityIndex[connectivity_i]; j++) {
	if ((*itList)==myReverseConnectivityValue[j-1]) {
	  find=true ;
	  break ;
	}
      }
      if (!find) {
	itList=cellsList.erase(itList);
	itList--; // well : rigth if itList = cellsList.begin() ??
      }
    }
  }

  if (cellsList.size()>1) // we have more than one cell
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Found more than one element !")) ;

  if (cellsList.size()==0)
    return -1;

  END_OF();

  return cellsList.front() ;
}

/*!
  Return a support which reference all elements on the boundary of mesh.

  For instance, we could get only face in 3D and edge in 2D.
*/
SUPPORT * MESH::getBoundaryElements(medEntityMesh Entity)
  throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getBoundaryElements : " ;
  BEGIN_OF(LOC) ;
  // some test :
  // actually we could only get face (in 3D) and edge (in 2D)
  medEntityMesh entityToParse=Entity;
  if(_spaceDimension == 3)
    if (Entity != MED_FACE)
      if(Entity==MED_NODE)
	entityToParse=MED_FACE;
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<Entity<<" !"));
  if(_spaceDimension == 2)
    if(Entity != MED_EDGE)
      if(Entity==MED_NODE)
	entityToParse=MED_EDGE;
      else
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<Entity<<" !"));

  const int * myConnectivityValue = getReverseConnectivity(MED_DESCENDING) ;
  const int * myConnectivityIndex = getReverseConnectivityIndex(MED_DESCENDING) ;
  int numberOf = getNumberOfElementsWithPoly(entityToParse,MED_ALL_ELEMENTS) ;
  list<int> myElementsList;

  for (int i=0 ; i<numberOf; i++)
    if (myConnectivityValue[myConnectivityIndex[i]] == 0) {
      myElementsList.push_back(i+1);
    }

  if(Entity==MED_NODE)
    return buildSupportOnNodeFromElementList(myElementsList,entityToParse);
  else
    return buildSupportOnElementsFromElementList(myElementsList,entityToParse);
}

/*!
  Method that do the same thing as buildSupportOnNodeFromElementList except that a SUPPORT is not created.
 */
void MESH::fillSupportOnNodeFromElementList(const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION)
{
  MED_EN::medEntityMesh entity=supportToFill->getEntity();
  supportToFill->setAll(false);
  supportToFill->setMeshDirectly((MESH *)this);

  int i;
  set<int> nodes;
  if ( entity == MED_NODE ) {
    supportToFill->fillFromNodeList(listOfElt);
  }
  else {
    for(list<int>::const_iterator iter=listOfElt.begin();iter!=listOfElt.end();iter++)
    {
      int lgth;
      const int *conn=_connectivity->getConnectivityOfAnElementWithPoly(MED_NODAL,entity,*iter,lgth);
      for(i=0;i<lgth;i++)
	nodes.insert(conn[i]);
    }
    list<int> nodesList;
    for(set<int>::iterator iter2=nodes.begin();iter2!=nodes.end();iter2++)
      nodesList.push_back(*iter2);
    supportToFill->fillFromNodeList(nodesList);
  }
}

/*!
  Method created to factorize code. This method creates a new support on NODE (to deallocate) containing all the nodes id contained in elements 'listOfElt' of
  entity 'entity'.
 */
SUPPORT *MESH::buildSupportOnNodeFromElementList(const list<int>& listOfElt,MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION)
{
  SUPPORT * mySupport = new SUPPORT((MESH *)this,"Boundary",entity);
  fillSupportOnNodeFromElementList(listOfElt,mySupport);
  return mySupport;
}

/*!
  Method created to factorize code. This method creates a new support on entity 'entity' (to deallocate) containing all the entities contained in
  elements 'listOfElt' of entity 'entity'.
 */
SUPPORT *MESH::buildSupportOnElementsFromElementList(const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION)
{
  //const char * LOC = "MESH::buildSupportOnElementsFromElementList : " ;
  BEGIN_OF("MESH::buildSupportOnElementsFromElementList : ");
  SUPPORT *mySupport=new SUPPORT((MESH *)this,"Boundary",entity);
  mySupport->fillFromElementList(listOfElt);
  END_OF();
  return mySupport ;
}

FIELD<double, FullInterlace>* MESH::getVolume(const SUPPORT *Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getVolume(SUPPORT*) : ";
  BEGIN_OF(LOC);

  // Support must be on 3D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  MESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;
  nb_type = Support->getNumberOfTypes();
  length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();
  int index;
  FIELD<double, FullInterlace>* Volume =
    new FIELD<double, FullInterlace>(Support,1);
  //  double *volume = new double [length_values];
  Volume->setName("VOLUME");
  Volume->setDescription("cells volume");
  Volume->setComponentName(1,"volume");
  Volume->setComponentDescription(1,"desc-comp");

  /*  string MEDComponentUnit(MED_TAILLE_PNOM,' ');*/

  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1]+"*"+myMesh->getCoordinatesUnits()[2];

  Volume->setMEDComponentUnit(1,MEDComponentUnit);

  Volume->setIterationNumber(0);
  Volume->setOrderNumber(0);
  Volume->setTime(0.0);

  //const double *volume = Volume->getValue(MED_FULL_INTERLACE);
  typedef  MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array ArrayNoGauss;
  ArrayNoGauss  *volume = Volume->getArrayNoGauss();

  index = 1;
  const double * coord = getCoordinates(MED_FULL_INTERLACE);

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      double xvolume;
      nb_entity_type = Support->getNumberOfElements(type);
      if(type != MED_EN::MED_POLYHEDRA)
	{
	  if (onAll)
	    {
	      global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	    }
	  else
	    {
	      const int * supp_number = Support->getNumber(type);
	      const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
	      const int * connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
	      int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

	      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
		for (int j_ent = 0; j_ent<(type%100); j_ent++) {
		  global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
		}
	      }
	      global_connectivity = global_connectivity_tmp ;
	    }
	}

      switch (type)
	{
	case MED_TETRA4 : case MED_TETRA10 :
	  {
	    for (int tetra=0;tetra<nb_entity_type;tetra++)
	      {
		int tetra_index = (type%100)*tetra;
		int N1 = global_connectivity[tetra_index]-1;
		int N2 = global_connectivity[tetra_index+1]-1;
		int N3 = global_connectivity[tetra_index+2]-1;
		int N4 = global_connectivity[tetra_index+3]-1;
		xvolume=CalculateVolumeForTetra(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4);
		volume->setIJ(index,1,xvolume) ;
		index++;
	      }
	    break;
	  }
	case MED_PYRA5 : case MED_PYRA13 :
	  {
	    for (int pyra=0;pyra<nb_entity_type;pyra++)
	      {
		int pyra_index = (type%100)*pyra;
		int N1 = global_connectivity[pyra_index]-1;
		int N2 = global_connectivity[pyra_index+1]-1;
		int N3 = global_connectivity[pyra_index+2]-1;
		int N4 = global_connectivity[pyra_index+3]-1;
		int N5 = global_connectivity[pyra_index+4]-1;
		xvolume=CalculateVolumeForPyra(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5);
		volume->setIJ(index,1,xvolume) ;
		index++;
	      }
	    break;
	  }
	case MED_PENTA6 : case MED_PENTA15 :
	  {
	    for (int penta=0;penta<nb_entity_type;penta++)
	      {
		int penta_index = (type%100)*penta;
		int N1 = global_connectivity[penta_index]-1;
		int N2 = global_connectivity[penta_index+1]-1;
		int N3 = global_connectivity[penta_index+2]-1;
		int N4 = global_connectivity[penta_index+3]-1;
		int N5 = global_connectivity[penta_index+4]-1;
		int N6 = global_connectivity[penta_index+5]-1;
		xvolume=CalculateVolumeForPenta(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5,coord+dim_space*N6);
		volume->setIJ(index,1,xvolume) ;
		index++;
	      }
	    break;
	  }
	case MED_HEXA8 : case MED_HEXA20 :
	  {
	    for (int hexa=0;hexa<nb_entity_type;hexa++)
	      {
		int hexa_index = (type%100)*hexa;

		int N1 = global_connectivity[hexa_index]-1;
		int N2 = global_connectivity[hexa_index+1]-1;
		int N3 = global_connectivity[hexa_index+2]-1;
		int N4 = global_connectivity[hexa_index+3]-1;
		int N5 = global_connectivity[hexa_index+4]-1;
		int N6 = global_connectivity[hexa_index+5]-1;
		int N7 = global_connectivity[hexa_index+6]-1;
		int N8 = global_connectivity[hexa_index+7]-1;
		xvolume=CalculateVolumeForHexa(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5,coord+dim_space*N6,coord+dim_space*N7,coord+dim_space*N8);
		volume->setIJ(index,1,xvolume) ;
		index++;
	      }
	    break;
	  }
	case MED_POLYHEDRA:
	  {
	    double bary[3];
	    if(onAll)
	      {
		for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
		  {
		    int lgthNodes,iPts,iFaces,iPtsInFace;
		    int offsetWithClassicType=getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
		    int *nodes=_connectivity->getNodesOfPolyhedron(offsetWithClassicType+polyhs+1,lgthNodes);
		    int nbOfFaces,*nbOfNodesPerFaces;
		    int **nodes1=_connectivity->getNodesPerFaceOfPolyhedron(offsetWithClassicType+polyhs+1,nbOfFaces,nbOfNodesPerFaces);
		    double **pts=new double * [lgthNodes];
		    double ***pts1=new double ** [nbOfFaces];
		    for(iPts=0;iPts<lgthNodes;iPts++)
		      pts[iPts]=(double *)(coord+3*(nodes[iPts]-1));
		    for(iFaces=0;iFaces<nbOfFaces;iFaces++)
		      {
			pts1[iFaces]=new double* [nbOfNodesPerFaces[iFaces]];
			for(iPtsInFace=0;iPtsInFace<nbOfNodesPerFaces[iFaces];iPtsInFace++)
			  pts1[iFaces][iPtsInFace]=(double *)(coord+3*(nodes1[iFaces][iPtsInFace]-1));
		      }
		    delete [] nodes1;
		    CalculateBarycenterDyn((const double **)pts,lgthNodes,3,bary);
		    delete [] nodes;
		    delete [] pts;
		    xvolume=CalculateVolumeForPolyh((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
		    delete [] nbOfNodesPerFaces;
		    for(iFaces=0;iFaces<nbOfFaces;iFaces++)
			delete [] pts1[iFaces];
		    delete [] pts1;
		    volume->setIJ(index,1,xvolume) ;
		    index++;
		  }
	      }
	    else
	      {
		const int * supp_number = Support->getNumber(MED_EN::MED_POLYHEDRA);
		for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
		  {
		    int lgthNodes,iPts,iFaces,iPtsInFace;
		    int *nodes=_connectivity->getNodesOfPolyhedron(supp_number[polyhs],lgthNodes);
		    int nbOfFaces,*nbOfNodesPerFaces;
		    int **nodes1=_connectivity->getNodesPerFaceOfPolyhedron(supp_number[polyhs],nbOfFaces,nbOfNodesPerFaces);
		    double **pts=new double * [lgthNodes];
		    double ***pts1=new double ** [nbOfFaces];
		    for(iPts=0;iPts<lgthNodes;iPts++)
		      pts[iPts]=(double *)(coord+3*(nodes[iPts]-1));
		    for(iFaces=0;iFaces<nbOfFaces;iFaces++)
		      {
			pts1[iFaces]=new double* [nbOfNodesPerFaces[iFaces]];
			for(iPtsInFace=0;iPtsInFace<nbOfNodesPerFaces[iFaces];iPtsInFace++)
			  pts1[iFaces][iPtsInFace]=(double *)(coord+3*(nodes1[iFaces][iPtsInFace]-1));
		      }
		    delete [] nodes1;
		    CalculateBarycenterDyn((const double **)pts,lgthNodes,3,bary);
		    delete [] nodes;
		    delete [] pts;
		    xvolume=CalculateVolumeForPolyh((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
		    delete [] nbOfNodesPerFaces;
		    for(iFaces=0;iFaces<nbOfFaces;iFaces++)
			delete [] pts1[iFaces];
		    delete [] pts1;
		    volume->setIJ(index,1,xvolume) ;
		    index++;
		  }
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Volumes on it !"));
	  break;
	}

      if (!onAll && type!=MED_EN::MED_POLYHEDRA)
	delete [] global_connectivity ;
    }

  return Volume;
}

FIELD<double, FullInterlace>* MESH::getArea(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getArea(SUPPORT*) : ";
  BEGIN_OF(LOC);

  // Support must be on 2D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  MESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;

  nb_type = Support->getNumberOfTypes();
  length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;
  FIELD<double, FullInterlace>* Area;

  Area = new FIELD<double, FullInterlace>(Support,1);
  Area->setName("AREA");
  Area->setDescription("cells or faces area");

  Area->setComponentName(1,"area");
  Area->setComponentDescription(1,"desc-comp");

  /*  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1];*/

  string MEDComponentUnit="trucmuch";

  Area->setMEDComponentUnit(1,MEDComponentUnit);

  Area->setIterationNumber(0);
  Area->setOrderNumber(0);
  Area->setTime(0.0);

  double *area = (double *)Area->getValue();

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      nb_entity_type = Support->getNumberOfElements(type);
      const int *global_connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
      if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
	{
	  if (onAll)
	    {
	      global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	    }
	  else
	    {
	      const int * supp_number = Support->getNumber(type);
	      const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
	      int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

	      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
		for (int j_ent = 0; j_ent<(type%100); j_ent++) {
		  global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[global_connectivityIndex[supp_number[k_type]-1]+j_ent-1];
		}
	      }
	      global_connectivity = global_connectivity_tmp ;
	    }
	}
      switch (type)
	{
	case MED_TRIA3 : case MED_TRIA6 :
	  {
	    for (int tria=0;tria<nb_entity_type;tria++)
	      {
		int tria_index = (type%100)*tria;

		int N1 = global_connectivity[tria_index]-1;
		int N2 = global_connectivity[tria_index+1]-1;
		int N3 = global_connectivity[tria_index+2]-1;

		area[index]=CalculateAreaForTria(coord+(dim_space*N1),
						   coord+(dim_space*N2),
						   coord+(dim_space*N3),dim_space);
		index++;
	      }
	    break;
	  }
	case MED_QUAD4 : case MED_QUAD8 :
	  {
	    for (int quad=0;quad<nb_entity_type;quad++)
	      {
		int quad_index = (type%100)*quad;

		int N1 = global_connectivity[quad_index]-1;
		int N2 = global_connectivity[quad_index+1]-1;
		int N3 = global_connectivity[quad_index+2]-1;
		int N4 = global_connectivity[quad_index+3]-1;

		area[index]=CalculateAreaForQuad(coord+dim_space*N1,
						   coord+dim_space*N2,
						   coord+dim_space*N3,
						   coord+dim_space*N4,dim_space);
		index++;
	      }
	    break;
	  }
	case MED_POLYGON :
	  {
	    if(onAll)
	      {
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int size=connectivity_index[polygs+1]-connectivity_index[polygs];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1]-1));
		    area[index] = CalculateAreaForPolyg((const double **)pts,size,dim_space);
		    delete [] pts;
		    index++;
		  }
	      }
	    else
	      {
		const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		int offsetWithClassicType=getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int size=connectivity_index[supp_number[polygs]-offsetWithClassicType]-connectivity_index[supp_number[polygs]-offsetWithClassicType-1];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[supp_number[polygs]-offsetWithClassicType-1]+iPts-1]-1));
		    area[index]=CalculateAreaForPolyg((const double **)pts,size,dim_space);
		    delete [] pts;
		    index++;
		  }
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Areas on it !"));
	  break;
	}

      if (!onAll)
	if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
	  delete [] global_connectivity ;
    }
  return Area;
}

FIELD<double, FullInterlace>* MESH::getLength(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getLength(SUPPORT*) : ";
  BEGIN_OF(LOC);

  // Support must be on 1D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  MESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;

  nb_type = Support->getNumberOfTypes();
  length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;
  FIELD<double, FullInterlace>* Length;

  Length = new FIELD<double, FullInterlace>(Support,1);
  //  double *length = new double [length_values];

  //const double *length = Length->getValue(MED_FULL_INTERLACE);
  typedef  MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array ArrayNoGauss;
  ArrayNoGauss * length = Length->getArrayNoGauss();

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 1;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      double xlength;

      if (onAll)
	{
	  nb_entity_type = getNumberOfElements(support_entity,type);
	  global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	}
      else
	{
 	  nb_entity_type = Support->getNumberOfElements(type);
 	  const int * supp_number = Support->getNumber(type);
 	  const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
 	  const int * connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
	  int* global_connectivity_tmp = new int[(type%100)*nb_entity_type];

 	  for (int k_type = 0; k_type<nb_entity_type; k_type++) {
 	    for (int j_ent = 0; j_ent<(type%100); j_ent++) {
 	      global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
 	    }
 	  }
	  global_connectivity = global_connectivity_tmp ;
	}

      switch (type)
	{
	case MED_SEG2 : case MED_SEG3 :
	  {
	    for (int edge=0;edge<nb_entity_type;edge++)
	      {
		int edge_index = (type%100)*edge;

		int N1 = global_connectivity[edge_index]-1;
		int N2 = global_connectivity[edge_index+1]-1;

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];

		double z1, z2 ; z1 = 0.0 ; z2 = 0.0 ; if (dim_space==3) {
		  z1 = coord[dim_space*N1+2]; z2 = coord[dim_space*N2+2];}

		xlength =  sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) +
				(z1 - z2)*(z1 - z2));

		length->setIJ(index,1,xlength) ;
		index++;
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Lengths on it !"));
	  break;
	}

      if (!onAll) delete [] global_connectivity ;
    }

  return Length;
}

FIELD<double, FullInterlace>* MESH::getNormal(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getNormal(SUPPORT*) : ";
  BEGIN_OF(LOC);

  // Support must be on 2D or 1D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  MESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh : pointeur problem !"));

  int dim_space = getSpaceDimension();
  int mesh_dim=getMeshDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  if( support_entity!=MED_EDGE && (mesh_dim!=1 || support_entity!=MED_CELL) && ( mesh_dim!=2 || support_entity!=MED_CELL ) && ( mesh_dim!=3 || support_entity!=MED_FACE ))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"incompatible mesh dimension and entity"));
  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;

  nb_type = Support->getNumberOfTypes();
  length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;

  FIELD<double, FullInterlace>* Normal =
    new FIELD<double, FullInterlace>(Support,dim_space);
  Normal->setName("NORMAL");
  Normal->setDescription("cells or faces normal");
  for (int k=1;k<=dim_space;k++) {
    Normal->setComponentName(k,"normal");
    Normal->setComponentDescription(k,"desc-comp");
    Normal->setMEDComponentUnit(k,"unit");
  }

  Normal->setIterationNumber(MED_NOPDT);
  Normal->setOrderNumber(MED_NONOR);
  Normal->setTime(0.0);
  double *normal = (double *)Normal->getValue();

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      nb_entity_type = Support->getNumberOfElements(type);

      // Make sure we trying to get Normals on
      // TRIA3 or TRIA6 or QUAD4 QUAD8 (2D) cells on a 3D mesh
      // or on SEG2 or SEG3 (1D) cells on a 2D mesh

      if ( (((type==MED_TRIA3) || (type==MED_TRIA6) ||
	     (type==MED_QUAD4) || (type==MED_QUAD8) || (type==MED_POLYGON)) &&
      	    (dim_space != 3)) || (((type==MED_SEG2) || (type==MED_SEG3)) &&
				  (dim_space != 2)) )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh : dimension problem !"));
      if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
	{
	  if (onAll)
	    {
	      global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	    }
	  else
	    {
	      const int * supp_number = Support->getNumber(type);
	      const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
	      const int * connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
	      int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

	      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
		for (int j_ent = 0; j_ent<(type%100); j_ent++) {
		  global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
		}
	      }

	      global_connectivity = global_connectivity_tmp ;
	    }
	}

      switch (type)
	{
	case MED_TRIA3 : case MED_TRIA6 :
	  {
	    for (int tria=0;tria<nb_entity_type;tria++)
	      {
		int tria_index = (type%100)*tria;
		int N1 = global_connectivity[tria_index]-1;
		int N2 = global_connectivity[tria_index+1]-1;
		int N3 = global_connectivity[tria_index+2]-1;
		CalculateNormalForTria(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,normal+3*index);
		index++;
	      }
	    break;
	  }
	case MED_QUAD4 : case MED_QUAD8 :
	  {
	    for (int quad=0;quad<nb_entity_type;quad++)
	      {
		int quad_index = (type%100)*quad;
		int N1 = global_connectivity[quad_index]-1;
		int N2 = global_connectivity[quad_index+1]-1;
		int N3 = global_connectivity[quad_index+2]-1;
		int N4 = global_connectivity[quad_index+3]-1;
		CalculateNormalForQuad(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,normal+3*index);
		index++;
	      }
	    break;
	  }
	case MED_SEG2 : case MED_SEG3 :
	  {
	    double xnormal1, xnormal2;
	    for (int edge=0;edge<nb_entity_type;edge++)
	      {
		int edge_index = (type%100)*edge;

		int N1 = global_connectivity[edge_index]-1;
		int N2 = global_connectivity[edge_index+1]-1;

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];

		xnormal1 = -(y2-y1);
		xnormal2 = x2-x1;

		normal[2*index] = xnormal1 ;
		normal[2*index+1] = xnormal2 ;

		index++;
	      }
	    break;
	  }
	case MED_POLYGON :
	  {
	    if(onAll)
	      {
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int size=connectivity_index[polygs+1]-connectivity_index[polygs];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1])-1);
		    CalculateNormalForPolyg((const double **)pts,size,normal+3*index);
		    delete [] pts;
		    index++;
		  }
	      }
	    else
	      {
		const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		int offsetWithClassicType=getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int localPolygsNbP1=supp_number[polygs]-offsetWithClassicType;
		    int size=connectivity_index[localPolygsNbP1]-connectivity_index[localPolygsNbP1-1];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[localPolygsNbP1-1]+iPts-1])-1);
		    CalculateNormalForPolyg((const double **)pts,size,normal+3*index);
		    delete [] pts;
		    index++;
		  }
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Normals on it !"));
	  break;
	}
      if (!onAll && type!=MED_EN::MED_POLYGON)
	delete [] global_connectivity ;
    }
  END_OF();

  return Normal;
}

FIELD<double, FullInterlace>* MESH::getBarycenter(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getBarycenter(SUPPORT*) : ";
  MESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  const int* global_connectivity;

  nb_type = Support->getNumberOfTypes();
  length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  FIELD<double, FullInterlace>* Barycenter;
  Barycenter = new FIELD<double, FullInterlace>(Support,dim_space);
  Barycenter->setName("BARYCENTER");
  Barycenter->setDescription("cells or faces barycenter");

  for (int k=0;k<dim_space;k++) {
    int kp1 = k + 1;
    Barycenter->setComponentName(kp1,myMesh->getCoordinatesNames()[k]);
    Barycenter->setComponentDescription(kp1,"desc-comp");
    Barycenter->setMEDComponentUnit(kp1,myMesh->getCoordinatesUnits()[k]);
  }
  Barycenter->setIterationNumber(0);
  Barycenter->setOrderNumber(0);
  Barycenter->setTime(0.0);
  double *barycenter=(double *)Barycenter->getValue();
  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  int index=0;
  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      nb_entity_type = Support->getNumberOfElements(type);
      if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA )
	{
	  if (onAll)
	    {
	      global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	    }
	  else
	    {
	      const int * supp_number = Support->getNumber(type);
	      const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
	      int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

	      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
		for (int j_ent = 0; j_ent<(type%100); j_ent++) {
		  const int *global_connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
		  global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[global_connectivityIndex[supp_number[k_type]-1]+j_ent-1];
		}
	      }
	      global_connectivity = global_connectivity_tmp;
	    }
	}

      switch (type)
	{
	case MED_TETRA4 : case MED_TETRA10 :
	  {
	    for (int tetra =0;tetra<nb_entity_type;tetra++)
	      {
		int tetra_index = (type%100)*tetra;

		int N1 = global_connectivity[tetra_index]-1;
		int N2 = global_connectivity[tetra_index+1]-1;
		int N3 = global_connectivity[tetra_index+2]-1;
		int N4 = global_connectivity[tetra_index+3]-1;
		double *pts[4];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		pts[3]=(double *)coord+dim_space*N4;
		CalculateBarycenter<4,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_PYRA5 : case MED_PYRA13 :
	  {
	    for (int pyra=0;pyra<nb_entity_type;pyra++)
	      {
		int pyra_index = (type%100)*pyra;

		int N1 = global_connectivity[pyra_index]-1;
		int N2 = global_connectivity[pyra_index+1]-1;
		int N3 = global_connectivity[pyra_index+2]-1;
		int N4 = global_connectivity[pyra_index+3]-1;
		int N5 = global_connectivity[pyra_index+4]-1;
		double *pts[5];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		pts[3]=(double *)coord+dim_space*N4;
		pts[4]=(double *)coord+dim_space*N5;
		CalculateBarycenter<5,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_PENTA6 : case MED_PENTA15 :
	  {
	    for (int penta=0;penta<nb_entity_type;penta++)
	      {
		int penta_index = (type%100)*penta;

		int N1 = global_connectivity[penta_index]-1;
		int N2 = global_connectivity[penta_index+1]-1;
		int N3 = global_connectivity[penta_index+2]-1;
		int N4 = global_connectivity[penta_index+3]-1;
		int N5 = global_connectivity[penta_index+4]-1;
		int N6 = global_connectivity[penta_index+5]-1;
		double *pts[6];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		pts[3]=(double *)coord+dim_space*N4;
		pts[4]=(double *)coord+dim_space*N5;
		pts[5]=(double *)coord+dim_space*N6;
		CalculateBarycenter<6,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_HEXA8 : case MED_HEXA20 :
	  {
	    for (int hexa=0;hexa<nb_entity_type;hexa++)
	      {
		int hexa_index = (type%100)*hexa;

		int N1 = global_connectivity[hexa_index]-1;
		int N2 = global_connectivity[hexa_index+1]-1;
		int N3 = global_connectivity[hexa_index+2]-1;
		int N4 = global_connectivity[hexa_index+3]-1;
		int N5 = global_connectivity[hexa_index+4]-1;
		int N6 = global_connectivity[hexa_index+5]-1;
		int N7 = global_connectivity[hexa_index+6]-1;
		int N8 = global_connectivity[hexa_index+7]-1;
		double *pts[8];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		pts[3]=(double *)coord+dim_space*N4;
		pts[4]=(double *)coord+dim_space*N5;
		pts[5]=(double *)coord+dim_space*N6;
		pts[6]=(double *)coord+dim_space*N7;
		pts[7]=(double *)coord+dim_space*N8;
		CalculateBarycenter<8,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_TRIA3 : case MED_TRIA6 :
	  {
	    for (int tria=0;tria<nb_entity_type;tria++)
	      {
		int tria_index = (type%100)*tria;
		int N1 = global_connectivity[tria_index]-1;
		int N2 = global_connectivity[tria_index+1]-1;
		int N3 = global_connectivity[tria_index+2]-1;
		double *pts[3];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		if (dim_space==2)
		  CalculateBarycenter<3,2>((const double **)pts,barycenter+2*index);
		else
		  CalculateBarycenter<3,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_QUAD4 : case MED_QUAD8 :
	  {
	    for (int quad=0;quad<nb_entity_type;quad++)
	      {
		int quad_index = (type%100)*quad;
		int N1 = global_connectivity[quad_index]-1;
		int N2 = global_connectivity[quad_index+1]-1;
		int N3 = global_connectivity[quad_index+2]-1;
		int N4 = global_connectivity[quad_index+3]-1;
		double *pts[4];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		pts[2]=(double *)coord+dim_space*N3;
		pts[3]=(double *)coord+dim_space*N4;
		if (dim_space==2)
		  CalculateBarycenter<4,2>((const double **)pts,barycenter+2*index);
		else
		  CalculateBarycenter<4,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_SEG2 : case MED_SEG3 :
	  {
	    for (int edge=0;edge<nb_entity_type;edge++)
	      {
		int edge_index = (type%100)*edge;
		int N1 = global_connectivity[edge_index]-1;
		int N2 = global_connectivity[edge_index+1]-1;
		double *pts[2];
		pts[0]=(double *)coord+dim_space*N1;
		pts[1]=(double *)coord+dim_space*N2;
		if (dim_space==2)
		  CalculateBarycenter<2,2>((const double **)pts,barycenter+2*index);
		else
		  CalculateBarycenter<2,3>((const double **)pts,barycenter+3*index);
		index++;
	      }
	    break;
	  }
	case MED_POLYGON :
	  {
	    if(onAll)
	      {
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int size=connectivity_index[polygs+1]-connectivity_index[polygs];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1]-1);
		    CalculateBarycenterDyn((const double **)pts,size,dim_space,barycenter+dim_space*index);
		    delete [] pts;
		  }
	      }
	    else
	      {
		const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
		const int * connectivity = getPolygonsConnectivity(MED_EN::MED_NODAL,support_entity);
		const int * connectivity_index = getPolygonsConnectivityIndex(MED_EN::MED_NODAL,support_entity);
		int offsetWithClassicType=getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
		for (int polygs=0;polygs<nb_entity_type;polygs++)
		  {
		    int localPolygsNbP1=supp_number[polygs]-offsetWithClassicType;
		    int size=connectivity_index[localPolygsNbP1]-connectivity_index[localPolygsNbP1-1];
		    double **pts=new double * [size];
		    for(int iPts=0;iPts<size;iPts++)
		      pts[iPts]=(double *)coord+dim_space*(connectivity[connectivity_index[localPolygsNbP1-1]+iPts-1]-1);
		    CalculateBarycenterDyn((const double **)pts,size,dim_space,barycenter+dim_space*index);
		    delete [] pts;
		  }
	      }
	    index++;
	    break;
	  }
	case MED_EN::MED_POLYHEDRA:
	  {
	    if(onAll)
	      {
		for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
		  {
		    int lgthNodes;
		    int offsetWithClassicType=getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
		    int *nodes=_connectivity->getNodesOfPolyhedron(offsetWithClassicType+polyhs+1,lgthNodes);
		    double **pts=new double * [lgthNodes];
		    for(int iPts=0;iPts<lgthNodes;iPts++)
		      pts[iPts]=(double *)coord+3*(nodes[iPts]-1);
		    CalculateBarycenterDyn((const double **)pts,lgthNodes,3,barycenter+3*index);
		    delete [] pts;
		    delete [] nodes;
		    index++;
		  }
	      }
	    else
	      {
		const int * supp_number = Support->getNumber(MED_EN::MED_POLYHEDRA);
		for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
		  {
		    int lgthNodes;
		    int *nodes=_connectivity->getNodesOfPolyhedron(supp_number[polyhs],lgthNodes);
		    double **pts=new double * [lgthNodes];
		    for(int iPts=0;iPts<lgthNodes;iPts++)
		      pts[iPts]=(double *)coord+3*(nodes[iPts]-1);
		    CalculateBarycenterDyn((const double **)pts,lgthNodes,3,barycenter+3*index);
		    delete [] pts;
		    delete [] nodes;
		    index++;
		  }
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get a barycenter on it (in fact unknown type) !"));
	  break;
	}

      if (!onAll)
	if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
	  delete [] global_connectivity;
    }
  //END_OF();
  return Barycenter;
}

bool MESH::isEmpty() const
{
    bool notempty = _name != "NOT DEFINED"                || _coordinate != NULL           || _connectivity != NULL ||
	         _spaceDimension !=MED_INVALID || _meshDimension !=MED_INVALID  ||
		 _numberOfNodes !=MED_INVALID  || _groupNode.size() != 0   ||
		 _familyNode.size() != 0       || _groupCell.size() != 0   ||
		 _familyCell.size() != 0       || _groupFace.size() != 0   ||
		 _familyFace.size() != 0       || _groupEdge.size() != 0   ||
		 _familyEdge.size() != 0       || _isAGrid != 0 ;
    return !notempty;
}

void MESH::read(int index)
{
  const char * LOC = "MESH::read(int index=0) : ";
  BEGIN_OF(LOC);

  if (_drivers[index]) {
    _drivers[index]->open();
    _drivers[index]->read();
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     )
                          );
  END_OF();
}

/*! Write all the content of the MESH using driver referenced by the integer handler index.*/
void MESH::write(int index/*=0*/, const string & driverName/* = ""*/)
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
  END_OF();
}

//=======================================================================
//function : getSkin
//purpose  :
//=======================================================================

SUPPORT * MESH::getSkin(const SUPPORT * Support3D) throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getSkin : " ;
  BEGIN_OF(LOC) ;
  // some test :
  if (this != Support3D->getMesh())
    throw MEDEXCEPTION(STRING(LOC) <<  "no compatibility between *this and SUPPORT::_mesh !");
  if (_meshDimension != 3 || Support3D->getEntity() != MED_CELL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Defined on 3D cells only"));

  // well, all rigth !
  SUPPORT * mySupport = new SUPPORT(this,"Skin",MED_FACE);
  mySupport->setAll(false);

  list<int> myElementsList ;
  int i,j, size = 0 ;

  calculateConnectivity(MED_FULL_INTERLACE, MED_DESCENDING, MED_CELL);
  if (Support3D->isOnAllElements())
  {
    int * myConnectivityValue = const_cast <int*> (getReverseConnectivity(MED_DESCENDING)) ;
    int nbFaces = getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS);
    for (i=0, j=1 ; j<=nbFaces; ++j, i += 2)
    {
      int cellNb1 = myConnectivityValue [i];
      int cellNb2 = myConnectivityValue [i+1];
      //MESSAGE( " FACE # " << j << " -- Cells: " << cellNb1 << ", " << cellNb2 );
      if ((cellNb1 == 0 || cellNb2 == 0) && (cellNb1 + cellNb2 > 0))
      {
        myElementsList.push_back( j ) ;
        size++ ;
      }
    }
  }
  else
  {
    map<int,int> FaceNbEncounterNb;

    int * myConnectivityValue = const_cast <int *>
      (getConnectivity(MED_FULL_INTERLACE, MED_DESCENDING,
		       MED_CELL, MED_ALL_ELEMENTS));
    int * myConnectivityIndex = const_cast <int *> (getConnectivityIndex(MED_DESCENDING, MED_CELL));
    int * myCellNbs = const_cast <int *> (Support3D->getnumber()->getValue());
    int nbCells = Support3D->getnumber()->getLength();
    for (i=0; i<nbCells; ++i)
    {
      int cellNb = myCellNbs [ i ];
      int faceFirst = myConnectivityIndex[ cellNb-1 ];
      int faceLast  = myConnectivityIndex[ cellNb ];
      for (j = faceFirst; j < faceLast; ++j)
      {
        int faceNb = abs( myConnectivityValue [ j-1 ] );
        //MESSAGE( "Cell # " << i << " -- Face: " << faceNb);
        if (FaceNbEncounterNb.find( faceNb ) == FaceNbEncounterNb.end())
          FaceNbEncounterNb[ faceNb ] = 1;
        else
          FaceNbEncounterNb[ faceNb ] ++;
      }
    }
    map<int,int>::iterator FaceNbEncounterNbItr;
    for (FaceNbEncounterNbItr = FaceNbEncounterNb.begin();
         FaceNbEncounterNbItr != FaceNbEncounterNb.end();
         FaceNbEncounterNbItr ++)
      if ((*FaceNbEncounterNbItr).second == 1)
      {
        myElementsList.push_back( (*FaceNbEncounterNbItr).first) ;
        size++ ;
      }
  }
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::iterator myElementsListIt ;
  for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
    myListArray[id]=(*myElementsListIt) ;
    id ++ ;
  }

  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * geometricTypeNumber ;
  int * numberOfEntities ;
  //  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY() ;
  int * mySkyLineArrayIndex ;

  int numberOfType = getNumberOfTypes(MED_FACE) ;
  if (numberOfType == 1) { // wonderfull : it's easy !
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    const medGeometryElement *  allType = getTypes(MED_FACE);
    geometricType[0] = allType[0] ;
    geometricTypeNumber = new int[1] ; // not use, but initialized to nothing
    geometricTypeNumber[0] = 0 ;
    numberOfEntities = new int[1] ;
    numberOfEntities[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
  }
  else {// hemmm
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
      medGeometryElement myType = getElementType(MED_FACE,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
	theType[myType]+=1 ;
      else
	theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    //const medGeometryElement *  allType = getTypes(MED_FACE); !! UNUSED VARIABLE !!
    geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
    numberOfEntities = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      geometricTypeNumber[index] = 0 ;
      numberOfEntities[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfEntities[index] ;
      index++ ;
    }
  }
  //  mySkyLineArray->setMEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;

  mySupport->setNumberOfGeometricType(numberOfGeometricType) ;
  mySupport->setGeometricType(geometricType) ;
  //  mySupport->setGeometricTypeNumber(geometricTypeNumber) ;
  mySupport->setNumberOfElements(numberOfEntities) ;
  //mySupport->setTotalNumberOfElements(size) ;
  mySupport->setNumber(mySkyLineArray) ;

  delete[] numberOfEntities;
  delete[] geometricTypeNumber;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;
//   delete mySkyLineArray;

  END_OF();
  return mySupport ;

}

/*!
  return a SUPPORT pointer on the union of all SUPPORTs in Supports.
  You should delete this pointer after use to avoid memory leaks.
*/
SUPPORT * MESH::mergeSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION)
{
  const char * LOC = "MESH:::mergeSupports(const vector<SUPPORT *> ) : " ;
  BEGIN_OF(LOC) ;

  SUPPORT * returnedSupport;
  string returnedSupportName;
  string returnedSupportDescription;
  char * returnedSupportNameChar;
  char * returnedSupportDescriptionChar;
  int size = Supports.size();

  if (size == 0)
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) <<
                                     " mergeSupports() does't accept zero size vector"));
    
  if (size == 1)
    {
      MESSAGE(LOC <<" there is only one SUPPORT in the argument list, the method return a copy of this object !");
      SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);

      returnedSupport = new SUPPORT(*obj);

      int lenName = strlen((Supports[0]->getName()).c_str()) + 8 + 1;
      int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 8 + 1;

      returnedSupportNameChar = new char[lenName];
      returnedSupportDescriptionChar = new char[lenDescription];

      returnedSupportNameChar = strcpy(returnedSupportNameChar,"Copy of ");
      returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
      returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Copy of ");
      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
					      (Supports[0]->getDescription()).c_str());

      returnedSupportName = string(returnedSupportNameChar);
      returnedSupportDescription = string(returnedSupportDescriptionChar);

      returnedSupport->setName(returnedSupportName);
      returnedSupport->setDescription(returnedSupportDescription);

      delete [] returnedSupportNameChar;
      delete [] returnedSupportDescriptionChar;
    }
  else
    {
      SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);
      returnedSupport = new SUPPORT(*obj);

      int lenName = strlen((Supports[0]->getName()).c_str()) + 9 + 1;
      int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 9 + 1;

      for(int i = 1;i<size;i++)
	{
	  obj = const_cast <SUPPORT *> (Supports[i]);
	  returnedSupport->blending(obj);

	  if (i == (size-1))
	    {
	      lenName = lenName + 5 + strlen((Supports[i]->getName()).c_str());
	      lenDescription = lenDescription + 5 +
		strlen((Supports[i]->getDescription()).c_str());
	    }
	  else
	    {
	      lenName = lenName + 2 + strlen((Supports[i]->getName()).c_str());
	      lenDescription = lenDescription + 2 +
		strlen((Supports[i]->getDescription()).c_str());
	    }
	}

      returnedSupportNameChar = new char[lenName];
      returnedSupportDescriptionChar = new char[lenDescription];

      returnedSupportNameChar = strcpy(returnedSupportNameChar,"Merge of ");
      returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Merge of ");

      returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
					      (Supports[0]->getDescription()).c_str());

      for(int i = 1;i<size;i++)
	{
	  if (i == (size-1))
	    {
	      returnedSupportNameChar = strcat(returnedSupportNameChar," and ");
	      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar," and ");

	      returnedSupportNameChar = strcat(returnedSupportNameChar,
					       (Supports[i]->getName()).c_str());
	      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
						      (Supports[i]->getDescription()).c_str());
	    }
	  else
	    {
	      returnedSupportNameChar = strcat(returnedSupportNameChar,", ");
	      returnedSupportNameChar = strcat(returnedSupportNameChar,
					       (Supports[i]->getName()).c_str());

	      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,", ");
	      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
						      (Supports[i]->getDescription()).c_str());
	    }
	}

      returnedSupportName = string(returnedSupportNameChar);
      returnedSupport->setName(returnedSupportName);

      returnedSupportDescription = string(returnedSupportDescriptionChar);
      returnedSupport->setDescription(returnedSupportDescription);

      delete [] returnedSupportNameChar;
      delete [] returnedSupportDescriptionChar;
    }

  END_OF();
  return returnedSupport;
}

/*!
  return a SUPPORT pointer on the intersection of all SUPPORTs in Supports.
  The (SUPPORT *) NULL pointer is returned if the intersection is empty.
  You should delete this pointer after use to avois memory leaks.
*/
SUPPORT * MESH::intersectSupports(const vector<SUPPORT *> Supports) throw (MEDEXCEPTION)
{
  //const char * LOC = "MESH:::intersectSupports(const vector<SUPPORT *> ) : " ;
  BEGIN_OF("MESH:::intersectSupports(const vector<SUPPORT *> ) : ") ;

  SUPPORT * returnedSupport;
  string returnedSupportName;
  string returnedSupportDescription;
  char * returnedSupportNameChar;
  char * returnedSupportDescriptionChar;
  int size = Supports.size();

  if (size == 1)
    {
      MESSAGE(__LOC <<" there is only one SUPPORT in the argument list, the method return a copy of this object !");
      SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);

      returnedSupport = new SUPPORT(*obj);

      int lenName = strlen((Supports[0]->getName()).c_str()) + 8 + 1;
      int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 8 + 1;

      returnedSupportNameChar = new char[lenName];
      returnedSupportDescriptionChar = new char[lenDescription];

      returnedSupportNameChar = strcpy(returnedSupportNameChar,"Copy of ");
      returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
      returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Copy of ");
      returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
					      (Supports[0]->getDescription()).c_str());

      returnedSupportName = string(returnedSupportNameChar);
      returnedSupportDescription = string(returnedSupportDescriptionChar);

      returnedSupport->setName(returnedSupportName);
      returnedSupport->setDescription(returnedSupportDescription);

      delete [] returnedSupportNameChar;
      delete [] returnedSupportDescriptionChar;
    }
  else
    {
      SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);
      returnedSupport = new SUPPORT(*obj);

      int lenName = strlen((Supports[0]->getName()).c_str()) + 16 + 1;
      int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 16 + 1;

      for(int i = 1;i<size;i++)
	{
	  obj = const_cast <SUPPORT *> (Supports[i]);
	  returnedSupport->intersecting(obj);

	  if (i == (size-1))
	    {
	      lenName = lenName + 5 + strlen((Supports[i]->getName()).c_str());
	      lenDescription = lenDescription + 5 +
		strlen((Supports[i]->getDescription()).c_str());
	    }
	  else
	    {
	      lenName = lenName + 2 + strlen((Supports[i]->getName()).c_str());
	      lenDescription = lenDescription + 2 +
		strlen((Supports[i]->getDescription()).c_str());
	    }
	}
      if(returnedSupport != (SUPPORT *) NULL)
	{
	  returnedSupportNameChar = new char[lenName];
	  returnedSupportDescriptionChar = new char[lenDescription];

	  returnedSupportNameChar = strcpy(returnedSupportNameChar,
					   "Intersection of ");
	  returnedSupportDescriptionChar =
	    strcpy(returnedSupportDescriptionChar,"Intersection of ");

	  returnedSupportNameChar = strcat(returnedSupportNameChar,
					   (Supports[0]->getName()).c_str());
	  returnedSupportDescriptionChar =
	    strcat(returnedSupportDescriptionChar,
		   (Supports[0]->getDescription()).c_str());

	  for(int i = 1;i<size;i++)
	    {
	      if (i == (size-1))
		{
		  returnedSupportNameChar = strcat(returnedSupportNameChar,
						   " and ");
		  returnedSupportDescriptionChar =
		    strcat(returnedSupportDescriptionChar," and ");

		  returnedSupportNameChar =
		    strcat(returnedSupportNameChar,
			   (Supports[i]->getName()).c_str());
		  returnedSupportDescriptionChar =
		    strcat(returnedSupportDescriptionChar,
			   (Supports[i]->getDescription()).c_str());
		}
	      else
		{
		  returnedSupportNameChar = strcat(returnedSupportNameChar,
						   ", ");
		  returnedSupportNameChar =
		    strcat(returnedSupportNameChar,
			   (Supports[i]->getName()).c_str());

		  returnedSupportDescriptionChar =
		    strcat(returnedSupportDescriptionChar,", ");
		  returnedSupportDescriptionChar =
		    strcat(returnedSupportDescriptionChar,
			   (Supports[i]->getDescription()).c_str());
		}
	    }

	  returnedSupportName = string(returnedSupportNameChar);
	  returnedSupport->setName(returnedSupportName);

	  returnedSupportDescription = string(returnedSupportDescriptionChar);
	  returnedSupport->setDescription(returnedSupportDescription);

	  delete [] returnedSupportNameChar;
	  delete [] returnedSupportDescriptionChar;
	}
    }

  END_OF();
  return returnedSupport;
}


// internal helper type
struct _cell
{
    std::vector<int> groups;
    MED_EN::medGeometryElement geometricType;
};

/*!
  Create groups from families.

  It is used to create groups that have only one family
  for meshes that come from codes that use families instead 
  of groups to define a subregion.
*/
void MESH::createGroups()
{
  for (medEntityMesh entity=MED_CELL; entity!=MED_ALL_ENTITIES; ++entity)
    {
      // make myFamilies points to the member corresponding to entity
      vector<FAMILY*>* myFamilies;
      vector<GROUP*>* myGroups;
      switch ( entity )
        {
        case MED_CELL :
          myFamilies = & _familyCell;
          myGroups = & _groupCell;
          break;
        case MED_FACE :
          myFamilies = & _familyFace;
          myGroups = & _groupFace;
          break;
        case MED_EDGE :
          myFamilies = & _familyEdge;
          myGroups = & _groupEdge;
          break;
        case MED_NODE :
          myFamilies = & _familyNode;
          myGroups = & _groupNode;
          break;
        }
      
      
      for (int i=0; i< myFamilies->size(); i++)
        {
          list <FAMILY*> fam_list;
          fam_list.push_back((*myFamilies)[i]);
          //creates a group with the family name and only one family
          GROUP* group=new GROUP((*myFamilies)[i]->getName(),fam_list);
          (*myGroups).push_back(group);
        }
    }
}
// Create families from groups
void MESH::createFamilies()
{
    int idFamNode = 0; // identifier for node families
    int idFamElement = 0; // identifier for cell, face or edge families

    // Main loop on mesh's entities
    for (medEntityMesh entity=MED_CELL; entity!=MED_ALL_ENTITIES; ++entity)
    {
	int numberofgroups = getNumberOfGroups(entity);
	if(!numberofgroups)
	    continue; // no groups for this entity

	vector< vector<FAMILY*> > whichFamilyInGroup(numberofgroups); // this container is used to update groups at the end

	// make myFamilies points to the member corresponding to entity
	vector<FAMILY*>* myFamilies;
	switch ( entity )
	{
	    case MED_CELL :
		myFamilies = & _familyCell;
		break;
	    case MED_FACE :
		myFamilies = & _familyFace;
		break;
	    case MED_EDGE :
		myFamilies = & _familyEdge;
		break;
	    case MED_NODE :
		myFamilies = & _familyNode;
		break;
	}

	vector<GROUP*> myGroups=getGroups(entity); // get a copy of the groups ptr for the entity
	// get a copy of the (old) family ptrs before clearing
	vector<FAMILY*> myOldFamilies=getFamilies(entity);
	myFamilies->clear();


	// 1 - Create a vector containing for each cell (of the entity) an information structure
	//     giving geometric type and the groups it belong to

	med_int numberOfTypes=getNumberOfTypes(entity);
	const int * index=getGlobalNumberingIndex(entity);
	const medGeometryElement* geometricTypes=_connectivity->getGeometricTypes(entity); // pb avec entity=MED_NODE???
	med_int numberOfCells=index[numberOfTypes]-1;  // total number of cells for that entity
	SCRUTE(numberOfTypes);
	SCRUTE(numberOfCells);
	vector< _cell > tab_cell(numberOfCells);
	for(med_int t=0; t!=numberOfTypes; ++t)
	    for(int n=index[t]-1; n!=index[t+1]-1; ++n)
		tab_cell[n].geometricType=geometricTypes[t];


	// 2 - Scan cells in groups and update in tab_cell the container of groups a cell belong to

	for (unsigned g=0; g!=myGroups.size(); ++g)
	{
	    // scan cells that belongs to the group
	    const int* groupCells=myGroups[g]->getnumber()->getValue();
	    int nbCells=myGroups[g]->getnumber()->getLength();
	    for(int c=0; c!=nbCells; ++c)
		tab_cell[groupCells[c]-1].groups.push_back(g);
	}


	// 3 - Scan the cells vector, genarate family name, and create a map associating the family names
	//     whith the vector of contained cells

	map< string,vector<int> > tab_families;
	map< string,vector<int> >::iterator fam;
	for(int n=0; n!=numberOfCells; ++n)
	{
	    ostringstream key; // to generate the name of the family
	    key << "FAM";
	    if(tab_cell[n].groups.empty()) // this cell don't belong to any group
		key << "_NONE" << entity;

	    for(vector<int>::const_iterator it=tab_cell[n].groups.begin(); it!=tab_cell[n].groups.end(); ++it)
	    {
		string groupName=myGroups[*it]->getName();
		if(groupName.empty())
		    key << "_G" << *it;
		else
		    key << "_" << groupName;
	    }

	    tab_families[key.str()].push_back(n+1); // fill the vector of contained cells associated whith the family
	}


	// 4 - Scan the family map, create MED Families, check if it already exist.

	for( fam=tab_families.begin(); fam!=tab_families.end(); ++fam)
	{
	    vector<medGeometryElement> tab_types_geometriques;
	    medGeometryElement geometrictype=MED_NONE;
	    vector<int> tab_index_types_geometriques;
	    vector<int> tab_nombres_elements;
            if ( fam->second.empty() )
              continue; // it is just a truncated long family name

	    // scan family cells and fill the tab that are needed by the create a MED FAMILY
	    for( int i=0; i!=fam->second.size(); ++i)
	    {
		int ncell=fam->second[i]-1;
		if(tab_cell[ncell].geometricType != geometrictype)
		{
		    // new geometric type -> we store it and complete index tabs
		    if(!tab_index_types_geometriques.empty())
			tab_nombres_elements.push_back(i+1-tab_index_types_geometriques.back());
		    tab_types_geometriques.push_back( (geometrictype=tab_cell[ncell].geometricType));
		    tab_index_types_geometriques.push_back(i+1);
		}
	    }
	    // store and complete index tabs for the last geometric type
	    tab_nombres_elements.push_back(fam->second.size()+1-tab_index_types_geometriques.back());
	    tab_index_types_geometriques.push_back(fam->second.size()+1);

            // family name sould not be longer than MED_TAILLE_NOM
            string famName = fam->first;
            if ( famName.size() > MED_TAILLE_NOM ) {
              // try to cut off "FAM_" from the head
              if ( famName.size() - 4 <= MED_TAILLE_NOM ) {
                famName = famName.substr(4);
              }
              else { // try to make a unique name by cutting off char by char from the tail
                famName.substr(0, MED_TAILLE_NOM);
                map< string,vector<int> >::iterator foundName = tab_families.find( famName );
                while ( foundName != tab_families.end() && !famName.empty() ) {
                  famName = famName.substr( 0, famName.size() - 1 );
                  foundName = tab_families.find( famName );
                }
              }
              tab_families[ famName ]; // add a new name in the table to assure uniqueness
            }

	    // create an empty MED FAMILY and fill it with the tabs we constructed
	    FAMILY* newFam = new FAMILY();
	    //newFam->setTotalNumberOfElements(fam->second.size());
	    newFam->setName(famName);
	    newFam->setMeshDirectly(this);
	    newFam->setNumberOfGeometricType(tab_types_geometriques.size());
	    newFam->setGeometricType(&tab_types_geometriques[0]); // we know the tab is not empy
	    newFam->setNumberOfElements(&tab_nombres_elements[0]);
	    newFam->setNumber(&tab_index_types_geometriques[0],&fam->second[0]);
	    newFam->setEntity(entity);
	    newFam->setAll(false);

	    int idFam = 0;

	    switch ( entity )
	      {
	      case MED_NODE :
		++idFamNode;
		idFam = idFamNode;
		break;
	      case MED_CELL:
		++idFamElement;
		idFam = -idFamElement;
		break;
	      case MED_FACE :
		++idFamElement;
		idFam = -idFamElement;
		break;
	      case MED_EDGE :
		++idFamElement;
		idFam = -idFamElement;
		break;
	      }

	    newFam->setIdentifier(idFam);

	    // Update links between families and groups

	    int ncell1=fam->second[0]-1;  // number of first cell in family
	    int numberOfGroups=tab_cell[ncell1].groups.size(); // number of groups in the family
	    if(numberOfGroups)
	    {
		newFam->setNumberOfGroups(numberOfGroups);
		string * groupNames=new string[numberOfGroups];

		// iterate on the groups the family belongs to
		vector<int>::const_iterator it=tab_cell[ncell1].groups.begin();
		for(int ng=0 ; it!=tab_cell[ncell1].groups.end(); ++it, ++ng)
		{
		    whichFamilyInGroup[*it].push_back(newFam);
		    groupNames[ng]=myGroups[*it]->getName();
		}
		newFam->setGroupsNames(groupNames);
	    }

	    int sizeOfFamVect = myFamilies->size();

	    MESSAGE("  MESH::createFamilies() entity " << entity << " size " << sizeOfFamVect);

	    myFamilies->push_back(newFam);
	}

	// delete old families
	for (unsigned int i=0;i<myOldFamilies.size();i++)
	    delete myOldFamilies[i] ;

	// update references in groups
	for (unsigned int i=0;i<myGroups.size();i++)
	{
	    myGroups[i]->setNumberOfFamilies(whichFamilyInGroup[i].size());
	    myGroups[i]->setFamilies(whichFamilyInGroup[i]);
	}

    // re-scan the cells vector, and fill the family vector with cells.
    // creation of support, check if it already exist.
    }
}

int MESH::getElementContainingPoint(const double *coord)
{
  if(_spaceDimension==3)
    {
      Meta_Wrapper<3> fromWrapper(getNumberOfNodes(),const_cast<double *>(getCoordinates(MED_FULL_INTERLACE)),
							const_cast<CONNECTIVITY *>(getConnectivityptr()));
      Meta_Wrapper<3> toWrapper(1,const_cast<double *>(coord));
      Meta_Mapping<3> mapping(&fromWrapper,&toWrapper);
      mapping.Cree_Mapping(1);
      return mapping.Get_Mapping()[0]+1;
    }
  else if(_spaceDimension==2)
    {
      Meta_Wrapper<2> fromWrapper(getNumberOfNodes(),const_cast<double *>(getCoordinates(MED_FULL_INTERLACE)),
							const_cast<CONNECTIVITY *>(getConnectivityptr()));
      Meta_Wrapper<2> toWrapper(1,const_cast<double *>(coord));
      Meta_Mapping<2> mapping(&fromWrapper,&toWrapper);
      mapping.Cree_Mapping(1);
      return mapping.Get_Mapping()[0]+1;
      }
  else
    throw MEDEXCEPTION("MESH::getElementContainingPoint : invalid _spaceDimension must be equal to 2 or 3 !!!");
}

vector< vector<double> > MESH::getBoundingBox() const
{
  const double *myCoords=_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE);
  vector< vector<double> > ret(2);
  int i,j;
  ret[0].resize(_spaceDimension);
  ret[1].resize(_spaceDimension);
  for(i=0;i<_spaceDimension;i++)
    {
      ret[0][i]=1.e300;
      ret[1][i]=-1.e300;
    }
  for(i=0;i<_coordinate->getNumberOfNodes();i++)
    {
      for(j=0;j<_spaceDimension;j++)
	{
	  double tmp=myCoords[i*_spaceDimension+j];
	  if(tmp<ret[0][j])
	    ret[0][j]=tmp;
	  if(tmp>ret[1][j])
	    ret[1][j]=tmp;
	}
    }
  return ret;
}

//Presently disconnected in C++
void MESH::addReference() const
{
}

//Presently disconnected in C++
void MESH::removeReference() const
{
}
