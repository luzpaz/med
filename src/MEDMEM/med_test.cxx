#include<string>

#include <math.h>
#include <stdlib.h>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_define.hxx"


double dmax(double x, double y) { return (x>y)?x:y;}

double dmin(double x, double y) { return (x>y)?y:x;}

double infty = 1.e20;

void affiche_support(SUPPORT * mySupport) 
{
  MESSAGE( "  - Name : "<<mySupport->getName().c_str());
  MESSAGE( "  - Description : "<<mySupport->getDescription().c_str());
  MESSAGE( "  - Entity : "<<mySupport->getEntity());
  MESSAGE( "  - Entities list : ");
  if (!(mySupport->isOnAllElements())) {
    int NumberOfTypes = mySupport->getNumberOfTypes() ;
    MESSAGE("  - NumberOfTypes : "<<NumberOfTypes);
    medGeometryElement * Types = mySupport->getTypes() ;
    for (int j=0;j<NumberOfTypes;j++) {
      MESSAGE( "    * Type "<<Types[j]<<" : " );
      int NumberOfElements = mySupport->getNumberOfElements(Types[j]) ;
      int * Number = mySupport->getNumber(Types[j]) ;
      for (int k=0; k<NumberOfElements;k++)
	MESSAGE( Number[k] << " ");
      MESSAGE("");
    }
  } else
    MESSAGE( "    Is on all entities !");
}


void affiche_famille(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfFamilies = myMesh->getNumberOfFamilies(Entity) ;
  MESSAGE( "NumberOfFamilies : "<<NumberOfFamilies);
  for (int i=1; i<NumberOfFamilies+1;i++) {
    FAMILY* myFamily = myMesh->getFamily(Entity,i);
    affiche_support(myFamily);
    MESSAGE( "  - Identifier : "<<myFamily->getIdentifier());
    int NumberOfAttributes = myFamily->getNumberOfAttributes() ;
    MESSAGE( "  - Attributes ("<<NumberOfAttributes<<") :");
    for (int j=1;j<NumberOfAttributes+1;j++)
      MESSAGE( "    * "<<myFamily->getAttributeIdentifier(j)<<" : "<<myFamily->getAttributeValue(j)<<", "<<myFamily->getAttributeDescription(j).c_str());
    int NumberOfGroups = myFamily->getNumberOfGroups() ;
    MESSAGE( "  - Groups ("<<NumberOfGroups<<") :");
    for (int j=1;j<NumberOfGroups+1;j++)
      MESSAGE( "    * "<<myFamily->getGroupName(j).c_str());
  }
}

void affiche_groupe(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfGroups = myMesh->getNumberOfGroups(Entity) ;
  MESSAGE( "NumberOfGroups : "<<NumberOfGroups);
  for (int i=1; i<NumberOfGroups+1;i++) {
    GROUP* myGroup = myMesh->getGroup(Entity,i);
    affiche_support(myGroup);
    int NumberOfFamillies = myGroup->getNumberOfFamilies() ;
    MESSAGE( "  - Families ("<<NumberOfFamillies<<") :");
    for (int j=1;j<NumberOfFamillies+1;j++)
      MESSAGE( "    * "<<myGroup->getFamily(j)->getName().c_str());
  }
}

