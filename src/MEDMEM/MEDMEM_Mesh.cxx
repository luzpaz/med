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
//  File   : MEDMEM_Mesh.cxx
//  Module : MED

using namespace std;
/*
 File Mesh.cxx
 $Header$
*/

#include <math.h>

#include <list>
#include <map>

#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"

//update Families with content list
//int family_count(int family_number, int count, int * entities_number, int * entities_list) ;

// ------- Drivers Management Part

// MESH::INSTANCE_DE<MED_MESH_RDONLY_DRIVER> MESH::inst_med_rdonly ;
//const MESH::INSTANCE * const MESH::instances[] = { &MESH::inst_med_rdonly , &MESH::inst_med_rdwr } ;

// Add a similar line for your personnal driver (step 3)
MESH::INSTANCE_DE<MED_MESH_RDWR_DRIVER>   MESH::inst_med   ;
MESH::INSTANCE_DE<GIBI_MESH_RDWR_DRIVER>  MESH::inst_gibi ;
// Add your own driver in the driver list       (step 4)
// Note the list must be coherent with the driver type list defined in MEDMEM_DRIVER.hxx. 
const MESH::INSTANCE * const MESH::instances[] =   {  &MESH::inst_med, &MESH::inst_gibi } ;

/*! Add a MESH driver of type (MED_DRIVER, ....) associated with file <fileName>. The meshname used in the file
    is  <driverName>. addDriver returns an int handler. */
int MESH::addDriver(driverTypes driverType, 
                    const string & fileName/*="Default File Name.med"*/,const string & driverName/*="Default Mesh Name"*/) {

  const char * LOC = "MESH::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Mesh Name\") : ";
  
  GENDRIVER * driver;
  int current;

  BEGIN_OF(LOC);
  
  driver = instances[driverType]->run(fileName, this) ;
  _drivers.push_back(driver);
  current = _drivers.size()-1;
  
  _drivers[current]->setMeshName(driverName);  
  return current;

  END_OF(LOC);

}

/*! Add an existing MESH driver. */
int  MESH::addDriver(GENDRIVER & driver) {
  const char * LOC = "MESH::addDriver(GENDRIVER &) : ";
  BEGIN_OF(LOC);

  // A faire : Vérifier que le driver est de type MESH.
  GENDRIVER * newDriver = driver.copy() ;

  _drivers.push_back(newDriver);
  return _drivers.size()-1;

  END_OF(LOC);
}

/*! Remove an existing MESH driver. */
void MESH::rmDriver (int index/*=0*/) {
  const char * LOC = "MESH::rmDriver (int index=0): ";
  BEGIN_OF(LOC);

  if ( _drivers[index] ) {
    //_drivers.erase(&_drivers[index]); 
    // why not ????
    MESSAGE ("detruire");
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "The index given is invalid, index must be between  0 and  |" 
                                     << _drivers.size() 
                                     )
                          );   
  
  END_OF(LOC);

}; 

// ------ End of Drivers Management Part


void MESH::init() {

  const char * LOC = "MESH::init(): ";

  BEGIN_OF(LOC);

  string        _name = "NOT DEFINED"; // A POSITIONNER EN FCT DES IOS ?

  COORDINATE   * _coordinate   = (COORDINATE   *) NULL;
  CONNECTIVITY * _connectivity = (CONNECTIVITY *) NULL;

  _spaceDimension        =          MED_INVALID; // 0 ?!?
  _meshDimension         =          MED_INVALID;
  _numberOfNodes         =          MED_INVALID;
  
  _numberOfNodesFamilies =          0; // MED_INVALID ?!?
  _numberOfCellsFamilies =          0;
  _numberOfFacesFamilies =          0;
  _numberOfEdgesFamilies =          0;

  _numberOfNodesGroups   =          0;  // MED_INVALID ?!?
  _numberOfCellsGroups   =          0; 
  _numberOfFacesGroups   =          0; 
  _numberOfEdgesGroups   =          0; 

  _isAGrid = false;
};

