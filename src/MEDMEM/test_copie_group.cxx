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


void affiche_famille(const FAMILY * myFamily)
{
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

void affiche_groupe(const GROUP * myGroup) 
{
    affiche_support(myGroup);
    int NumberOfFamillies = myGroup->getNumberOfFamilies() ;
    cout << "  - Families ("<<NumberOfFamillies<<") :"<<endl;
    for (int j=1;j<NumberOfFamillies+1;j++)
      cout << "    * "<<myGroup->getFamily(j)->getName().c_str()<<endl ;
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

  const GROUP * myGroup = myMesh->getGroup(MED_NODE,1);
  cout << "Show Group :"<<endl ;
  affiche_groupe(myGroup);
  GROUP * myGroup2 = new GROUP(* myGroup);
  //delete myGroup; // no because in mesh !!
  affiche_groupe(myGroup2);
  GROUP * myGroup3 = new GROUP(* myGroup2);
  delete myGroup2;
  affiche_groupe(myGroup3);
  delete myGroup3;

  delete myMesh ;

  return 0;
}
