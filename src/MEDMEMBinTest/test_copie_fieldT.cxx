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
/* Programme de test du constructeur de copies de la classe FIELD_ de MEDMEM
   jroy - 12/12/2002 */

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

#include<string>

#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;


static void affiche_field_(FIELD_ * myField, const SUPPORT * mySupport)
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

int main (int argc, char ** argv)
{
  if ((argc !=3) && (argc != 4))
    {
      cerr << "Usage : " << argv[0] 
           << " filename meshname fieldname" << endl << endl;
      exit(-1);
    }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH;
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
  const SUPPORT * mySupport = myMesh->getSupportOnAll(MED_CELL);
  try
    {
      myField = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
    }
  catch (...)
    {
      mySupport = myMesh->getSupportOnAll(MED_NODE);
      try
        {
          myField = new FIELD<double>(mySupport,MED_DRIVER,filename,fieldname) ;
        }
      catch (...)
        {
          cout << "Field double " << fieldname << " not found !!!" << endl ;
          exit (-1) ;
        }
    }

  affiche_fieldT(myField, mySupport);
  FIELD<double> * myField2 = new FIELD<double>(* myField); // Contructeur par recopie, sauf SUPPORT
  myField->removeReference(); // Ne détruit pas le Support 
  affiche_fieldT(myField2, myField2->getSupport());
  FIELD<double,NoInterlace>   * myField3  = FieldConvert( *myField2 );
  myField2->removeReference();

  affiche_fieldT(myField3, myField3->getSupport());
  FIELD<double,FullInterlace> * myField4  = FieldConvert( *myField3 );
  myField3->removeReference();
  affiche_fieldT(myField4, myField4->getSupport());
  myField4->removeReference();

  FIELD<double,NoInterlace> * myField5 = new FIELD<double,NoInterlace>(mySupport,MED_DRIVER,filename,fieldname) ;
  affiche_fieldT(myField5, myField5->getSupport());
  myField5->removeReference();

  myMesh->removeReference();

  return 0;
}