/*! Create an empty MESH. */
MESH::MESH():_coordinate(NULL),_connectivity(NULL) {
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

  _numberOfNodesFamilies = m._numberOfNodesFamilies;
  _familyNode = m._familyNode;
  for (int i=0; i<m._numberOfNodesFamilies; i++)
    {
      _familyNode[i] = new FAMILY(* m._familyNode[i]);
      _familyNode[i]->setMesh(this);
    }

  _numberOfCellsFamilies = m._numberOfCellsFamilies;
  _familyCell = m._familyCell;
  for (int i=0; i<m._numberOfCellsFamilies; i++)
    {
      _familyCell[i] = new FAMILY(* m._familyCell[i]);
      _familyCell[i]->setMesh(this);
    }

  _numberOfFacesFamilies = m._numberOfFacesFamilies;
  _familyFace = m._familyFace;
  for (int i=0; i<m._numberOfFacesFamilies; i++)
    {
      _familyFace[i] = new FAMILY(* m._familyFace[i]);
      _familyFace[i]->setMesh(this);
    }

  _numberOfEdgesFamilies = m._numberOfEdgesFamilies;
  _familyEdge = m._familyEdge;
  for (int i=0; i<m._numberOfEdgesFamilies; i++)
    {
      _familyEdge[i] = new FAMILY(* m._familyEdge[i]);
      _familyEdge[i]->setMesh(this);
    }

  _numberOfNodesGroups = m._numberOfNodesGroups;
  _groupNode = m._groupNode;
  for (int i=0; i<m._numberOfNodesGroups; i++)
    {
      _groupNode[i] = new GROUP(* m._groupNode[i]);
      _groupNode[i]->setMesh(this);
    }

  _numberOfCellsGroups = m._numberOfCellsGroups;
  _groupCell = m._groupCell;
  for (int i=0; i<m._numberOfCellsGroups; i++)
    {
      _groupCell[i] = new GROUP(* m._groupCell[i]);
      _groupCell[i]->setMesh(this);
    }

  _numberOfFacesGroups = m._numberOfFacesGroups;
  _groupFace = m._groupFace;
  for (int i=0; i<m._numberOfFacesGroups; i++)
    {
      _groupFace[i] = new GROUP(* m._groupFace[i]);
      _groupFace[i]->setMesh(this);
    }

  _numberOfEdgesGroups = m._numberOfEdgesGroups;
  _groupEdge = m._groupEdge;
  for (int i=0; i<m._numberOfEdgesGroups; i++)
    {
      _groupEdge[i] = new GROUP(* m._groupEdge[i]);
      _groupEdge[i]->setMesh(this);
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
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _familyNode[i] ;
  size = _familyCell.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _familyCell[i] ;
  size = _familyFace.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _familyFace[i] ;
  size = _familyEdge.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _familyEdge[i] ;
  size = _groupNode.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _groupNode[i] ;
  size = _groupCell.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _groupCell[i] ;
  size = _groupFace.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _groupFace[i] ;
  size = _groupEdge.size() ;
  for (int i=0;i<size;i++)
    ;// mpv such destructors call is problemmatic for ALLIANCES algorithms
    //delete _groupEdge[i] ;

  MESSAGE("In this object MESH there is(are) " << _drivers.size() << " driver(s)");

  for (int index=0; index < _drivers.size(); index++ )
    {
      SCRUTE(_drivers[index]);
      if ( _drivers[index] != NULL) delete _drivers[index];
    }

}

MESH & MESH::operator=(const MESH &m)
{

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
  return *this;
}

/*! Create a MESH object using a MESH driver of type (MED_DRIVER, ....) associated with file <fileName>. 
  The meshname <driverName> must already exists in the file.*/
MESH::MESH(driverTypes driverType, const string &  fileName/*=""*/, const string &  driverName/*=""*/) {
  const char * LOC ="MESH::MESH(driverTypes driverType, const string &  fileName="", const string &  driverName="") : ";
  
  int current;
  
  BEGIN_OF(LOC);

  init();

  MED_MESH_RDONLY_DRIVER myDriver(fileName,this) ;
  myDriver.setMeshName(driverName);
  current = addDriver(myDriver);
//   current = addDriver(driverType,fileName,driverName);
//   switch(_drivers[current]->getAccessMode() ) {
//   case MED_WRONLY : {
//     MESSAGE("MESH::MESH(driverTypes driverType, .....) : driverType must not be a MED_WRONLY accessMode");
//     rmDriver(current);
//     break;}
//   default : {
//   }
//   }
  _drivers[current]->open();   
  _drivers[current]->read();
  _drivers[current]->close();

  if (_isAGrid)
    ((GRID *) this)->fillMeshAfterRead();

  END_OF(LOC);
};


//  Node MESH::Donne_Barycentre(const Maille &m) const
//  {
//    Node N=node[m[0]];
//    for (int i=1;i<m.donne_nbr_sommets();i++) N=N+node[m[i]];
//    N=N*(1.0/m.donne_nbr_sommets());
//    return N;
//  }

ostream & operator<<(ostream &os, MESH &myMesh)
{
  int spacedimension = myMesh.getSpaceDimension();
  int meshdimension  = myMesh.getMeshDimension();
  int numberofnodes  = myMesh.getNumberOfNodes();

  os << "Space Dimension : " << spacedimension << endl << endl; 

  os << "Mesh Dimension : " << meshdimension << endl << endl; 

  const double * coordinates = myMesh.getCoordinates(MED_FULL_INTERLACE);
  os << "SHOW NODES COORDINATES : " << endl;

  os << "Name :" << endl;
  const string * coordinatesnames = myMesh.getCoordinatesNames();
  for(int i=0; i<spacedimension ; i++)
    {
      os << " - " << coordinatesnames[i] << endl;
    }
  os << "Unit :" << endl;
  const string * coordinatesunits = myMesh.getCoordinatesUnits();
  for(int i=0; i<spacedimension ; i++)
    {
      os << " - " << coordinatesunits[i] << endl;
    }
  for(int i=0; i<numberofnodes ; i++)
    {
      os << "Nodes " << i+1 << " : ";
      for (int j=0; j<spacedimension ; j++)
	os << coordinates[i*spacedimension+j] << " ";
      os << endl;
    }

  const CONNECTIVITY * myConnectivity = myMesh.getConnectivityptr();
  if (!myConnectivity->existConnectivity(MED_NODAL,MED_CELL) && myConnectivity->existConnectivity(MED_DESCENDING,MED_CELL))
    {
      os << endl << "SHOW CONNECTIVITY (DESCENDING) :" << endl;
      int numberofelements;
      const int * connectivity;
      const int * connectivity_index;
      myMesh.calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL);
      try {
	numberofelements = myMesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
	connectivity =  myMesh.getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
	connectivity_index =  myMesh.getConnectivityIndex(MED_DESCENDING,MED_CELL);
      }
      catch (MEDEXCEPTION m) {
	os << m.what() << endl;
	exit (-1);
      }
      for (int j=0;j<numberofelements;j++) {
	os << "Element "<<j+1<<" : ";
	for (int k=connectivity_index[j];k<connectivity_index[j+1];k++)
	  os << connectivity[k-1]<<" ";
	os << endl;
      }
    }
  else
    {
      int numberoftypes = myMesh.getNumberOfTypes(MED_CELL);
      const medGeometryElement  * types = myMesh.getTypes(MED_CELL);
      os << endl << "SHOW CONNECTIVITY (NODAL) :" << endl;
      for (int i=0; i<numberoftypes; i++) {
	os << "For type " << types[i] << " : " << endl;
	int numberofelements = myMesh.getNumberOfElements(MED_CELL,types[i]);
	const int * connectivity =  myMesh.getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,types[i]);
	int numberofnodespercell = types[i]%100;
	for (int j=0;j<numberofelements;j++){
	  os << "Element "<< j+1 <<" : ";
	  for (int k=0;k<numberofnodespercell;k++)
	    os << connectivity[j*numberofnodespercell+k]<<" ";
	  os << endl;
	}
      }
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
      using namespace MED_FR;
      os << "NumberOfFamilies on "<< entNames[(MED_FR::med_entite_maillage) entity] <<" : "<<numberoffamilies<<endl;
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
      using namespace MED_FR;
      os << "NumberOfGroups on "<< entNames[(MED_FR::med_entite_maillage) entity] <<" : "<<numberofgroups<<endl;
      using namespace MED_EN;
      for (int i=1; i<numberofgroups+1;i++)
	{
	  os << * myMesh.getGroup(entity,i) << endl;
	}
    }

  return os;
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

  END_OF(LOC);

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
  if (_spaceDimension == 3) 
    if (Entity != MED_FACE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<Entity<<" !"));
  if (_spaceDimension == 2) 
    if (Entity != MED_EDGE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<Entity<<" !"));
  
  // well, all rigth !
  SUPPORT * mySupport = new SUPPORT(this,"Boundary",Entity);
  //mySupport.setDescription("boundary of type ...");
  mySupport->setAll(false);
  

  const int * myConnectivityValue = getReverseConnectivity(MED_DESCENDING) ;
  const int * myConnectivityIndex = getReverseConnectivityIndex(MED_DESCENDING) ;
  int numberOf = getNumberOfElements(Entity,MED_ALL_ELEMENTS) ;
  list<int> myElementsList ;
  int size = 0 ;
  for (int i=0 ; i<numberOf; i++)
    if (myConnectivityValue[myConnectivityIndex[i]] == 0) {
      myElementsList.push_back(i+1) ;
      size++ ;
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
  int * numberOfGaussPoint ;
  int * geometricTypeNumber ;
  int * numberOfElements ;
  //MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY() ;
  int * mySkyLineArrayIndex ;

  int numberOfType = getNumberOfTypes(Entity) ;
  if (numberOfType == 1) { // wonderfull : it's easy !
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    const medGeometryElement *  allType = getTypes(Entity);
    geometricType[0] = allType[0] ;
    numberOfGaussPoint = new int[1] ;
    numberOfGaussPoint[0] = 1 ;
    geometricTypeNumber = new int[1] ; // not use, but initialized to nothing
    geometricTypeNumber[0] = 0 ;
    numberOfElements = new int[1] ;
    numberOfElements[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
  }
  else {// hemmm
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
      medGeometryElement myType = getElementType(Entity,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
	theType[myType]+=1 ;
      else
	theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    const medGeometryElement *  allType = getTypes(Entity);
    numberOfGaussPoint = new int[numberOfGeometricType] ;
    geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
    numberOfElements = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      numberOfGaussPoint[index] = 1 ;
      geometricTypeNumber[index] = 0 ;
      numberOfElements[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfElements[index] ;
      index++ ;
    }
  }
  //mySkyLineArray->setMEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;

  mySupport->setNumberOfGeometricType(numberOfGeometricType) ;
  mySupport->setGeometricType(geometricType) ;
  mySupport->setNumberOfGaussPoint(numberOfGaussPoint) ;
  mySupport->setNumberOfElements(numberOfElements) ;
  mySupport->setTotalNumberOfElements(size) ;
  // mySupport->setGeometricTypeNumber(geometricTypeNumber) ;
  mySupport->setNumber(mySkyLineArray) ;
    
  delete[] numberOfElements;
  delete[] geometricTypeNumber;
  delete[] numberOfGaussPoint;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;
//   delete mySkyLineArray;

  END_OF(LOC) ;
  return mySupport ;
}

FIELD<double>* MESH::getVolume(const SUPPORT * Support) const throw (MEDEXCEPTION)
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

//    if (onAll)
//      {
//        nb_type = myMesh->getNumberOfTypes(support_entity);
//        length_values = getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
//        types = getTypes(support_entity);
//      }
//    else
    {
      nb_type = Support->getNumberOfTypes();
      length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
      types = Support->getTypes();
    }

  int index;
  FIELD<double>* Volume = new FIELD<double>::FIELD(Support,1);
  //  double *volume = new double [length_values];
  Volume->setName("VOLUME");
  Volume->setDescription("cells volume");
  Volume->setComponentName(1,"volume");
  Volume->setComponentDescription(1,"desc-comp");

  /*  string MEDComponentUnit(MED_TAILLE_PNOM,' ');*/

  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1]+"*"+myMesh->getCoordinatesUnits()[2];

  Volume->setMEDComponentUnit(1,MEDComponentUnit);

  Volume->setValueType(MED_REEL64);

  Volume->setIterationNumber(0);
  Volume->setOrderNumber(0);
  Volume->setTime(0.0);

  //const double *volume = Volume->getValue(MED_FULL_INTERLACE);
  MEDARRAY<double> *volume = Volume->getvalue();

  index = 1;
  const double * coord = getCoordinates(MED_FULL_INTERLACE);

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      double xvolume;

      if (onAll)
	{
	  nb_entity_type = getNumberOfElements(support_entity,type);
	  global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	}
      else
	{
	  // throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on all"));

 	  nb_entity_type = Support->getNumberOfElements(type);
	  
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];

		xvolume = ((x3-x1)*((y2-y1)*(z4-z1) - (z2-z1)*(y4-y1)) -
			   (x2-x1)*((y3-y1)*(z4-z1) - (z3-z1)*(y4-y1)) +
			   (x4-x1)*((y3-y1)*(z2-z1) - (z3-z1)*(y2-y1)))/6.0;

		//volume[index] = xvolume ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];

		xvolume = (((x3-x1)*((y2-y1)*(z5-z1) - (z2-z1)*(y5-y1)) -
			    (x2-x1)*((y3-y1)*(z5-z1) - (z3-z1)*(y5-y1)) +
			    (x5-x1)*((y3-y1)*(z2-z1) - (z3-z1)*(y2-y1))) +
			   ((x4-x1)*((y3-y1)*(z5-z1) - (z3-z1)*(y5-y1)) -
			    (x3-x1)*((y4-y1)*(z5-z1) - (z4-z1)*(y5-y1)) +
			    (x5-x1)*((y4-y1)*(z3-z1) - (z4-z1)*(y3-y1)))
			   )/6.0;

		//volume[index] = xvolume ;
		volume->setIJ(index,1,xvolume) ;
		index = index++;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];
		double x6 = coord[dim_space*N6];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];
		double y6 = coord[dim_space*N6+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];
		double z6 = coord[dim_space*N6+2];

		double a1 = (x2-x3)/2.0, a2 = (y2-y3)/2.0, a3 = (z2-z3)/2.0;
		double b1 = (x5-x6)/2.0, b2 = (y5-y6)/2.0, b3 = (z5-z6)/2.0;
		double c1 = (x4-x1)/2.0, c2 = (y4-y1)/2.0, c3 = (z4-z1)/2.0;
		double d1 = (x5-x2)/2.0, d2 = (y5-y2)/2.0, d3 = (z5-z2)/2.0;
		double e1 = (x6-x3)/2.0, e2 = (y6-y3)/2.0, e3 = (z6-z3)/2.0;
		double f1 = (x1-x3)/2.0, f2 = (y1-y3)/2.0, f3 = (z1-z3)/2.0;
		double h1 = (x4-x6)/2.0, h2 = (y4-y6)/2.0, h3 = (z4-z6)/2.0;

		double A = a1*c2*f3 - a1*c3*f2 - a2*c1*f3 + a2*c3*f1 +
		  a3*c1*f2 - a3*c2*f1;
		double B = b1*c2*h3 - b1*c3*h2 - b2*c1*h3 + b2*c3*h1 +
		  b3*c1*h2 - b3*c2*h1;
		double C = (a1*c2*h3 + b1*c2*f3) - (a1*c3*h2 + b1*c3*f2) -
		  (a2*c1*h3 + b2*c1*f3) + (a2*c3*h1 + b2*c3*f1) +
		  (a3*c1*h2 + b3*c1*f2) - (a3*c2*h1 + b3*c2*f1);
		double D = a1*d2*f3 - a1*d3*f2 - a2*d1*f3 + a2*d3*f1 +
		  a3*d1*f2 - a3*d2*f1;
		double E = b1*d2*h3 - b1*d3*h2 - b2*d1*h3 + b2*d3*h1 +
		  b3*d1*h2 - b3*d2*h1;
		double F = (a1*d2*h3 + b1*d2*f3) - (a1*d3*h2 + b1*d3*f2) -
		  (a2*d1*h3 + b2*d1*f3) + (a2*d3*h1 + b2*d3*f1) +
		  (a3*d1*h2 + b3*d1*f2) - (a3*d2*h1 + b3*d2*f1);
		double G = a1*e2*f3 - a1*e3*f2 - a2*e1*f3 + a2*e3*f1 +
		  a3*e1*f2 - a3*e2*f1;
		double H = b1*e2*h3 - b1*e3*h2 - b2*e1*h3 + b2*e3*h1 +
		  b3*e1*h2 - b3*e2*h1;
		double P = (a1*e2*h3 + b1*e2*f3) - (a1*e3*h2 + b1*e3*f2) -
		  (a2*e1*h3 + b2*e1*f3) + (a2*e3*h1 + b2*e3*f1) +
		  (a3*e1*h2 + b3*e1*f2) - (a3*e2*h1 + b3*e2*f1);

		xvolume = -2.0*(2.0*(A + B + D + E + G + H) + C + F + P)/9.0;

		//volume[index] = xvolume ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];
		double x6 = coord[dim_space*N6];
		double x7 = coord[dim_space*N7];
		double x8 = coord[dim_space*N8];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];
		double y6 = coord[dim_space*N6+1];
		double y7 = coord[dim_space*N7+1];
		double y8 = coord[dim_space*N8+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];
		double z6 = coord[dim_space*N6+2];
		double z7 = coord[dim_space*N7+2];
		double z8 = coord[dim_space*N8+2];

		double a1 = (x3-x4)/8.0, a2 = (y3-y4)/8.0, a3 = (z3-z4)/8.0;
		double b1 = (x2-x1)/8.0, b2 = (y2-y1)/8.0, b3 = (z2-z1)/8.0;
		double c1 = (x7-x8)/8.0, c2 = (y7-y8)/8.0, c3 = (z7-z8)/8.0;
		double d1 = (x6-x5)/8.0, d2 = (y6-y5)/8.0, d3 = (z6-z5)/8.0;
		double e1 = (x3-x2)/8.0, e2 = (y3-y2)/8.0, e3 = (z3-z2)/8.0;
		double f1 = (x4-x1)/8.0, f2 = (y4-y1)/8.0, f3 = (z4-z1)/8.0;
		double h1 = (x7-x6)/8.0, h2 = (y7-y6)/8.0, h3 = (z7-z6)/8.0;
		double p1 = (x8-x5)/8.0, p2 = (y8-y5)/8.0, p3 = (z8-z5)/8.0;
		double q1 = (x3-x7)/8.0, q2 = (y3-y7)/8.0, q3 = (z3-z7)/8.0;
		double r1 = (x4-x8)/8.0, r2 = (y4-y8)/8.0, r3 = (z4-z8)/8.0;
		double s1 = (x2-x6)/8.0, s2 = (y2-y6)/8.0, s3 = (z2-z6)/8.0;
		double t1 = (x1-x5)/8.0, t2 = (y1-y5)/8.0, t3 = (z1-z5)/8.0;

		double A = a1*e2*q3 - a1*e3*q2 - a2*e1*q3 + a2*e3*q1 +
		  a3*e1*q2 - a3*e2*q1;
		double B = c1*h2*q3 - c1*h3*q2 - c2*h1*q3 + c2*h3*q1 +
		  c3*h1*q2 - c3*h2*q1;
		double C = (a1*h2 + c1*e2)*q3 - (a1*h3 + c1*e3)*q2 -
		  (a2*h1 + c2*e1)*q3 + (a2*h3 + c2*e3)*q1 +
		  (a3*h1 + c3*e1)*q2 - (a3*h2 + c3*e2)*q1;
		double D = b1*e2*s3 - b1*e3*s2 - b2*e1*s3 + b2*e3*s1 +
		  b3*e1*s2 - b3*e2*s1;
		double E = d1*h2*s3 - d1*h3*s2 - d2*h1*s3 + d2*h3*s1 +
		  d3*h1*s2 - d3*h2*s1;
		double F = (b1*h2 + d1*e2)*s3 - (b1*h3 + d1*e3)*s2 -
		  (b2*h1 + d2*e1)*s3 + (b2*h3 + d2*e3)*s1 +
		  (b3*h1 + d3*e1)*s2 - (b3*h2 + d3*e2)*s1;
		double G = (a1*e2*s3 + b1*e2*q3) - (a1*e3*s2 + b1*e3*q2) -
		  (a2*e1*s3 + b2*e1*q3) + (a2*e3*s1 + b2*e3*q1) +
		  (a3*e1*s2 + b3*e1*q2) - (a3*e2*s1 + b3*e2*q1);
		double H = (c1*h2*s3 + d1*h2*q3) - (c1*h3*s2 + d1*h3*q2) -
		  (c2*h1*s3 + d2*h1*q3) + (c2*h3*s1 + d2*h3*q1) +
		  (c3*h1*s2 + d3*h1*q2) - (c3*h2*s1 + d3*h2*q1);
		double I = ((a1*h2 + c1*e2)*s3 + (b1*h2 + d1*e2)*q3) -
		  ((a1*h3 + c1*e3)*s2 + (b1*h3 + d1*e3)*q2) -
		  ((a2*h1 + c2*e1)*s3 + (b2*h1 + d2*e1)*q3) +
		  ((a2*h3 + c2*e3)*s1 + (b2*h3 + d2*e3)*q1) +
		  ((a3*h1 + c3*e1)*s2 + (b3*h1 + d3*e1)*q2) -
		  ((a3*h2 + c3*e2)*s1 + (b3*h2 + d3*e2)*q1);
		double J = a1*f2*r3 - a1*f3*r2 - a2*f1*r3 + a2*f3*r1 +
		  a3*f1*r2 - a3*f2*r1;
		double K = c1*p2*r3 - c1*p3*r2 - c2*p1*r3 + c2*p3*r1 +
		  c3*p1*r2 - c3*p2*r1;
		double L = (a1*p2 + c1*f2)*r3 - (a1*p3 + c1*f3)*r2 -
		  (a2*p1 + c2*f1)*r3 + (a2*p3 + c2*f3)*r1 +
		  (a3*p1 + c3*f1)*r2 - (a3*p2 + c3*f2)*r1;
		double M = b1*f2*t3 - b1*f3*t2 - b2*f1*t3 + b2*f3*t1 +
		  b3*f1*t2 - b3*f2*t1;
		double N = d1*p2*t3 - d1*p3*t2 - d2*p1*t3 + d2*p3*t1 +
		  d3*p1*t2 - d3*p2*t1;
		double O = (b1*p2 + d1*f2)*t3 - (b1*p3 + d1*f3)*t2 -
		  (b2*p1 + d2*f1)*t3 + (b2*p3 + d2*f3)*t1 +
		  (b3*p1 + d3*f1)*t2 - (b3*p2 + d3*f2)*t1;
		double P = (a1*f2*t3 + b1*f2*r3) - (a1*f3*t2 + b1*f3*r2) -
		  (a2*f1*t3 + b2*f1*r3) + (a2*f3*t1 + b2*f3*r1) +
		  (a3*f1*t2 + b3*f1*r2) - (a3*f2*t1 + b3*f2*r1);
		double Q = (c1*p2*t3 + d1*p2*r3) - (c1*p3*t2 + d1*p3*r2) -
		  (c2*p1*t3 + d2*p1*r3) + (c2*p3*t1 + d2*p3*r1) +
		  (c3*p1*t2 + d3*p1*r2) - (c3*p2*t1 + d3*p2*r1);
		double R = ((a1*p2 + c1*f2)*t3 + (b1*p2 + d1*f2)*r3) -
		  ((a1*p3 + c1*f3)*t2 + (b1*p3 + d1*f3)*r2) -
		  ((a2*p1 + c2*f1)*t3 + (b2*p1 + d2*f1)*r3) +
		  ((a2*p3 + c2*f3)*t1 + (b2*p3 + d2*f3)*r1) +
		  ((a3*p1 + c3*f1)*t2 + (b3*p1 + d3*f1)*r2) -
		  ((a3*p2 + c3*f2)*t1 + (b3*p2 + d3*f2)*r1);
		double S = (a1*e2*r3 + a1*f2*q3) - (a1*e3*r2 + a1*f3*q2) -
		  (a2*e1*r3 + a2*f1*q3) + (a2*e3*r1 + a2*f3*q1) +
		  (a3*e1*r2 + a3*f1*q2) - (a3*e2*r1 + a3*f2*q1);
		double T = (c1*h2*r3 + c1*p2*q3) - (c1*h3*r2 + c1*p3*q2) -
		  (c2*h1*r3 + c2*p1*q3) + (c2*h3*r1 + c2*p3*q1) +
		  (c3*h1*r2 + c3*p1*q2) - (c3*h2*r1 + c3*p2*q1);
		double U = ((a1*h2 + c1*e2)*r3 + (a1*p2 + c1*f2)*q3) -
		  ((a1*h3 + c1*e3)*r2 + (a1*p3 + c1*f3)*q2) -
		  ((a2*h1 + c2*e1)*r3 + (a2*p1 + c2*f1)*q3) +
		  ((a2*h3 + c2*e3)*r1 + (a2*p3 + c2*f3)*q1) +
		  ((a3*h1 + c3*e1)*r2 + (a3*p1 + c3*f1)*q2) -
		  ((a3*h2 + c3*e2)*r1 + (a3*p2 + c3*f2)*q1);
		double V = (b1*e2*t3 + b1*f2*s3) - (b1*e3*t2 + b1*f3*s2) -
		  (b2*e1*t3 + b2*f1*s3) + (b2*e3*t1 + b2*f3*s1) +
		  (b3*e1*t2 + b3*f1*s2) - (b3*e2*t1 + b3*f2*s1);
		double W = (d1*h2*t3 + d1*p2*s3) - (d1*h3*t2 + d1*p3*s2) -
		  (d2*h1*t3 + d2*p1*s3) + (d2*h3*t1 + d2*p3*s1) +
		  (d3*h1*t2 + d3*p1*s2) - (d3*h2*t1 + d3*p2*s1);
		double X = ((b1*h2 + d1*e2)*t3 + (b1*p2 + d1*f2)*s3) -
		  ((b1*h3 + d1*e3)*t2 + (b1*p3 + d1*f3)*s2) -
		  ((b2*h1 + d2*e1)*t3 + (b2*p1 + d2*f1)*s3) +
		  ((b2*h3 + d2*e3)*t1 + (b2*p3 + d2*f3)*s1) +
		  ((b3*h1 + d3*e1)*t2 + (b3*p1 + d3*f1)*s2) -
		  ((b3*h2 + d3*e2)*t1 + (b3*p2 + d3*f2)*s1);
		double Y = (a1*e2*t3 + a1*f2*s3 + b1*e2*r3 + b1*f2*q3) -
		  (a1*e3*t2 + a1*f3*s2 + b1*e3*r2 + b1*f3*q2) -
		  (a2*e1*t3 + a2*f1*s3 + b2*e1*r3 + b2*f1*q3) +
		  (a2*e3*t1 + a2*f3*s1 + b2*e3*r1 + b2*f3*q1) +
		  (a3*e1*t2 + a3*f1*s2 + b3*e1*r2 + b3*f1*q2) -
		  (a3*e2*t1 + a3*f2*s1 + b3*e2*r1 + b3*f2*q1);
		double Z = (c1*h2*t3 + c1*p2*s3 + d1*h2*r3 + d1*p2*q3) -
		  (c1*h3*t2 + c1*p3*s2 + d1*h3*r2 + d1*p3*q2) -
		  (c2*h1*t3 + c2*p1*s3 + d2*h1*r3 + d2*p1*q3) +
		  (c2*h3*t1 + c2*p3*s1 + d2*h3*r1 + d2*p3*q1) +
		  (c3*h1*t2 + c3*p1*s2 + d3*h1*r2 + d3*p1*q2) -
		  (c3*h2*t1 + c3*p2*s1 + d3*h2*r1 + d3*p2*q1);
		double AA = ((a1*h2 + c1*e2)*t3 + (a1*p2 + c1*f2)*s3 +
			     (b1*h2 + d1*e2)*r3 + (b1*p2 + d1*f2)*q3) -
		  ((a1*h3 + c1*e3)*t2 + (a1*p3 + c1*f3)*s2 +
		   (b1*h3 + d1*e3)*r2 + (b1*p3 + d1*f3)*q2) -
		  ((a2*h1 + c2*e1)*t3 + (a2*p1 + c2*f1)*s3 +
		   (b2*h1 + d2*e1)*r3 + (b2*p1 + d2*f1)*q3) +
		  ((a2*h3 + c2*e3)*t1 + (a2*p3 + c2*f3)*s1 +
		   (b2*h3 + d2*e3)*r1 + (b2*p3 + d2*f3)*q1) +
		  ((a3*h1 + c3*e1)*t2 + (a3*p1 + c3*f1)*s2 +
		   (b3*h1 + d3*e1)*r2 + (b3*p1 + d3*f1)*q2) -
		  ((a3*h2 + c3*e2)*t1 + (a3*p2 + c3*f2)*s1 +
		   (b3*h2 + d3*e2)*r1 + (b3*p2 + d3*f2)*q1);

		xvolume = 64.0*(8.0*(A + B + D + E + J + K + M + N) +
				4.0*(C + F + G + H + L + O + P + Q + S + T +
				     V + W) + 2.0*(I + R + U + X + Y + Z) +
				AA)/27.0;

		//volume[index] = xvolume ;
		volume->setIJ(index,1,xvolume) ;
		index++;
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Volumes on it !"));
	  break;
	}
    }

  return Volume;
}

