/* Programme de test du constructeur de copies de la classe FIELD_ de MEDMEM
   jroy - 12/12/2002 */

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
#include "MEDMEM_FieldConvert.hxx"
#include "MEDMEM_define.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;


void affiche_field_(FIELD_ * myField, const SUPPORT * mySupport)
{
  cout << "Field "<< myField->getName() << " : " <<myField->getDescription() <<  endl ;
  int NumberOfComponents = myField->getNumberOfComponents() ;
  cout << "- Nombre de composantes : "<< NumberOfComponents << endl ;
  for (int i=1; i<NumberOfComponents+1; i++) {
    cout << "  - composante "<<i<<" :"<<endl ;
    cout << "      - nom         : "<<myField->getComponentName(i)<< endl;
    cout << "      - description : "<<myField->getComponentDescription(i) << endl;
    cout << "      - units       : "<<myField->getMEDComponentUnit(i) << endl;
  }
  cout << "- iteration :" << endl ;
  cout << "    - numero : " << myField->getIterationNumber()<< endl  ;
  cout << "    - ordre  : " << myField->getOrderNumber()<< endl  ;
  cout << "    - temps  : " << myField->getTime()<< endl  ;

  cout << "- Type : " << myField->getValueType()<< endl;

  cout << "- Adresse support : " << mySupport << endl;
}

template <class INTERLACING_TAG>
void affiche_fieldT(FIELD<double, INTERLACING_TAG> * myField,
		    const SUPPORT * mySupport)
{
  affiche_field_((FIELD_ *) myField, mySupport);

  cout << "- Valeurs :"<<endl;
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int NumberOfComponents = myField->getNumberOfComponents() ;

  if ( myField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE ) {
    for (int i=1; i<NumberOf+1; i++) {
      const double * value = myField->getRow(i) ;
      for (int j=0; j<NumberOfComponents; j++)
	cout << value[j]<< " ";
      cout<<endl;
    }
  }
  else {
    for (int j=1; j<NumberOfComponents+1; j++) {
      const double * value = myField->getColumn(j) ;
      for (int i=0; i<NumberOf; i++)
	cout << value[i]<< " ";
      cout<<endl;
    }
  }
}

int main (int argc, char ** argv) {
  // int read; !! UNUSED VARIABLE !!

  if ((argc !=3) && (argc != 4)) {
    cerr << "Usage : " << argv[0] 
	 << " filename meshname fieldname" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH() ;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  int current = myMesh->addDriver(myMeshDriver);
  myMesh->read(current);

  // read field :
  if (argc != 4) exit(0) ;
  // else we have a field !
  string fieldname = argv[3];

  FIELD<double> * myField ;
  //  SUPPORT * mySupport = new SUPPORT(myMesh,"On_all_node",MED_NODE);
  SUPPORT * mySupport = new SUPPORT(myMesh,"On_all_cell",MED_CELL);
  try {
    myField = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
  } catch (...) {
    delete mySupport ;
    mySupport = new SUPPORT(myMesh,"On_all_node",MED_NODE);
    try {
      myField = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
    } catch (...) {
      cout << "Field double " << fieldname << " not found !!!" << endl ;
      exit (-1) ;
    }
  }
  
  affiche_fieldT(myField, mySupport);
  FIELD<double> * myField2 = new FIELD<double>(* myField); // Contructeur par recopie, sauf SUPPORT
  delete myField; // Ne détruit pas le Support 
  affiche_fieldT(myField2, myField2->getSupport());
  FIELD<double,NoInterlace>   * myField3  = FieldConvert( *myField2 );
  delete myField2;

  affiche_fieldT(myField3, myField3->getSupport());
  FIELD<double,FullInterlace> * myField4  = FieldConvert( *myField3 );
  delete myField3;
  affiche_fieldT(myField4, myField4->getSupport());
  delete myField4;

  FIELD<double,NoInterlace> * myField5 = new FIELD<double,NoInterlace>(mySupport,MED_DRIVER,filename,fieldname) ;
  affiche_fieldT(myField5, myField5->getSupport());
  delete myField5;

  delete mySupport ;
  delete myMesh ;

  return 0;
}
