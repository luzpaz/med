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

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_define.hxx"

#include<string>

#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static void affiche_support(const SUPPORT * mySupport) 
{
  cout << "  - Name : "<<mySupport->getName().c_str()<<endl ;
  cout << "  - Description : "<<mySupport->getDescription().c_str()<<endl ;
  cout << "  - Entity : "<<mySupport->getEntity()<<endl ;
  cout << "  - Entities list : "<<endl ;
  if (!(mySupport->isOnAllElements())) 
    {
      int NumberOfTypes = mySupport->getNumberOfTypes() ;
      cout<<"  - NumberOfTypes : "<<NumberOfTypes<<endl;
      const medGeometryElement * Types = mySupport->getTypes() ;
      for (int j=0;j<NumberOfTypes;j++) 
        {
          cout<<"    * Type "<<Types[j]<<" : ";
          int NumberOfElements = mySupport->getNumberOfElements(Types[j]) ;
          const int * Number = mySupport->getNumber(Types[j]) ;
          for (int k=0; k<NumberOfElements;k++)
            cout << Number[k] << " ";
          cout << endl ;
        }
    }
  else
    cout << "    Is on all entities !"<< endl;
}

int main (int argc, char ** argv)
{
  if (argc <3) // after 3, ignored !
    {
      cerr << "Usage : " << argv[0] 
           << " filename meshname" << endl << endl;
      exit(-1);
    }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ;
  myMeshDriver.close() ;

  //Construction d'un support total
  const SUPPORT * mySupportTotal = myMesh->getSupportOnAll(MED_CELL);

  cout << "Show Support on all :"<<endl ;
  affiche_support(mySupportTotal);
  SUPPORT * mySupport2 = new SUPPORT(* mySupportTotal);
  affiche_support(mySupport2);
  mySupport2->removeReference();

  //Construction d'un support partiel
  SUPPORT* mySupport = new SUPPORT;
  mySupport->setMesh(myMesh);
  mySupport->setName("Support on CELLs");
  mySupport->setEntity(MED_CELL);

  int NumberOfGeometricType = 0;
  int TotalNumberOfElements = 0;
  int * NumberOfElements = new int[myMesh->getNumberOfTypes(MED_CELL)];
  int * NumberValue = new int[myMesh->getGlobalNumberingIndex(MED_CELL)[myMesh->getNumberOfTypes(MED_CELL)]-1];
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
  mySupport->removeReference();
  affiche_support(mySupport2);
  mySupport2->removeReference();

  myMesh->removeReference();

  return 0;
}