FIELD<double>* MESH::getArea(const SUPPORT * Support) const throw (MEDEXCEPTION)
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

//    if (onAll)
//      {
//        nb_type = myMesh->getNumberOfTypes(support_entity);
//        length_values = getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
//        types = getTypes(support_entity);
//      }
//    else
    {
      nb_type = Support->getNumberOfTypes();
      length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
      types = Support->getTypes();
    }

  int index;
  FIELD<double>* Area;

  Area = new FIELD<double>::FIELD(Support,1);
  Area->setName("AREA");
  Area->setDescription("cells or faces area");

  Area->setComponentName(1,"area");
  Area->setComponentDescription(1,"desc-comp");

  /*  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1];*/

  string MEDComponentUnit="trucmuch";

  Area->setMEDComponentUnit(1,MEDComponentUnit);

  Area->setValueType(MED_REEL64);

  Area->setIterationNumber(0);
  Area->setOrderNumber(0);
  Area->setTime(0.0);

  double *area = new double[length_values];
  //double *area = Area->getValue(MED_FULL_INTERLACE);

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      double xarea;

      if (onAll)
	{
	  nb_entity_type = getNumberOfElements(support_entity,type);
	  global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	}
      else
	{
	  // throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on all !"));

 	  nb_entity_type = Support->getNumberOfElements(type);
	  
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];

		if (dim_space==2)
		  {
		    xarea = - ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1))/2.0;
		  }
		else
		  {
		    double z1 = coord[dim_space*N1+2];
		    double z2 = coord[dim_space*N2+2];
		    double z3 = coord[dim_space*N3+2];

		    xarea = sqrt(((y2-y1)*(z3-z1) - (y3-y1)*(z2-z1))*
				 ((y2-y1)*(z3-z1) - (y3-y1)*(z2-z1)) +
				 ((x3-x1)*(z2-z1) - (x2-x1)*(z3-z1))*
				 ((x3-x1)*(z2-z1) - (x2-x1)*(z3-z1)) +
				 ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1))*
				 ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1)))/2.0;
		  }

		area[index] = xarea ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];

		if (dim_space==2)
		  {
		    double a1 = (x2-x1)/4.0, a2 = (y2-y1)/4.0;
		    double b1 = (x3-x4)/4.0, b2 = (y3-y4)/4.0;
		    double c1 = (x3-x2)/4.0, c2 = (y3-y2)/4.0;
		    double d1 = (x4-x1)/4.0, d2 = (y4-y1)/4.0;

		    xarea = - 4.0*(b1*c2 - c1*b2 + a1*c2 - c1*a2 + b1*d2 -
				 d1*b2 + a1*d2 - d1*a2);
		  }
		else
		  {
		    double z1 = coord[dim_space*N1+2];
		    double z2 = coord[dim_space*N2+2];
		    double z3 = coord[dim_space*N3+2];
		    double z4 = coord[dim_space*N4+2];

		    xarea = (sqrt(((y2-y1)*(z4-z1) - (y4-y1)*(z2-z1))*
				  ((y2-y1)*(z4-z1) - (y4-y1)*(z2-z1)) +
				  ((x4-x1)*(z2-z1) - (x2-x1)*(z4-z1))*
				  ((x4-x1)*(z2-z1) - (x2-x1)*(z4-z1)) +
				  ((x2-x1)*(y4-y1) - (x4-x1)*(y2-y1))*
				  ((x2-x1)*(y4-y1) - (x4-x1)*(y2-y1))) +
			     sqrt(((y4-y3)*(z2-z3) - (y2-y3)*(z4-z3))*
				  ((y4-y3)*(z2-z3) - (y2-y3)*(z4-z3)) +
				  ((x2-x3)*(z4-z3) - (x4-x3)*(z2-z3))*
				  ((x2-x3)*(z4-z3) - (x4-x3)*(z2-z3)) +
				  ((x4-x3)*(y2-y3) - (x2-x3)*(y4-y3))*
				  ((x4-x3)*(y2-y3) - (x2-x3)*(y4-y3))))/2.0;
		  }

		area[index] = xarea ;
		index++;
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Areas on it !"));
	  break;
	}
    }

  Area->setValue(MED_FULL_INTERLACE,area);
  delete[] area;
  return Area;
}

