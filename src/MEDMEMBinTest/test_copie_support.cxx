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
using namespace MED_EN;

void affiche_support(const SUPPORT * mySupport) 
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
      cout<<"    * Type "<<Types[j]<<" : ";
      int NumberOfElements = mySupport->getNumberOfElements(Types[j]) ;
      const int * Number = mySupport->getNumber(Types[j]) ;
      for (int k=0; k<NumberOfElements;k++)
	cout << Number[k] << " ";
      cout << endl ;
    }
  } else
    cout << "    Is on all entities !"<< endl;
}


void affiche_famille(MESH *myMesh,medEntityMesh Entity) 
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

void affiche_groupe(MESH *myMesh,medEntityMesh Entity) 
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
  int read;

  if (argc <3) { // after 3, ignored !
    cerr << "Usage : " << argv[0] 
	 << " filename meshname" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH() ;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ;
  myMeshDriver.close() ;

  //Construction d'un support total
  SUPPORT * mySupport = new SUPPORT(myMesh,"Support on CELLs",MED_CELL);

  cout << "Show Support on all :"<<endl ;
  affiche_support(mySupport);
  SUPPORT * mySupport2 = new SUPPORT(* mySupport);
  delete mySupport;
  affiche_support(mySupport2);
  delete mySupport2;

  //Construction d'un support partiel
  mySupport = new SUPPORT(myMesh,"Support on CELLs",MED_CELL);
  mySupport->setAll(false);

  //  int NumberOfGeometricType = 1;
  int NumberOfGeometricType = 0;
  //  int TotalNumberOfEntity = 2;
  //  medGeometryElement * GeometricTypePartial = new medGeometryElement[NumberOfGeometricType];
  //  GeometricTypePartial[0] = MED_HEXA8;
  int TotalNumberOfElements = 0;
  int * NumberOfElements = new int[myMesh->getNumberOfTypes(MED_CELL)];
  //  NumberOfEntity[0] = 2;
  //  int * NumberValue = new int[TotalNumberOfEntity];
  int * NumberValue = new int[myMesh->getGlobalNumberingIndex(MED_CELL)[myMesh->getNumberOfTypes(MED_CELL)]-1];
  //  NumberValue[0] = 14;
  //  NumberValue[1] = 15;
  int cmp = 0;
  medGeometryElement * GeometricTypePartial = new medGeometryElement[myMesh->getNumberOfTypes(MED_CELL)];
  const medGeometryElement * GeometricType = myMesh->getTypes(MED_CELL);
  for (int i=0;i<myMesh->getNumberOfTypes(MED_CELL);i=i+2)
    { 
      NumberOfGeometricType=NumberOfGeometricType+1;
      TotalNumberOfElements=TotalNumberOfElements+myMesh->getNumberOfElements(MED_CELL,GeometricType[i]);
      NumberOfElements[i/2]=myMesh->getNumberOfElements(MED_CELL,GeometricType[i]);
      for (int j=0;j<myMesh->getNumberOfElements(MED_CELL,GeometricType[i]);j++)
	{
	  NumberValue[cmp]=myMesh->getGlobalNumberingIndex(MED_CELL)[i]+j;
	  cmp=cmp+1;
	}
      GeometricTypePartial[i/2]=GeometricType[i];
    }

  mySupport->setpartial("Support partiel",NumberOfGeometricType,TotalNumberOfElements,GeometricTypePartial,NumberOfElements,NumberValue);

  delete[] NumberOfElements ;
  delete[] NumberValue ;
  delete[] GeometricTypePartial ;

  cout << "Show Partial Support :"<<endl ;
  affiche_support(mySupport);
  mySupport2 = new SUPPORT(* mySupport);
  delete mySupport;
  affiche_support(mySupport2);
  delete mySupport2;

  /*
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
  */

  delete myMesh ;

  return 0;
}