int main (int argc, char ** argv) {

  int read;

  if ((argc !=3) && (argc != 4)) {
    cerr << "Usage : " << argv[0] 
	 << " filename meshname [fieldname]" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  //  MESH * myMesh= new MESH(MED_DRIVER,filename,meshname) ;
  MESH * myMesh= new MESH() ;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ;
  myMeshDriver.close() ;

  //    int drv = myMesh->addDriver(MED_DRIVER,"sortie.med",meshname);
  //    myMesh->write(drv); 


  int SpaceDimension = myMesh->getSpaceDimension() ;
  int MeshDimension  = myMesh->getMeshDimension() ;
  int NumberOfNodes  = myMesh->getNumberOfNodes() ;

  MESSAGE( "Space Dimension : " << SpaceDimension << endl ); 

  MESSAGE( "Mesh Dimension : " << MeshDimension << endl ); 

  const double * Coordinates = myMesh->getCoordinates(MED_FULL_INTERLACE) ;

  MESSAGE( "Show Nodes Coordinates : " );

  MESSAGE( "Name :" );
  string * CoordinatesNames = myMesh->getCoordinatesNames() ;
  for(int i=0; i<SpaceDimension ; i++) {
    MESSAGE( " - " << CoordinatesNames[i] );
  }
  MESSAGE( "Unit :" );
  string * CoordinatesUnits = myMesh->getCoordinatesUnits() ;
  for(int i=0; i<SpaceDimension ; i++) {
    MESSAGE( " - " << CoordinatesUnits[i] );
  }
  for(int i=0; i<NumberOfNodes ; i++) {
    MESSAGE( "Nodes " << i+1 << " : " );
    for (int j=0; j<SpaceDimension ; j++)
      MESSAGE( Coordinates[i*SpaceDimension+j] << " " );
    MESSAGE("");
  }

  int NumberOfTypes = myMesh->getNumberOfTypes(MED_CELL) ;
  medGeometryElement  * Types = myMesh->getTypes(MED_CELL) ;

  MESSAGE( "Show Connectivity (Nodal) :" );
  for (int i=0; i<NumberOfTypes; i++) {
    MESSAGE( "For type " << Types[i] << " : " );
    int NumberOfElements = myMesh->getNumberOfElements(MED_CELL,Types[i]);
    int * connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,Types[i]);
    int NomberOfNodesPerCell = Types[i]%100 ;
    for (int j=0;j<NumberOfElements;j++){
      MESSAGE( "Element "<< j+1 <<" : " );
      for (int k=0;k<NomberOfNodesPerCell;k++)
	MESSAGE( connectivity[j*NomberOfNodesPerCell+k]<<" ");
      MESSAGE("");
    }
  }

  MESSAGE( "Show Family :");
  affiche_famille(myMesh,MED_NODE);
  affiche_famille(myMesh,MED_CELL);
  affiche_famille(myMesh,MED_FACE);
  affiche_famille(myMesh,MED_EDGE);

  MESSAGE( "Show Group :");
  affiche_groupe(myMesh,MED_NODE);
  affiche_groupe(myMesh,MED_CELL);
  affiche_groupe(myMesh,MED_FACE);
  affiche_groupe(myMesh,MED_EDGE);

  MESSAGE( "Show Reverse Nodal Connectivity :" );
  int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
  int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    MESSAGE( "Node "<<i+1<<" : " );
    for (int j=ReverseNodalConnectivityIndex[i];j<ReverseNodalConnectivityIndex[i+1];j++)
      MESSAGE( ReverseNodalConnectivity[j-1] << " " );
    MESSAGE("");
  }

  MESSAGE( "Show Connectivity (Descending) :" );
  int NumberOfElements ;
  int * connectivity ;
  int * connectivity_index ;
  myMesh->calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL);
  try {
    NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
    connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
    connectivity_index =  myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  }
  catch (MEDEXCEPTION m) {
    MESSAGE( m.what() );
    exit (-1) ;
  }
  for (int j=0;j<NumberOfElements;j++) {
    MESSAGE( "Element "<<j+1<<" : " );
    for (int k=connectivity_index[j];k<connectivity_index[j+1];k++)
      MESSAGE( connectivity[k-1]<<" ");
    MESSAGE("");
  }

  MESSAGE( "Show Reverse Descending Connectivity :" );
  int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
  int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;

  int NumberOfConstituents  = 0;
  string constituent ;
  medEntityMesh constituentEntity ;

  if (MeshDimension==3) {
    constituent = "Face" ;
    constituentEntity = MED_FACE ;
  }

  if (MeshDimension==2) {
    constituent = "Edge" ;
    constituentEntity = MED_EDGE ;
  }

  if (MeshDimension==1) {
    MESSAGE("ERROR : MeshDimension = 1 !");
    MESSAGE("We could not see Reverse Descending Connectivity.") ;
  } else {
    NumberOfConstituents = myMesh->getNumberOfElements (constituentEntity,MED_ALL_ELEMENTS);
    for (int i=0; i<NumberOfConstituents; i++) {
      MESSAGE( constituent <<i+1<<" : " );
      for (int j=ReverseDescendingConnectivityIndex[i];j<ReverseDescendingConnectivityIndex[i+1];j++)
	MESSAGE( ReverseDescendingConnectivity[j-1] << " " );
      MESSAGE("");
    }
  }
  MESSAGE( "Show "<<constituent<<" Connectivity (Nodal) :" );
  int * face_connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
  int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    MESSAGE( constituent <<i+1<<" : " );
    for (int j=face_connectivity_index[i];j<face_connectivity_index[i+1];j++)
      MESSAGE( face_connectivity[j-1]<<" ");
    MESSAGE("");
  }

  /* test of normal, area, volume, barycenter */

  SUPPORT * support1 = (SUPPORT*) NULL;
  
  FIELD<double>* normal = new FIELD<double>::FIELD();
  FIELD<double>* length = new FIELD<double>::FIELD();
  normal = NULL;
  length = NULL;
  string support_name = "Support on all " ;
  support_name+=constituent;
  support1 = new SUPPORT(myMesh,support_name,constituentEntity);
  MESSAGE( "Building of the Support on all cells dimensionned (Meshdim-1) of the mesh :");
  MESSAGE( "Face in 3D or Edge in 2D" );
  
  MESSAGE( "Getting the normal of each face of this support !" );
  
  normal = myMesh->getNormal(support1);
  
  double normal_square, norm ;
  double maxnorm=-infty;
  double minnorm=infty;
  double tmp_value ;
  for (int i = 1; i<=NumberOfConstituents;i++) {
    normal_square = 0. ;
    MESSAGE( "Normal " << i << " " ); 
    for (int j=1; j<=SpaceDimension; j++) {
      tmp_value = normal->getValueIJ(i,j) ;
      normal_square += tmp_value*tmp_value ;
      MESSAGE( tmp_value << " " );
    }
    norm = sqrt(normal_square);
    maxnorm = dmax(maxnorm,norm);
    minnorm = dmin(minnorm,norm);
    MESSAGE( ", Norm = " << norm );
  }
  MESSAGE( "Max Norm " << maxnorm << " Min Norm " << minnorm );
  

  if (SpaceDimension == 2)
    {
      MESSAGE( "Getting the length of each edge !" );

      length = myMesh->getLength(support1);

      double length_value,maxlength,minlength;
      maxlength = -infty;
      minlength = infty;
      for (int i = 1; i<=NumberOfConstituents;i++)
	{
	  length_value = length->getValueIJ(i,1) ;
	  MESSAGE( "Length " << i << " " << length_value );
	  maxlength = dmax(maxlength,length_value);
	  minlength = dmin(minlength,length_value);
	}
      MESSAGE( "Max Length " << maxlength << " Min Length " << minlength );
    }

  MESSAGE( "Building of the Support on all space-dimensionned cells of the mesh :");
  SUPPORT * support = new SUPPORT(myMesh);

  MESSAGE( "Getting the barycenter of each element of this support !" );

  FIELD<double>* barycenter = new FIELD<double>::FIELD();

  barycenter = myMesh->getBarycenter(support);
  NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);

  for (int i = 1; i<=NumberOfElements;i++)
    {
      if (SpaceDimension == 3)
	MESSAGE( "Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << " " << barycenter->getValueIJ(i,3) );

      if (SpaceDimension == 2)
	MESSAGE( "Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) );
    }

  FIELD<double>* volume = new FIELD<double>::FIELD();
  FIELD<double>* area = new FIELD<double>::FIELD();
  volume = NULL;
  area = NULL;

  if (SpaceDimension == 3)
    {
      MESSAGE( "Getting the Volume of each element of this support which is a 3D one !" );

      volume = myMesh->getVolume(support);

      double maxvol,minvol,voltot;
      maxvol = -infty;
      minvol = infty;
      voltot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
	{
	  MESSAGE( "Volume " << i << " " << volume->getValueIJ(i,1) );
	  maxvol = dmax(maxvol,volume->getValueIJ(i,1));
	  minvol = dmin(minvol,volume->getValueIJ(i,1));
	  voltot = voltot + volume->getValueIJ(i,1);
	}

      MESSAGE( "Max Volume " << maxvol << " Min Volume " << minvol );
      MESSAGE( "Support Volume " << voltot );
    }
  else if (SpaceDimension == 2)
    {
      MESSAGE( "Getting the Area of each element of this support which is a 2D one !" );

      area = myMesh->getArea(support);

      //    MESSAGE( "nb of comp "<< area->getNumberOfComponents() << " length " << area->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) );

      double maxarea,minarea,areatot;
      maxarea = -infty;
      minarea = infty;
      areatot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
	{
	  MESSAGE( "Area " << i << " " << area->getValueIJ(i,1) );
	  maxarea = dmax(maxarea,area->getValueIJ(i,1));
	  minarea = dmin(minarea,area->getValueIJ(i,1));
	  areatot = areatot + area->getValueIJ(i,1);
	}

      MESSAGE( "Max Area " << maxarea << " Min Area " << minarea );
      MESSAGE( "Support Area " << areatot );
    }

  if (barycenter != NULL) delete barycenter;
  if (volume != NULL ) delete volume;
  if (area != NULL ) delete area;


  if (argc < 4) return 0;

  // read field :

  if (argc != 4) exit(0) ;
  // else we have a field !

  string fieldname = argv[3];

  //  SUPPORT * mySupport = new SUPPORT(myMesh,"On_all_node",MED_NODE);
  SUPPORT * mySupport = new SUPPORT(myMesh,"On_all_cell",MED_CELL);
  FIELD<double> * myField= new FIELD<double>() ;

  myField->setName(fieldname);
  myField->setSupport(mySupport);
  MED_FIELD_RDONLY_DRIVER<double> myFieldDriver(filename,myField) ;
  myFieldDriver.setFieldName(fieldname);
  myFieldDriver.open() ;

  try {
    myFieldDriver.read() ;
  } catch (...) {
    delete mySupport ;
    mySupport = new SUPPORT(myMesh,"On_all_node",MED_NODE);
    myField->setSupport(mySupport);
    try {
      myFieldDriver.read() ;
    } catch (...) {
      cout << "Field " << fieldname << " not found !!!" << endl ;
      exit (-1) ;
    }
  }
  
  myFieldDriver.close() ;

  MESSAGE( "Field "<< myField->getName() << " : " <<myField->getDescription() );
  int NumberOfComponents = myField->getNumberOfComponents() ;
  MESSAGE( "- Nombre de composantes : "<< NumberOfComponents );
  for (int i=1; i<NumberOfComponents+1; i++) {
    MESSAGE( "  - composante "<<i<<" :");
    MESSAGE( "      - nom         : "<<myField->getComponentName(i));
    MESSAGE( "      - description : "<<myField->getComponentDescription(i) );
    MESSAGE( "      - units       : "<<myField->getMEDComponentUnit(i) );
  }
  MESSAGE( "- iteration :" );
  MESSAGE( "    - numero : " << myField->getIterationNumber());
  MESSAGE( "    - ordre  : " << myField->getOrderNumber());
  MESSAGE( "    - temps  : " << myField->getTime());

  MESSAGE( "- Valeurs :");
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  //    for (int i=1; i<NumberOfComponents+1; i++) {
  //      double * value = myField->getValueI(MED_NO_INTERLACE,i) ;
  //      for (int j=0; j<NumberOf; j++)
  //        MESSAGE( value[j]<< " ");
  //      MESSAGE();
  //    }
  for (int i=1; i<NumberOf+1; i++) {
    double * value = myField->getValueI(MED_FULL_INTERLACE,i) ;
    for (int j=0; j<NumberOfComponents; j++)
      MESSAGE( value[j]<< " ");
    MESSAGE("");
  }


  return 0;
}