FIELD<double>* MESH::getLength(const SUPPORT * Support) const throw (MEDEXCEPTION)
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

//    if (onAll)
//      {
//        nb_type = myMesh->getNumberOfTypes(support_entity);
//        length_values = getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
//        types = getTypes(support_entity);
//      }
//    else
    {
      nb_type = Support->getNumberOfTypes();
      length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
      types = Support->getTypes();
    }

  int index;
  FIELD<double>* Length;

  Length = new FIELD<double>::FIELD(Support,1);
  //  double *length = new double [length_values];
  Length->setValueType(MED_REEL64);

  //const double *length = Length->getValue(MED_FULL_INTERLACE);
  MEDARRAY<double> * length = Length->getvalue();

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
	  //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on all !"));

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
    }

  return Length;
}

FIELD<double>* MESH::getNormal(const SUPPORT * Support) const throw (MEDEXCEPTION)
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
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type, length_values;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;

//    if (onAll)
//      {
//        nb_type = myMesh->getNumberOfTypes(support_entity);
//        length_values = getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
//        types = getTypes(support_entity);
//      }
//    else
    {
      nb_type = Support->getNumberOfTypes();
      length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
      types = Support->getTypes();
    }

  int index;

  FIELD<double>* Normal = new FIELD<double>::FIELD(Support,dim_space);
  Normal->setName("NORMAL");
  Normal->setDescription("cells or faces normal");
  for (int k=1;k<=dim_space;k++) {
    Normal->setComponentName(k,"normal");
    Normal->setComponentDescription(k,"desc-comp");
    Normal->setMEDComponentUnit(k,"unit");
  }

  Normal->setValueType(MED_REEL64);

  Normal->setIterationNumber(MED_NOPDT);
  Normal->setOrderNumber(MED_NONOR);
  Normal->setTime(0.0);

  double * normal = new double [dim_space*length_values];
  //double *normal = Normal->getValue(MED_FULL_INTERLACE);

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;

      // Make sure we trying to get Normals on
      // TRIA3 or TRIA6 or QUAD4 QUAD8 (2D) cells on a 3D mesh
      // or on SEG2 or SEG3 (1D) cells on a 2D mesh

      if ( (((type==MED_TRIA3) || (type==MED_TRIA6) ||
	     (type==MED_QUAD4) || (type==MED_QUAD8)) &&
      	    (dim_space != 3)) || (((type==MED_SEG2) || (type==MED_SEG3)) &&
				  (dim_space != 2)) )
	throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh : dimension problem !"));

      double xnormal1, xnormal2, xnormal3 ;

      if (onAll)
	{
	  nb_entity_type = getNumberOfElements(support_entity,type);
	  global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	}
      else
	{
	  // throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on all for instance !"));
 	  nb_entity_type = Support->getNumberOfElements(type);
	  
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

		double xarea;
		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];

		xnormal1 = ((y2-y1)*(z3-z1) - (y3-y1)*(z2-z1))/2.0;
		xnormal2 = ((x3-x1)*(z2-z1) - (x2-x1)*(z3-z1))/2.0;
		xnormal3 = ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1))/2.0;

		normal[3*index] = xnormal1 ;
		normal[3*index+1] = xnormal2 ;
		normal[3*index+2] = xnormal3 ;

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

		double xarea;
		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];

		xnormal1 = (y2-y1)*(z4-z1) - (y4-y1)*(z2-z1);
		xnormal2 = (x4-x1)*(z2-z1) - (x2-x1)*(z4-z1);
		xnormal3 = (x2-x1)*(y4-y1) - (x4-x1)*(y2-y1);
		xarea = sqrt(xnormal1*xnormal1 + xnormal2*xnormal2 +
			     xnormal3*xnormal3);

		xnormal1 = xnormal1/xarea;
		xnormal2 = xnormal2/xarea;
		xnormal3 = xnormal3/xarea;

		xarea = (sqrt(((y2-y1)*(z4-z1) - (y4-y1)*(z2-z1))*
			      ((y2-y1)*(z4-z1) - (y4-y1)*(z2-z1)) +
			      ((x4-x1)*(z2-z1) - (x2-x1)*(z4-z1))*
			      ((x4-x1)*(z2-z1) - (x2-x1)*(z4-z1)) +
			      ((x2-x1)*(y4-y1) - (x4-x1)*(y2-y1))*
			      ((x2-x1)*(y4-y1) - (x4-x1)*(y2-y1))) +
			 sqrt(((y3-y2)*(z4-z3) - (y4-y3)*(z3-z2))*
			      ((y3-y2)*(z4-z3) - (y4-y3)*(z3-z2)) +
			      ((x4-x3)*(z2-z2) - (x3-x2)*(z4-z3))*
			      ((x4-x3)*(z4-z2) - (x3-x2)*(z4-z3)) +
			      ((x3-x2)*(y4-y3) - (x4-x3)*(y3-y2))*
			      ((x3-x2)*(y4-y3) - (x4-x3)*(y3-y2))))/2.0;

		xnormal1 = xnormal1*xarea;
		xnormal2 = xnormal2*xarea;
		xnormal3 = xnormal3*xarea;

		normal[3*index] = xnormal1 ;
		normal[3*index+1] = xnormal2 ;
		normal[3*index+2] = xnormal3 ;

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
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Normals on it !"));
	  break;
	}
    }

  Normal->setValue(MED_FULL_INTERLACE,normal);
  delete[] normal ;

  END_OF(LOC);

  return Normal;
}

