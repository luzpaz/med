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

using namespace std;
using namespace MEDMEM;

double dmax(double x, double y) { return (x>y)?x:y;}

double dmin(double x, double y) { return (x>y)?y:x;}

double infty = 1.e20;

void affiche_support(const SUPPORT * mySupport) 
{
  cout << "med_test   - Name : "<<mySupport->getName().c_str()<<endl ;
  cout << "med_test   - Description : "<<mySupport->getDescription().c_str()<<endl ;
  cout << "med_test   - Entity : "<<mySupport->getEntity()<<endl ;
  cout << "med_test   - Entities list : "<<endl ;
  if (!(mySupport->isOnAllElements())) {
    int NumberOfTypes = mySupport->getNumberOfTypes() ;
    cout<<"med_test   - NumberOfTypes : "<<NumberOfTypes<<endl;
    const medGeometryElement * Types = mySupport->getTypes() ;
    for (int j=0;j<NumberOfTypes;j++) {
      cout << "med_test     * Type "<<Types[j]<<" : " ;
      int NumberOfElements = mySupport->getNumberOfElements(Types[j]) ;
//CCRT      const int * Number = mySupport->getNumber(Types[j]) ;
      const med_int * Number = mySupport->getNumber(Types[j]) ;
      for (int k=0; k<NumberOfElements;k++)
	cout << Number[k] << " ";
      cout << endl ;
    }
  } else
    cout << "med_test     Is on all entities !"<< endl;
}


void affiche_famille(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfFamilies = myMesh->getNumberOfFamilies(Entity) ;
  cout << "med_test NumberOfFamilies : "<<NumberOfFamilies<<endl;
  for (int i=1; i<NumberOfFamilies+1;i++) {
    const FAMILY* myFamily = myMesh->getFamily(Entity,i);
    affiche_support(myFamily);
    cout << "med_test   - Identifier : "<<myFamily->getIdentifier()<<endl ;
    int NumberOfAttributes = myFamily->getNumberOfAttributes() ;
    cout << "med_test   - Attributes ("<<NumberOfAttributes<<") :"<<endl;
    for (int j=1;j<NumberOfAttributes+1;j++)
      cout << "med_test     * "<<myFamily->getAttributeIdentifier(j)<<" : "<<myFamily->getAttributeValue(j)<<", "<<myFamily->getAttributeDescription(j).c_str()<<endl ;
    int NumberOfGroups = myFamily->getNumberOfGroups() ;
    cout << "med_test   - Groups ("<<NumberOfGroups<<") :"<<endl;
    for (int j=1;j<NumberOfGroups+1;j++)
      cout << "med_test     * "<<myFamily->getGroupName(j).c_str()<<endl ;
  }
}

void affiche_groupe(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfGroups = myMesh->getNumberOfGroups(Entity) ;
  cout << "med_test NumberOfGroups : "<<NumberOfGroups<<endl;
  for (int i=1; i<NumberOfGroups+1;i++) {
    const GROUP* myGroup = myMesh->getGroup(Entity,i);
    affiche_support(myGroup);
    int NumberOfFamillies = myGroup->getNumberOfFamilies() ;
    cout << "med_test   - Families ("<<NumberOfFamillies<<") :"<<endl;
    for (int j=1;j<NumberOfFamillies+1;j++)
      cout << "med_test     * "<<myGroup->getFamily(j)->getName().c_str()<<endl ;
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

  cout << "med_test Space Dimension : " << SpaceDimension << endl << endl ; 

  cout << "med_test Mesh Dimension : " << MeshDimension << endl << endl ; 

  const double * Coordinates = myMesh->getCoordinates(MED_FULL_INTERLACE) ;

  cout << "med_test Show Nodes Coordinates : " << endl ;

  cout << "med_test Name :" << endl ;
  const string * CoordinatesNames = myMesh->getCoordinatesNames() ;
  for(int i=0; i<SpaceDimension ; i++) {
    cout << "med_test  - " << CoordinatesNames[i] << endl ;
  }
  cout << "med_test Unit :" << endl ;
  const string * CoordinatesUnits = myMesh->getCoordinatesUnits() ;
  for(int i=0; i<SpaceDimension ; i++) {
    cout << "med_test  - " << CoordinatesUnits[i] << endl ;
  }
  for(int i=0; i<NumberOfNodes ; i++) {
    cout << "med_test Nodes " << i+1 << " : " ;
    for (int j=0; j<SpaceDimension ; j++)
      cout << Coordinates[i*SpaceDimension+j] << " " ;
    cout << endl ;
  }

  int NumberOfTypes = myMesh->getNumberOfTypes(MED_CELL) ;
  const medGeometryElement  * Types = myMesh->getTypes(MED_CELL) ;

  cout << "med_test Show Connectivity (Nodal) :" << endl ;
  for (int i=0; i<NumberOfTypes; i++) {
    cout << "med_test For type " << Types[i] << " : " << endl ;
    int NumberOfElements = myMesh->getNumberOfElements(MED_CELL,Types[i]);
//CCRT    const int * connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,Types[i]);
    const med_int * connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,Types[i]);
    int NomberOfNodesPerCell = Types[i]%100 ;
    for (int j=0;j<NumberOfElements;j++){
      cout << "med_test Element "<< j+1 <<" : " ;
      for (int k=0;k<NomberOfNodesPerCell;k++)
	cout << connectivity[j*NomberOfNodesPerCell+k]<<" ";
      cout << endl ;
    }
  }

  cout << "med_test Show Family :"<<endl ;
  affiche_famille(myMesh,MED_NODE);
  affiche_famille(myMesh,MED_CELL);
  affiche_famille(myMesh,MED_FACE);
  affiche_famille(myMesh,MED_EDGE);

  cout << "med_test Show Group :"<<endl ;
  affiche_groupe(myMesh,MED_NODE);
  affiche_groupe(myMesh,MED_CELL);
  affiche_groupe(myMesh,MED_FACE);
  affiche_groupe(myMesh,MED_EDGE);

  cout << "med_test Show Reverse Nodal Connectivity :" << endl ;
//CCRT  const int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
  const med_int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
//CCRT  const int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  const med_int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    cout << "med_test Node "<<i+1<<" : " ;
    for (int j=ReverseNodalConnectivityIndex[i];j<ReverseNodalConnectivityIndex[i+1];j++)
      cout << ReverseNodalConnectivity[j-1] << " " ;
    cout << endl ;
  }

  cout << "med_test Show Connectivity (Descending) :" << endl ;
  int NumberOfElements ;
