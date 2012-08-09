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


static void affiche_groupe(const GROUP * myGroup) 
{
    affiche_support(myGroup);
    int NumberOfFamillies = myGroup->getNumberOfFamilies() ;
    cout << "  - Families ("<<NumberOfFamillies<<") :"<<endl;
    for (int j=1;j<NumberOfFamillies+1;j++)
      cout << "    * "<<myGroup->getFamily(j)->getName().c_str()<<endl ;
}

int main (int argc, char ** argv) {

  if (argc <3) { // after 3, ignored !
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

  const GROUP * myGroup = myMesh->getGroup(MED_NODE,1);
  cout << "Show Group :"<<endl ;
  affiche_groupe(myGroup);
  GROUP * myGroup2 = new GROUP(* myGroup);
  affiche_groupe(myGroup2);
  GROUP * myGroup3 = new GROUP(* myGroup2);
  myGroup2->removeReference();
  affiche_groupe(myGroup3);
  myGroup3->removeReference();

  myMesh->removeReference() ;

  return 0;
}
