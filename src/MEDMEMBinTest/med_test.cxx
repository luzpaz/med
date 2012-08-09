// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_define.hxx"

#include<string>

#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static double dmax(double x, double y) { return (x>y)?x:y;}

static double dmin(double x, double y) { return (x>y)?y:x;}

static double infty = 1.e20;

static void affiche_support(const SUPPORT * mySupport) 
{
  cout << "  - Name : "<<mySupport->getName().c_str()<<endl ;
  cout << "  - Description : "<<mySupport->getDescription().c_str()<<endl ;
  cout << "  - Entity : "<<mySupport->getEntity()<<endl ;
  cout << "  - Entities list : "<<endl ;
  if (!(mySupport->isOnAllElements())) {
    int NumberOfTypes = mySupport->getNumberOfTypes() ;
    cout<<"  - NumberOfTypes : "<<NumberOfTypes<<endl;
    const medGeometryElement * Types = mySupport->getTypes() ;
    for (int j=0;j<NumberOfTypes;j++) {
      cout << "    * Type "<<Types[j]<<" : " ;
      int NumberOfElements = mySupport->getNumberOfElements(Types[j]) ;
      const int * Number = mySupport->getNumber(Types[j]) ;
      for (int k=0; k<NumberOfElements;k++)
        cout << Number[k] << " ";
      cout << endl ;
    }
  } else
    cout << "    Is on all entities !"<< endl;
}


static void affiche_famille(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfFamilies = myMesh->getNumberOfFamilies(Entity) ;
  cout << "NumberOfFamilies : "<<NumberOfFamilies<<endl;
  for (int i=1; i<NumberOfFamilies+1;i++) {
    const FAMILY* myFamily = myMesh->getFamily(Entity,i);
    affiche_support(myFamily);
    cout << "  - Identifier : "<<myFamily->getIdentifier()<<endl ;
    int NumberOfAttributes = myFamily->getNumberOfAttributes() ;
    cout << "  - Attributes ("<<NumberOfAttributes<<") :"<<endl;
    for (int j=1;j<NumberOfAttributes+1;j++)
      cout << "    * "<<myFamily->getAttributeIdentifier(j)<<" : "<<myFamily->getAttributeValue(j)<<", "<<myFamily->getAttributeDescription(j).c_str()<<endl ;
    int NumberOfGroups = myFamily->getNumberOfGroups() ;
    cout << "  - Groups ("<<NumberOfGroups<<") :"<<endl;
    for (int j=1;j<NumberOfGroups+1;j++)
      cout << "    * "<<myFamily->getGroupName(j).c_str()<<endl ;
  }
}

static void affiche_groupe(MESH *myMesh,medEntityMesh Entity) 
{
  int NumberOfGroups = myMesh->getNumberOfGroups(Entity) ;
  cout << "NumberOfGroups : "<<NumberOfGroups<<endl;
  for (int i=1; i<NumberOfGroups+1;i++) {
    const GROUP* myGroup = myMesh->getGroup(Entity,i);
    affiche_support(myGroup);
    int NumberOfFamillies = myGroup->getNumberOfFamilies() ;
    cout << "  - Families ("<<NumberOfFamillies<<") :"<<endl;
    for (int j=1;j<NumberOfFamillies+1;j++)
      cout << "    * "<<myGroup->getFamily(j)->getName().c_str()<<endl ;
  }
}