//CCRT  const int * connectivity ;
  const med_int * connectivity ;
//CCRT  const int * connectivity_index ;
  const med_int * connectivity_index ;
  myMesh->calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL);
  try {
    NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
    connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
    connectivity_index =  myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  }
  catch (MEDEXCEPTION m) {
    cout << "med_test " << m.what() << endl ;
    exit (-1) ;
  }
  for (int j=0;j<NumberOfElements;j++) {
    cout << "med_test Element "<<j+1<<" : " ;
    for (int k=connectivity_index[j];k<connectivity_index[j+1];k++)
      cout << connectivity[k-1]<<" ";
    cout << endl ;
  }

  cout << "med_test Show Reverse Descending Connectivity :" << endl ;
//CCRT  const int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
  const med_int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
//CCRT  const int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;
  const med_int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;

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
    INFOS("ERROR : MeshDimension = 1 !");
    INFOS("We could not see Reverse Descending Connectivity.") ;
  } else {
    NumberOfConstituents = myMesh->getNumberOfElements (constituentEntity,MED_ALL_ELEMENTS);
    for (int i=0; i<NumberOfConstituents; i++) {
      cout << "med_test " << constituent <<i+1<<" : " ;
      for (int j=ReverseDescendingConnectivityIndex[i];j<ReverseDescendingConnectivityIndex[i+1];j++)
	cout << ReverseDescendingConnectivity[j-1] << " " ;
      cout << endl ;
    }
  }
  cout << "med_test Show "<<constituent<<" Connectivity (Nodal) :" << endl ;
//CCRT  const int * face_connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
  const med_int * face_connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
