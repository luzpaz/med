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

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace MEDMEM;
using namespace MED_EN;

int myfunction1(int x);
int myfunction1(int x)
{
  return 2*x;
}

int myfunction2(int x);
int myfunction2(int x)
{
  return x/2;
}

using namespace std;
static void affiche_field_(FIELD_ * myField, const SUPPORT * mySupport)
{
  cout << "Field "<< myField->getName() << " : " <<myField->getDescription() <<  endl ;
  int NumberOfComponents = myField->getNumberOfComponents() ;
  cout << "- Nombre de composantes : "<< NumberOfComponents << endl ;
  cout << "- Nombre de valeurs     : "<< myField->getNumberOfValues() << endl ;
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

  cout << "- Adresse support : " << mySupport << endl;
}

static void affiche_fieldT(FIELD<int> * myField, const SUPPORT * mySupport)
{
  affiche_field_((FIELD_ *) myField, mySupport);

  cout << "- Valeurs :"<<endl;
  int NumberOf = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int NumberOfComponents = myField->getNumberOfComponents() ;

  for (int i=1; i<NumberOf+1; i++) 
    {
      const int * value = myField->getRow(i) ;
      for (int j=0; j<NumberOfComponents; j++)
        cout << value[j]<< " ";
      cout<<endl;
    }
  std::cout << std::endl;
  std::cout << "Norme euclidienne : " << myField->norm2() << endl;
  std::cout << "Norme max         : " << myField->normMax() << endl;
  try
    {
      for (int i=1; i<=myField->getNumberOfComponents(); ++i)
        std::cout << "Norme L2 - comp=" << i << " : " << myField->normL2(i) << endl;
      std::cout << "Norme L2          : " << myField->normL2() << endl;

      for (int i=1; i<=myField->getNumberOfComponents(); ++i)
        std::cout << "Norme L1 - comp=" << i << " : " << myField->normL1(i) << endl;
      std::cout << "Norme L1          : " << myField->normL1() << endl;
    }
  catch (MEDEXCEPTION &ex)
    {
      std::cout << ex.what() << std::endl;
    }
}

static void affiche_valeur_field(const char * intitule, const int taille, const FIELD<int>& f)
{
  const int * value=f.getValue();
  std::cout << endl << intitule;
  for(int i=0;i<taille;i++)
    std::cout << setw(3) << value[i] << " ";
}