int main (int argc, char ** argv) {

  if ((argc !=3) && (argc != 4)) {
    cerr << "Usage : " << argv[0] 
         << " filename meshname [fieldname]" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH(MED_DRIVER,filename,meshname) ;
  
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int MeshDimension  = myMesh->getMeshDimension() ;
  int NumberOfNodes  = myMesh->getNumberOfNodes() ;

  cout << "Space Dimension : " << SpaceDimension << endl << endl ; 

  cout << "Mesh Dimension : " << MeshDimension << endl << endl ; 

  const double * Coordinates = myMesh->getCoordinates(MED_FULL_INTERLACE) ;

  cout << "Show Nodes Coordinates : " << endl ;

  cout << "Name :" << endl ;
  const string * CoordinatesNames = myMesh->getCoordinatesNames() ;
  for(int i=0; i<SpaceDimension ; i++) {
    cout << " - " << CoordinatesNames[i] << endl ;
  }
  cout << "Unit :" << endl ;
  const string * CoordinatesUnits = myMesh->getCoordinatesUnits() ;
  for(int i=0; i<SpaceDimension ; i++) {
    cout << " - " << CoordinatesUnits[i] << endl ;
  }
  for(int i=0; i<NumberOfNodes ; i++) {
    cout << "Nodes " << i+1 << " : " ;
    for (int j=0; j<SpaceDimension ; j++)
      cout << Coordinates[i*SpaceDimension+j] << " " ;
    cout << endl ;
  }

  int NumberOfTypes = myMesh->getNumberOfTypes(MED_CELL) ;
  const medGeometryElement  * Types = myMesh->getTypes(MED_CELL) ;

  cout << "Show Connectivity (Nodal) :" << endl ;
  for (int i=0; i<NumberOfTypes; i++) {
    cout << "For type " << Types[i] << " : " << endl ;
    int NumberOfElements = myMesh->getNumberOfElements(MED_CELL,Types[i]);
    const int * connectivity =  myMesh->getConnectivity(MED_NODAL,MED_CELL,Types[i]);
    int NomberOfNodesPerCell = Types[i]%100 ;
    for (int j=0;j<NumberOfElements;j++){
      cout << "Element "<< j+1 <<" : " ;
      for (int k=0;k<NomberOfNodesPerCell;k++)
        cout << connectivity[j*NomberOfNodesPerCell+k]<<" ";
      cout << endl ;
    }
  }

  cout << "Show Family :"<<endl ;
  affiche_famille(myMesh,MED_NODE);
  affiche_famille(myMesh,MED_CELL);
  affiche_famille(myMesh,MED_FACE);
  affiche_famille(myMesh,MED_EDGE);

  cout << "Show Group :"<<endl ;
  affiche_groupe(myMesh,MED_NODE);
  affiche_groupe(myMesh,MED_CELL);
  affiche_groupe(myMesh,MED_FACE);
  affiche_groupe(myMesh,MED_EDGE);

  cout << "Show Reverse Nodal Connectivity :" << endl ;
  const int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
  const int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    cout << "Node "<<i+1<<" : " ;
    for (int j=ReverseNodalConnectivityIndex[i];j<ReverseNodalConnectivityIndex[i+1];j++)
      cout << ReverseNodalConnectivity[j-1] << " " ;
    cout << endl ;
  }

  cout << "Show Connectivity (Descending) :" << endl ;
  int NumberOfElements ;
  const int * connectivity ;
  const int * connectivity_index ;
  myMesh->calculateConnectivity(MED_DESCENDING,MED_CELL);
  try {
    NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
    connectivity =  myMesh->getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
    connectivity_index =  myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  }
  catch (MEDEXCEPTION& m) {
    cout << m.what() << endl ;
    exit (-1) ;
  }
  for (int j=0;j<NumberOfElements;j++) {
    cout << "Element "<<j+1<<" : " ;
    for (int k=connectivity_index[j];k<connectivity_index[j+1];k++)
      cout << connectivity[k-1]<<" ";
    cout << endl ;
  }

  cout << "Show Reverse Descending Connectivity :" << endl ;
  const int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
  const int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;

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
    INFOS_MED("ERROR : MeshDimension = 1 !");
    INFOS_MED("We could not see Reverse Descending Connectivity.") ;
  } else {
    NumberOfConstituents = myMesh->getNumberOfElements (constituentEntity,MED_ALL_ELEMENTS);
    for (int i=0; i<NumberOfConstituents; i++) {
      cout << constituent <<i+1<<" : " ;
      for (int j=ReverseDescendingConnectivityIndex[i];j<ReverseDescendingConnectivityIndex[i+1];j++)
        cout << ReverseDescendingConnectivity[j-1] << " " ;
      cout << endl ;
    }
  }
  cout << "Show "<<constituent<<" Connectivity (Nodal) :" << endl ;
  const int * face_connectivity =  myMesh->getConnectivity(MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
  const int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    cout << constituent <<i+1<<" : " ;
    for (int j=face_connectivity_index[i];j<face_connectivity_index[i+1];j++)
      cout << face_connectivity[j-1]<<" ";
    cout << endl ;
  }

  /* test of normal, area, volume, barycenter */

  const SUPPORT* support1 = myMesh->getSupportOnAll(constituentEntity);
  cout << "Building of the Support on all cells dimensionned (Meshdim-1) of the mesh :"<< endl ;
  cout << "Face in 3D or Edge in 2D" << endl;

  cout << "Getting the normal of each face of this support !" << endl ;

  FIELD<double>* normal = myMesh->getNormal(support1);

  double normal_square, norm ;
  double maxnorm=-infty;
  double minnorm=infty;
  double tmp_value ;
  for (int i = 1; i<=NumberOfConstituents;i++)
    {
      normal_square = 0. ;
      cout << "Normal " << i << " " ; 
      for (int j=1; j<=SpaceDimension; j++)
        {
          tmp_value = normal->getValueIJ(i,j) ;
          normal_square += tmp_value*tmp_value ;
          cout << tmp_value << " " ;
        }
      norm = sqrt(normal_square);
      maxnorm = dmax(maxnorm,norm);
      minnorm = dmin(minnorm,norm);
      cout << ", Norm = " << norm << endl;
    }
  cout << "Max Norm " << maxnorm << " Min Norm " << minnorm << endl;

  if(normal)
    normal->removeReference() ;

  if (SpaceDimension == 2)
    {
      cout << "Getting the length of each edge !" << endl ;

      FIELD<double>* length = myMesh->getLength(support1);

      double length_value,maxlength,minlength;
      maxlength = -infty;
      minlength = infty;
      for (int i = 1; i<=NumberOfConstituents;i++)
        {
          length_value = length->getValueIJ(i,1) ;
          cout << "Length " << i << " " << length_value << endl;
          maxlength = dmax(maxlength,length_value);
          minlength = dmin(minlength,length_value);
        }
      cout << "Max Length " << maxlength << " Min Length " << minlength << endl;
      if(length)
        length->removeReference();
    }

  cout << "Building of the Support on all space-dimensionned cells of the mesh :"<< endl ;
  const SUPPORT * support = myMesh->getSupportOnAll( MED_CELL );

  cout << "Getting the barycenter of each element of this support !" << endl ;

  FIELD<double>* barycenter = myMesh->getBarycenter(support);
  NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);

  for (int i = 1; i<=NumberOfElements;i++)
    {
      if (SpaceDimension == 3)
        cout << "Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << " " << barycenter->getValueIJ(i,3) << endl;

      if (SpaceDimension == 2)
        cout << "Barycenter " << i << " " << barycenter->getValueIJ(i,1) << " " << barycenter->getValueIJ(i,2) << endl;
    }
  if(barycenter)
    barycenter->removeReference();

  if (SpaceDimension == 3)
    {
      cout << "Getting the Volume of each element of this support which is a 3D one !" << endl;

      FIELD<double>* volume = myMesh->getVolume(support);

      double maxvol,minvol,voltot;
      maxvol = -infty;
      minvol = infty;
      voltot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
        {
          cout << "Volume " << i << " " << volume->getValueIJ(i,1) << endl;
          maxvol = dmax(maxvol,volume->getValueIJ(i,1));
          minvol = dmin(minvol,volume->getValueIJ(i,1));
          voltot = voltot + volume->getValueIJ(i,1);
        }

      cout << "Max Volume " << maxvol << " Min Volume " << minvol << endl;
      cout << "Support Volume " << voltot << endl;
      if(volume)
        volume->removeReference() ;
    }
  else if (SpaceDimension == 2)
    {
      cout << "Getting the Area of each element of this support which is a 2D one !" << endl;

      FIELD<double>* area = myMesh->getArea(support);

      double maxarea,minarea,areatot;
      maxarea = -infty;
      minarea = infty;
      areatot = 0.0;
      for (int i = 1; i<=NumberOfElements;i++)
        {
          cout << "Area " << i << " " << area->getValueIJ(i,1) << endl;
          maxarea = dmax(maxarea,area->getValueIJ(i,1));
          minarea = dmin(minarea,area->getValueIJ(i,1));
          areatot = areatot + area->getValueIJ(i,1);
        }

      cout << "Max Area " << maxarea << " Min Area " << minarea << endl;
      cout << "Support Area " << areatot << endl;
      if(area)
        area->removeReference();
    }

  if (argc < 4) return 0;

  // read field :

  if (argc != 4) exit(0) ;
  // else we have a field !

  string fieldname = argv[3];

  const SUPPORT * mySupport = myMesh->getSupportOnAll(MED_CELL);
  FIELD<double> * myField= new FIELD<double>() ;

  myField->setName(fieldname);
  myField->setSupport(mySupport);
  MED_FIELD_RDONLY_DRIVER<double> myFieldDriver(filename,myField) ;
  myFieldDriver.setFieldName(fieldname);
  myFieldDriver.open() ;

  try
    {
      myFieldDriver.read() ;
    }
  catch (...)
    {
      mySupport = myMesh->getSupportOnAll(MED_NODE);
      myField->setSupport(mySupport);
      try
        {
          myFieldDriver.read() ;
        }
      catch (...)
        {
          cout << "Field " << fieldname << " not found !!!" << endl ;
          exit (-1) ;
        }
    }

  myFieldDriver.close() ;

  cout << "Field "<< myField->getName() << " : " <<myField->getDescription() <<  endl ;
  int NumberOfComponents = myField->getNumberOfComponents() ;
  cout << "- Nombre de composantes : "<< NumberOfComponents << endl ;
  for (int i=1; i<NumberOfComponents+1; i++)
    {
      cout << "  - composante "<<i<<" :"<<endl ;
      cout << "      - nom         : "<<myField->getComponentName(i)<< endl;
      cout << "      - description : "<<myField->getComponentDescription(i) << endl;
      cout << "      - units       : "<<myField->getMEDComponentUnit(i) << endl;
    }
  cout << "- iteration :" << endl ;
  cout << "    - numero : " << myField->getIterationNumber()<< endl  ;
  cout << "    - ordre  : " << myField->getOrderNumber()<< endl  ;
  cout << "    - temps  : " << myField->getTime()<< endl  ;

  cout << "- Valeurs :"<<endl;
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  MEDMEM_Array<double> * myvalue = myField->getArrayNoGauss();
  const double * value ;
  for (int i=1; i<NumberOf+1; i++)
    {
      value = myvalue->getRow(i) ;
      for (int j=0; j<NumberOfComponents; j++)
        cout << value[j]<< " ";
      cout<<endl;
    }
  cout<<endl;

  myField->removeReference();
  myMesh->removeReference();

  return 0;
}
