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
//CCRT      const int * Number = mySupport->getNumber(Types[j]) ;
      const med_int * Number = mySupport->getNumber(Types[j]) ;
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

  const FAMILY * myFamily = myMesh->getFamily(MED_NODE,1);
  //On renseigne les attributs spécifiques à FAMILY (p/r à SUPPORT) et non renseignés lors de la lecture du maillage
//    int  NumberOfAttribute = 3;
//    int *AttributeIdentifier = new int[NumberOfAttribute];
//    int *AttributeValue = new int[NumberOfAttribute];
//    string *AttributeDescription = new string[NumberOfAttribute];
//    char *tmp;
//    for (int i=0;i<NumberOfAttribute;i++)
//      {
//        AttributeIdentifier[i]=i+1;
//        AttributeValue[i]=(i+1)*10;
//        sprintf(tmp,"Attribut N° %d",i+1);
//        AttributeDescription[i]=tmp;
//      }

//    myFamily->setNumberOfAttributes(NumberOfAttribute);
//    myFamily->setAttributesIdentifiers (AttributeIdentifier);
//    myFamily->setAttributesValues (AttributeValue);
//    myFamily->setAttributesDescriptions (AttributeDescription);

  cout << "Show Family :"<<endl ;
  affiche_famille(myFamily);
  FAMILY * myFamily2 = new FAMILY(* myFamily);
  //delete myFamily;
  cout << "Show Family2 :"<<endl ;
  affiche_famille(myFamily2);
  FAMILY * myFamily3 = new FAMILY(* myFamily2);
  delete myFamily2;
  cout << "Show Family3 :"<<endl ;
  affiche_famille(myFamily3);
  delete myFamily3;

  cout << "That's all"<<endl ;

  /*
  cout << "Show Group :"<<endl ;
  affiche_groupe(myMesh,MED_NODE);
  affiche_groupe(myMesh,MED_CELL);
  affiche_groupe(myMesh,MED_FACE);
  affiche_groupe(myMesh,MED_EDGE);
  */

  delete myMesh;

  return 0;
}