FIELD<double>* MESH::getBarycenter(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getBarycenter(SUPPORT*) : ";
  BEGIN_OF(LOC);

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

//    if (onAll)
//      {
//        nb_type = myMesh->getNumberOfTypes(support_entity);
//        length_values = getNumberOfElements(support_entity,MED_ALL_ELEMENTS);
//        types = getTypes(support_entity);
//      }
//    else
    {
      nb_type = Support->getNumberOfTypes();
      length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
      types = Support->getTypes();
    }

  int index;
  FIELD<double>* Barycenter;

  Barycenter = new FIELD<double>::FIELD(Support,dim_space);
  Barycenter->setName("BARYCENTER");
  Barycenter->setDescription("cells or faces barycenter");

  for (int k=0;k<dim_space;k++) {
    int kp1 = k + 1;
    Barycenter->setComponentName(kp1,myMesh->getCoordinatesNames()[k]);
    Barycenter->setComponentDescription(kp1,"desc-comp");
    Barycenter->setMEDComponentUnit(kp1,myMesh->getCoordinatesUnits()[k]);
  }

  Barycenter->setValueType(MED_REEL64);

  Barycenter->setIterationNumber(0);
  Barycenter->setOrderNumber(0);
  Barycenter->setTime(0.0);

  double *barycenter = new double [dim_space*length_values];
  //  double *barycenter = Barycenter->getValue(MED_FULL_INTERLACE);

  const double * coord = getCoordinates(MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
    {
      medGeometryElement type = types[i] ;
      double xbarycenter1, xbarycenter2, xbarycenter3;

      if (onAll)
	{
	  nb_entity_type = getNumberOfElements(support_entity,type);
	  global_connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,type);
	}
      else
	{
	  // throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on all !"));
 	  nb_entity_type = Support->getNumberOfElements(type);
	  
 	  const int * supp_number = Support->getNumber(type);
 	  const int * connectivity = getConnectivity(MED_FULL_INTERLACE,MED_NODAL,support_entity,MED_ALL_ELEMENTS);
 	  const int * connectivityIndex = getConnectivityIndex(MED_NODAL,support_entity);
 	  int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];
	  
 	  for (int k_type = 0; k_type<nb_entity_type; k_type++) {
 	    for (int j_ent = 0; j_ent<(type%100); j_ent++) {
 	      global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
 	    }
 	  }
	  global_connectivity = global_connectivity_tmp;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];

		xbarycenter1 = (x1 + x2 + x3 + x4)/4.0;
		xbarycenter2 = (y1 + y2 + y3 + y4)/4.0;
		xbarycenter3 = (z1 + z2 + z3 + z4)/4.0;
		barycenter[3*index] = xbarycenter1 ;
		barycenter[3*index+1] = xbarycenter2 ;
		barycenter[3*index+2] = xbarycenter3 ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];

		xbarycenter1 = (x1 + x2 + x3 + x4 + x5)/5.0;
		xbarycenter2 = (y1 + y2 + y3 + y4 + y5)/5.0;
		xbarycenter3 = (z1 + z2 + z3 + z4 + z5)/5.0;
		barycenter[3*index] = xbarycenter1 ;
		barycenter[3*index+1] = xbarycenter2 ;
		barycenter[3*index+2] = xbarycenter3 ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];
		double x6 = coord[dim_space*N6];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];
		double y6 = coord[dim_space*N6+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];
		double z6 = coord[dim_space*N6+2];

		xbarycenter1 = (x1 + x2 + x3 + x4 + x5 + x6)/6.0;
		xbarycenter2 = (y1 + y2 + y3 + y4 + y5 + y6)/6.0;
		xbarycenter3 = (z1 + z2 + z3 + z4 + z5 + z6)/6.0;
		barycenter[3*index] = xbarycenter1 ;
		barycenter[3*index+1] = xbarycenter2 ;
		barycenter[3*index+2] = xbarycenter3 ;
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];
		double x5 = coord[dim_space*N5];
		double x6 = coord[dim_space*N6];
		double x7 = coord[dim_space*N7];
		double x8 = coord[dim_space*N8];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];
		double y5 = coord[dim_space*N5+1];
		double y6 = coord[dim_space*N6+1];
		double y7 = coord[dim_space*N7+1];
		double y8 = coord[dim_space*N8+1];

		double z1 = coord[dim_space*N1+2];
		double z2 = coord[dim_space*N2+2];
		double z3 = coord[dim_space*N3+2];
		double z4 = coord[dim_space*N4+2];
		double z5 = coord[dim_space*N5+2];
		double z6 = coord[dim_space*N6+2];
		double z7 = coord[dim_space*N7+2];
		double z8 = coord[dim_space*N8+2];

		xbarycenter1 = (x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8)/8.0;
		xbarycenter2 = (y1 + y2 + y3 + y4 + y5 + y6 + y7 + y8)/8.0;
		xbarycenter3 = (z1 + z2 + z3 + z4 + z5 + z6 + z7 + z8)/8.0;

		barycenter[3*index] = xbarycenter1 ;
		barycenter[3*index+1] = xbarycenter2 ;
		barycenter[3*index+2] = xbarycenter3 ;

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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];

		xbarycenter1 = (x1 + x2 + x3)/3.0;
		xbarycenter2 = (y1 + y2 + y3)/3.0;

		if (dim_space==2)
		  {
		    barycenter[2*index] = xbarycenter1 ;
		    barycenter[2*index+1] = xbarycenter2 ;
		  }
		else
		  {
		    double z1 =
		      coord[dim_space*N1+2];
		    double z2 =
		      coord[dim_space*N2+2];
		    double z3 =
		      coord[dim_space*N3+2];

		    xbarycenter3 = (z1 + z2 + z3)/3.0;

		    barycenter[3*index] = xbarycenter1 ;
		    barycenter[3*index+1] = xbarycenter2 ;
		    barycenter[3*index+2] = xbarycenter3 ;
		  }

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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];
		double x3 = coord[dim_space*N3];
		double x4 = coord[dim_space*N4];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];
		double y3 = coord[dim_space*N3+1];
		double y4 = coord[dim_space*N4+1];

		xbarycenter1 = (x1 + x2 + x3 + x4)/4.0;
		xbarycenter2 = (y1 + y2 + y3 + y4)/4.0;

		if (dim_space==2)
		  {
		    barycenter[2*index] = xbarycenter1 ;
		    barycenter[2*index+1] = xbarycenter2 ;
		  }
		else
		  {
		    double z1 = coord[dim_space*N1+2];
		    double z2 = coord[dim_space*N2+2];
		    double z3 = coord[dim_space*N3+2];
		    double z4 = coord[dim_space*N4+2];

		    xbarycenter3 = (z1 + z2 + z3 + z4)/4.0;

		    barycenter[3*index] = xbarycenter1 ;
		    barycenter[3*index+1] = xbarycenter2 ;
		    barycenter[3*index+2] = xbarycenter3 ;
		  }
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

		double x1 = coord[dim_space*N1];
		double x2 = coord[dim_space*N2];

		double y1 = coord[dim_space*N1+1];
		double y2 = coord[dim_space*N2+1];

		xbarycenter1 = (x1 + x2)/2.0;
		xbarycenter2 = (y1 + y2)/2.0;

		if (dim_space==2)
		  {
		    barycenter[2*index] = xbarycenter1 ;
		    barycenter[2*index+1] = xbarycenter2 ;
		  }
		else
		  {
		    double z1 = coord[dim_space*N1+2];
		    double z2 = coord[dim_space*N2+2];

		    xbarycenter3 = (z1 + z2)/2.0;

		    barycenter[3*index] = xbarycenter1 ;
		    barycenter[3*index+1] = xbarycenter2 ;
		    barycenter[3*index+2] = xbarycenter3 ;
		  }
		index++;
	      }
	    break;
	  }
	default :
	  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get a barycenter on it (in fact unknown type) !"));
	  break;
	}
    }

  Barycenter->setValue(MED_FULL_INTERLACE,barycenter);

  delete[] barycenter ;

  END_OF(LOC);

  return Barycenter;
}