//CCRT  const int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  const med_int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    cout << "med_test " << constituent <<i+1<<" : " ;
    for (int j=face_connectivity_index[i];j<face_connectivity_index[i+1];j++)
      cout << face_connectivity[j-1]<<" ";
    cout << endl ;
  }

  /* test of normal, area, volume, barycenter */

  SUPPORT * support1 = (SUPPORT*) NULL;
  
  //FIELD<double>* normal = new FIELD<double>::FIELD();
  //FIELD<double>* length = new FIELD<double>::FIELD();
  //normal = NULL;
  //length = NULL;
  string support_name = "Support on all " ;
  support_name+=constituent;
  support1 = new SUPPORT(myMesh,support_name,constituentEntity);
  cout << "med_test Building of the Support on all cells dimensionned (Meshdim-1) of the mesh :"<< endl ;
  cout << "med_test Face in 3D or Edge in 2D" << endl;
  
  cout << "med_test Getting the normal of each face of this support !" << endl ;
  
  FIELD<double>* normal = myMesh->getNormal(support1);
  
  double normal_square, norm ;
  double maxnorm=-infty;
  double minnorm=infty;
  double tmp_value ;
  for (int i = 1; i<=NumberOfConstituents;i++) {
    normal_square = 0. ;
    cout << "med_test Normal " << i << " " ; 
    for (int j=1; j<=SpaceDimension; j++) {
      tmp_value = normal->getValueIJ(i,j) ;
      normal_square += tmp_value*tmp_value ;
      cout << tmp_value << " " ;
    }
    norm = sqrt(normal_square);
    maxnorm = dmax(maxnorm,norm);
    minnorm = dmin(minnorm,norm);
    cout << ", Norm = " << norm << endl;
  }
  cout << "med_test Max Norm " << maxnorm << " Min Norm " << minnorm << endl;
  
  delete normal ;

  if (SpaceDimension == 2)
    {
      cout << "med_test Getting the length of each edge !" << endl ;

      FIELD<double>* length = myMesh->getLength(support1);

      double length_value,maxlength,minlength;
      maxlength = -infty;
      minlength = infty;
      for (int i = 1; i<=NumberOfConstituents;i++)
	{
	  length_value = length->getValueIJ(i,1) ;
	  cout << "med_test Length " << i << " " << length_value << endl;
	  maxlength = dmax(maxlength,length_value);
	  minlength = dmin(minlength,length_value);
	}
      cout << "med_test Max Length " << maxlength << " Min Length " << minlength << endl;

      delete length ;
    }

  delete support1 ;

  cout << "med_test Building of the Support on all space-dimensionned cells of the mesh :"<< endl ;
  SUPPORT * support = new SUPPORT(myMesh);

  cout << "med_test Getting the barycenter of each element of this support !" << endl ;

  //FIELD<double>* barycenter = new FIELD<double>::FIELD();

  FIELD<double>* barycenter = myMesh->getBarycenter(support);
  NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);

  for (int i = 1; i<=NumberOfElements;i++)
    {
      if (SpaceDimension == 3) {
	cout << "med_test Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << " " << barycenter->getValueIJ(i,3) << endl;
      }

      if (SpaceDimension == 2) {
	cout << "med_test Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << endl;
      }
    }

  delete barycenter ;

  //FIELD<double>* volume = new FIELD<double>::FIELD();
  //FIELD<double>* area = new FIELD<double>::FIELD();
  //volume = NULL;
  //area = NULL;

  if (SpaceDimension == 3)
    {
      cout << "med_test Getting the Volume of each element of this support which is a 3D one !" << endl;

      FIELD<double>* volume = myMesh->getVolume(support);

      double maxvol,minvol,voltot;
      maxvol = -infty;
      minvol = infty;
      voltot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
	{
	  cout << "med_test Volume " << i << " " << volume->getValueIJ(i,1) << endl;
	  maxvol = dmax(maxvol,volume->getValueIJ(i,1));
	  minvol = dmin(minvol,volume->getValueIJ(i,1));
	  voltot = voltot + volume->getValueIJ(i,1);
	}

      cout << "med_test Max Volume " << maxvol << " Min Volume " << minvol << endl;
      cout << "med_test Support Volume " << voltot << endl;

      delete volume ;
    }
  else if (SpaceDimension == 2)
    {
      cout << "med_test Getting the Area of each element of this support which is a 2D one !" << endl;

      FIELD<double>* area = myMesh->getArea(support);

      //    cout << "nb of comp "<< area->getNumberOfComponents() << " length " << area->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) << endl;

      double maxarea,minarea,areatot;
      maxarea = -infty;
      minarea = infty;
      areatot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
	{
	  cout << "med_test Area " << i << " " << area->getValueIJ(i,1) << endl;
	  maxarea = dmax(maxarea,area->getValueIJ(i,1));
	  minarea = dmin(minarea,area->getValueIJ(i,1));
	  areatot = areatot + area->getValueIJ(i,1);
	}

      cout << "med_test Max Area " << maxarea << " Min Area " << minarea << endl;
      cout << "med_test Support Area " << areatot << endl;

      delete area ;
    }

  delete support ;

  //if (barycenter != NULL) delete barycenter;
  //if (volume != NULL ) delete volume;
  //if (area != NULL ) delete area;


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
      cout << "med_test Field " << fieldname << " not found !!!" << endl ;
      exit (-1) ;
    }
  }
  
  myFieldDriver.close() ;

  cout << "med_test Field "<< myField->getName() << " : " <<myField->getDescription() <<  endl ;
  int NumberOfComponents = myField->getNumberOfComponents() ;
  cout << "med_test - Nombre de composantes : "<< NumberOfComponents << endl ;
  for (int i=1; i<NumberOfComponents+1; i++) {
    cout << "med_test   - composante "<<i<<" :"<<endl ;
    cout << "med_test       - nom         : "<<myField->getComponentName(i)<< endl;
    cout << "med_test       - description : "<<myField->getComponentDescription(i) << endl;
    cout << "med_test       - units       : "<<myField->getMEDComponentUnit(i) << endl;
  }
  cout << "med_test - iteration :" << endl ;
  cout << "med_test     - numero : " << myField->getIterationNumber()<< endl  ;
  cout << "med_test     - ordre  : " << myField->getOrderNumber()<< endl  ;
  cout << "med_test     - temps  : " << myField->getTime()<< endl  ;

  cout << "med_test - Valeurs :"<<endl;
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  //    for (int i=1; i<NumberOfComponents+1; i++) {
  //      double * value = myField->getValueI(MED_NO_INTERLACE,i) ;
  //      for (int j=0; j<NumberOf; j++)
  //        cout << value[j]<< " ";
  //      cout<<endl;
  //    }
  MEDARRAY<double> * myvalue = myField->getvalue();
  const double * value ;
  for (int i=1; i<NumberOf+1; i++) {
    value = myvalue->getRow(i) ;
    for (int j=0; j<NumberOfComponents; j++)
      cout << value[j]<< " ";
    cout<<endl;
  }
  cout<<endl;
  
  delete myField;
  delete mySupport;

  delete myMesh ;

  return 0;
}
