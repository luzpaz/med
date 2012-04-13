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
#include "MEDMEM_define.hxx"

#include<string>

#include <math.h>
#include <stdlib.h>


using namespace MEDMEM;
using namespace MED_EN;


static void affiche_field(FIELD_ * myField, const SUPPORT * mySupport)
{
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

  cout << "- Type : " << myField->getValueType()<< endl;
  /*
    cout << "- Valeurs :"<<endl;
    int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);

    for (int i=1; i<NumberOf+1; i++) 
    {
    double * value = myField->getValueI(MED_FULL_INTERLACE,i) ;
    for (int j=0; j<NumberOfComponents; j++)
    cout << value[j]<< " ";
    cout<<endl;
    }
  */
  cout << "- Adresse support : " << mySupport << endl;
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

  MESH * myMesh= new MESH(MED_DRIVER,filename,meshname) ;

  // read field :

  if (argc != 4) exit(0) ;
  // else we have a field !

  string fieldname = argv[3];


  const SUPPORT * mySupport = myMesh->getSupportOnAll(MED_EN::MED_CELL);
  FIELD<double> * myField = new FIELD<double>() ;

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
      myField->setSupport(0);
      mySupport = myMesh->getSupportOnAll(MED_EN::MED_NODE);
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

  FIELD_ * pt_field_ = myField;
  affiche_field(pt_field_, mySupport);
  FIELD_ * pt_field_2 = new FIELD_(* pt_field_);
  myField->removeReference();
  affiche_field(pt_field_2, pt_field_2->getSupport());

  pt_field_2->removeReference();
  myMesh->removeReference() ;

  return 0;
}