//=======================================================================
//function : checkGridFillCoords
//purpose  : if this->_isAGrid, assure that _coordinate is filled
//=======================================================================

inline void MESH::checkGridFillCoords() const
{
  if (_isAGrid)
    ((GRID *) this)->fillCoordinates();
}

//=======================================================================
//function : checkGridFillConnectivity
//purpose  : if this->_isAGrid, assure that _connectivity is filled
//=======================================================================

inline void MESH::checkGridFillConnectivity() const
{
  if (_isAGrid)
    ((GRID *) this)->fillConnectivity();
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
  if (_isAGrid)
    ((GRID *) this)->fillMeshAfterRead();

  END_OF(LOC);
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
  int * numberOfGaussPoint ;
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
    numberOfGaussPoint = new int[1] ;
    numberOfGaussPoint[0] = 1 ;
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
    const medGeometryElement *  allType = getTypes(MED_FACE);
    numberOfGaussPoint = new int[numberOfGeometricType] ;
    geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
    numberOfEntities = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      numberOfGaussPoint[index] = 1 ;
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
  mySupport->setNumberOfGaussPoint(numberOfGaussPoint) ;
  //  mySupport->setGeometricTypeNumber(geometricTypeNumber) ;
  mySupport->setNumberOfElements(numberOfEntities) ;
  mySupport->setTotalNumberOfElements(size) ;
  mySupport->setNumber(mySkyLineArray) ;

  delete[] numberOfEntities;
  delete[] geometricTypeNumber;
  delete[] numberOfGaussPoint;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;
//   delete mySkyLineArray;

  END_OF(LOC) ;
  return mySupport ;
 
}