int main (int argc, char ** argv)
{
  if (argc != 4)
    {
      cerr << "Usage : " << argv[0] 
           << " filename meshname fieldname" << endl << endl;
      exit(-1);
    }
  string filename = argv[1] ;
  string meshname = argv[2] ;
  string fieldname = argv[3];

  MESH * myMesh = new MESH(MED_DRIVER,filename,meshname);
  const SUPPORT * mySupport;
  FIELD<int> * myField1;
  try
    {
      /* read MESH, SUPPORT and FIELD */
      mySupport = myMesh->getSupportOnAll(MED_CELL);
      myField1 = new FIELD<int>(mySupport,MED_DRIVER,filename,fieldname) ;
    }
  catch (MEDEXCEPTION &ex)
    {
      mySupport = myMesh->getSupportOnAll(MED_NODE);
      try
        {
          myField1 = new FIELD<int>(mySupport,MED_DRIVER,filename,fieldname) ;
          myField1->setValueIJ(10,1,-9); // pour tester les normes max avec une valeur negative
        }
      catch (...)
        {
          cout << "Field int " << fieldname << " not found !!!" << endl ;
          exit (-1) ;
        }
    }

    FIELD<int> * myField2 = new FIELD<int>(* myField1);
    //myField1->setNumberOfValues(16); // PROVISOIRE !! BUG
    //myField2->setNumberOfValues(16); // PROVISOIRE !! BUG
//      FIELD<int>* myField1_vol=myField1->getSupport()->getMesh()->getVolume(myField1->getSupport());
//      affiche_fieldT(myField1_vol, myField1->getSupport());

    affiche_fieldT(myField1, myField1->getSupport());
    std::cout <<  endl << string(60,'-') << endl;
    affiche_fieldT(myField2, myField2->getSupport());

    // Verifie plusieurs cas de non compatibilité 

    // Unites non compatibles
    const string unite=myField1->getMEDComponentUnit(1);
    myField1->setMEDComponentUnit(1,string("UniteBidon"));
    try
    {
        std::cout << endl << string(60,'-') << endl;
        std::cout<< "Test incompatibilité d'unité :" << endl;
        FIELD<int> *myFieldPlus = *myField1 + *myField2;
        myFieldPlus->removeReference();
    }
    catch (MEDEXCEPTION & ex)
    {
        std::cout << "MEDEXCEPTION : " << ex.what() << endl;
        myField1->setMEDComponentUnit(1,unite);
    }

    // numberOfComponents non compatibles
    const int numberOfComponents =myField1->getNumberOfComponents();
    myField1->setNumberOfComponents(4);
    try
    {
        std::cout << endl << string(60,'-') << endl;
        std::cout<< "Test incompatibilité nombre de composantes :" << endl;
        FIELD<int> *myFieldPlus = *myField1 + *myField2;
        myFieldPlus->removeReference();
    }
    catch (MEDEXCEPTION & ex)
    {
        std::cout << ex.what() << endl;
        myField1->setNumberOfComponents(numberOfComponents);
    }

    // supports non compatibles
    const SUPPORT *mySupport2=myMesh->getSupportOnAll(MED_NODE);
    myField1->setSupport(mySupport2);
    try
    {
        std::cout << endl << string(60,'-') << endl;
        std::cout<< "Test incompatibilité des supports :" << endl;
        FIELD<int> *myFieldPlus = *myField1 + *myField2;
        myFieldPlus->removeReference();
    }
    catch (MEDEXCEPTION & ex)
    {
        std::cout << ex.what() << endl;
        myField1->setSupport( myField2->getSupport() );
    }

    // champs de taille nulle
    myField1->setNumberOfComponents(0);
    myField2->setNumberOfComponents(0);
    try
    {
        std::cout << endl << string(60,'-') << endl;
        std::cout<< "Test incompatibilité taille nulle :" << endl;
        FIELD<int> *myFieldPlus = *myField1 + *myField2;
        myFieldPlus->removeReference();
    }
    catch (MEDEXCEPTION & ex)
    {
        std::cout << ex.what() << endl;
    }
    try
    {
        myField1->norm2();
    }
    catch (MEDEXCEPTION & ex)
    {
        std::cout << ex.what() << endl;
        myField1->setNumberOfComponents(numberOfComponents);
        myField2->setNumberOfComponents(numberOfComponents);
    }

    // Apres toutes ces exceptions, des opérations qui marchent!

    FIELD<int> *myFieldPlus = *myField1 + *myField2;
    FIELD<int> *myFieldMoins = *myField1 - *myField2;
    FIELD<int> *myFieldNeg = -(*myField1);
    FIELD<int> *myFieldFois = *myField1 * *myField2;
    FIELD<int> *myFieldDiv = *myField1 / *myField2;
    FIELD<int> *myFieldAsso = (*myField1)+*((*myField2)*(*myField2));
    FIELD<int>* myFieldadd = FIELD<int>::add(*myField1, *myField2);
    FIELD<int>* myFieldsub = FIELD<int>::sub(*myField1, *myField2);
    FIELD<int>* myFieldmul = FIELD<int>::mul(*myField1, *myField2);
    FIELD<int>* myFielddiv = FIELD<int>::div(*myField1, *myField2);
    FIELD<int>* myFieldDot = FIELD<int>::scalarProduct(*myField1, *myField2);

    std::cout <<  endl << string(60,'-') << endl << "f1+f2 :" << endl << endl;
    affiche_fieldT(myFieldPlus, myFieldPlus->getSupport());
    std::cout <<  endl << string(60,'-') << endl << "add(f1,f2) :" << endl << endl;
    affiche_fieldT(myFieldadd, myFieldadd->getSupport());
    std::cout <<  endl << string(60,'-') << endl << "scalarProduct(f1,f2) :" << endl << endl;
    affiche_fieldT(myFieldDot, myFieldDot->getSupport());
    std::cout <<  endl << string(60,'-') << endl << " - f1 :" << endl << endl;
    affiche_fieldT(myFieldNeg, myFieldNeg->getSupport());
    int size=myFieldPlus->getNumberOfValues()*myFieldPlus->getNumberOfComponents();
  
    std::cout <<  endl << string(60,'-') << endl << "Tests opérations :" << endl << endl;
    affiche_valeur_field("  f1    :", size, *myField1);
    affiche_valeur_field("  f2    :", size, *myField2);
    std::cout << endl << "        " << string(4*size,'-');

    affiche_valeur_field("  +     :", size, *myFieldPlus);
    affiche_valeur_field(" add    :", size, *myFieldadd);
    affiche_valeur_field("  -     :", size, *myFieldMoins);
    affiche_valeur_field(" sub    :", size, *myFieldsub);
    affiche_valeur_field("  *     :", size, *myFieldFois);
    affiche_valeur_field(" mul    :", size, *myFieldmul);
    affiche_valeur_field("  /     :", size, *myFieldDiv);
    affiche_valeur_field(" div    :", size, *myFielddiv);
    affiche_valeur_field("f1+f2*f1:", size, *myFieldAsso);
    affiche_valeur_field("  - f1  :", size, *myFieldNeg);

    myFieldPlus->removeReference();
    myFieldMoins->removeReference();
    myFieldFois->removeReference();
    myFieldDiv->removeReference();
    myFieldAsso->removeReference();
    myFieldNeg->removeReference();

    // Test applyLin
    std::cout << endl;
    myField1->applyLin(1,1);
    affiche_valeur_field(" f1+1 :", size, *myField1);
    myField1->applyLin(1,-1);
    affiche_valeur_field(" -> f1  :", size, *myField1);
    
    // Test applyFunc
    std::cout << endl;
    myField1->applyFunc<myfunction1>();
    affiche_valeur_field(" CB 2f1 :", size, *myField1);
    myField1->applyFunc<myfunction2>();
    affiche_valeur_field(" -> f1  :", size, *myField1);

    // Test operateur +=
    std::cout << endl;
    *myField1 += *myField2;
    affiche_valeur_field(" f1+=f2 :", size, *myField1);

    // Test operateur *=
    *myField1 *= *myField2;
    affiche_valeur_field(" f1*=f2 :", size, *myField1);

    // Test operateur /=
    *myField1 /= *myField2;
    affiche_valeur_field(" f1/=f2 :", size, *myField1);

    // Test operateur -=
    *myField1 -= *myField2;
    affiche_valeur_field(" f1-=f2 :", size, *myField1);

    std::cout << endl << endl; 


    myFieldadd->removeReference();
    myFieldsub->removeReference();
    myFieldmul->removeReference();
    myFielddiv->removeReference();
    myFieldDot->removeReference();
//    myField1_vol->removeReference();

    myField1->removeReference();
    myField2->removeReference();
    mySupport->removeReference();
    myMesh->removeReference();
    return 0;
}
