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
#include <stdlib.h>
#include<string>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_GaussLocalization.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define INTERLACING_MODE FullInterlace
static int fct2(int i,const int * number) { return i;}
static int fct1(int i,const int * number) { return number[i]; }

static void affiche_field_(FIELD_ * myField)
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
  //PRESENCE DE POINTS DE GAUSS
}

// Cas de traitement des valeurs sans spécificité concernant
// les points de Gauss ou les profils
// Pas de spécificité concernant le type géométrique
template <class INTERLACING_TAG>
void affiche_fieldT(FIELD<double,INTERLACING_TAG> * myField)
{
  const double * value = 0;
  affiche_field_((FIELD_ *) myField);

  cout.setf(ios::fixed);

  int numberOf              = myField->getNumberOfValues();
  int numberOfComponents    = myField->getNumberOfComponents() ;
  int valueLength           = myField->getValueLength();
  int numberOfGeometricType = myField->getNumberOfGeometricTypes();
  const int * nbOfElements  = myField->getNumberOfElements();
  const MED_EN::medGeometryElement * typeList      = myField->getGeometricTypes();
  // Suivant le traitement, on peut faire sortir  si il y a des points de Gauss
  // ou des profils

  cout << "myField->getValueLength        (MED_ALL_ELEMENTS) : " << valueLength << endl;
  cout << "myField->getNumberOfElements   (MED_ALL_ELEMENTS) : " << numberOf   << endl;
  cout << "myField->getNumberOfComponents ()                 : " << numberOfComponents << endl;
  cout << "myField->getNumberOfGeometricType ()              : " << numberOfGeometricType << endl;
  for (int i=0; i < numberOfGeometricType; i++) {
    cout << "Number Of Elements on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbOfElements[i] << endl;
    cout << "Number Of Gauss Points on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< myField->getNumberOfGaussPoints(typeList[i]) << endl;
    cout << "Localization description : " << endl << myField->getGaussLocalization(typeList[i]) << endl;
  }

  cout << "- Valeurs :"<<endl;
  for (int index=0; index < valueLength; index++) {
    cout << "value["<<index<<"] = "<< value[index];
    cout<<endl;
  }

}

// Spécialisation du traitement pour le mode FullInterlace
// Spécifité de traitement par rapport aux profils (utilisation du SUPPORT)
// Pas de spécificité concernant le type géométrique
// Pas de spécificité concernant les points de Gauss
template <>
void affiche_fieldT(FIELD<double, FullInterlace> * myField)
{
  const double * value = 0;
  const int    * number = 0;

  affiche_field_((FIELD_ *) myField);
  const SUPPORT * mySupport=myField->getSupport();

  cout.setf(ios::fixed);


  int numberOfComponents    = myField->getNumberOfComponents() ;
  int valueLength           = myField->getValueLength();
  const int * nbGaussPoints = myField->getNumberOfGaussPoints();

  int numberOf              = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int numberOfGeometricType = mySupport->getNumberOfTypes();
  const int * nbOfElements  = mySupport->getNumberOfElements();
  const MED_EN::medGeometryElement * typeList      = mySupport->getTypes();
  bool  onAll               = mySupport->isOnAllElements();

  cout << "mySupport->getValueLength           (MED_ALL_ELEMENTS) : " << valueLength << endl;
  cout << "mySupport->getNumberOfElements      (MED_ALL_ELEMENTS) : " << numberOf   << endl;
  cout << "mySupport->getNumberOfComponents    ()                 : " << numberOfComponents << endl;
  cout << "mySupport->getNumberOfGeometricType ()                 : " << numberOfGeometricType << endl;
  assert(numberOf == myField->getNumberOfValues() );

  // S'il existe des profils, je récupère la liste des numéros d'éléments
  // pour tous les types géométriques
  for (int i=0; i < numberOfGeometricType; i++) {
    cout << "Number Of Elements on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbOfElements[i] << endl;
    cout << "Number Of Gauss Points on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbGaussPoints[i] << endl;
    cout << "Localization description : " << endl << myField->getGaussLocalization(typeList[i]) << endl;
  }

  // On récupère la liste complète
  if (!onAll) number = mySupport->getNumber(MED_ALL_ELEMENTS);

  int elNo = -1;
  cout << "- Valeurs :" << endl;
  for (int i=1; i<=numberOf; i++) {
    if (onAll) elNo = i; else elNo = number[i-1];
    //cout << endl << "myField->getRow("<<elNo<<") : "<< myField->getRow(elNo) << endl;
    value = myField->getRow(elNo) ;
    //UP: getRow prend un numéro d'élément qui existe, getRow(1) n'existe pas forcément si il y a un profil
    //qui ne défini pas cet indice
    //cout << endl << " Valeur de getNbGaussI("<<elNo<<") :" << myField->getNbGaussI(elNo) << endl;
    for (int j=0; j<numberOfComponents*myField->getNbGaussI(elNo); j++)
      //UP : Prend en compte le nombre de points de Gauss de l'élément elNo
      cout << "value["<< elNo << "] = " << value[j] << " ";
    cout<<endl;
  }

}

// Spécialisation du traitement pour le mode NoInterlace
// Spécifité de traitement par rapport aux profils (utilisation du SUPPORT)
// Pas de spécificité concernant le type géométrique
// Pas de spécificité concernant les points de Gauss
template <>
void affiche_fieldT(FIELD<double, NoInterlace> * myField)
{
  const double * value = 0;
  const int    * number = 0;

  affiche_field_((FIELD_ *) myField);
  const SUPPORT * mySupport=myField->getSupport();

  cout.setf(ios::fixed);


  int numberOfComponents    = myField->getNumberOfComponents() ;
  int valueLength           = myField->getValueLength();
  const int * nbGaussPoints = myField->getNumberOfGaussPoints();

  int numberOf              = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int numberOfGeometricType = mySupport->getNumberOfTypes();
  const int * nbOfElements  = mySupport->getNumberOfElements();
  const MED_EN::medGeometryElement * typeList      = mySupport->getTypes();
  bool  onAll               = mySupport->isOnAllElements();

  cout << "mySupport->getValueLength           (MED_ALL_ELEMENTS) : " << valueLength << endl;
  cout << "mySupport->getNumberOfElements      (MED_ALL_ELEMENTS) : " << numberOf   << endl;
  cout << "mySupport->getNumberOfComponents    ()                 : " << numberOfComponents << endl;
  cout << "mySupport->getNumberOfGeometricType ()                 : " << numberOfGeometricType << endl;
  cout << "mySupport->getNumberOfElements(MED_ALL_ELEMENTS)       : " << numberOf << endl;
  assert(numberOf == myField->getNumberOfValues() );

  // S'il existe des profils, je récupère la liste des numéros d'éléments
  // pour tous les types géométriques
  for (int i=0; i < numberOfGeometricType; i++) {
    cout << "Number Of Elements on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbOfElements[i] << endl;
    cout << "Number Of Gauss Points on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbGaussPoints[i] << endl;
    cout << "Localization description : " << endl << myField->getGaussLocalization(typeList[i]) << endl;
  }


  int (* fct)(int,const int *);

  if (!onAll) {
    number = mySupport->getNumber(MED_ALL_ELEMENTS);
    fct=fct1;
  } else
    fct=fct2;

  int oneDimlength = valueLength/numberOfComponents;
  for (int j=1; j<=numberOfComponents; j++) {
    value = myField->getColumn(j) ;
    for (int i=0; i<oneDimlength; i++)
      cout << "value["<< fct(i,number) << ","<<j<<"]" << value[ fct(i,number) ] << " ";
    cout<<endl;
  }

}


template <class T, class INTERLACING_TAG>
void affiche_fieldT2(FIELD< T,  INTERLACING_TAG> * myField)
{}


// Spécialisation du traitement pour le mode FullInterlace
// Spécifité de traitement par rapport aux profils (utilisation du SUPPORT)
// Spécificité concernant le type géométrique
// Spécificité concernant les points de Gauss
template <>
void affiche_fieldT2(FIELD<double, FullInterlace> * myField)
{
  const int    * number = 0;

  affiche_field_((FIELD_ *) myField);
  const SUPPORT * mySupport=myField->getSupport();

  cout.setf(ios::fixed);


  int numberOfComponents    = myField->getNumberOfComponents() ;
  int valueLength           = myField->getValueLength();
  const int * nbGaussPoints = myField->getNumberOfGaussPoints();

  int numberOf              = mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  int numberOfGeometricType = mySupport->getNumberOfTypes();
  const int * nbOfElements  = mySupport->getNumberOfElements();
  const MED_EN::medGeometryElement * typeList      = mySupport->getTypes();
  bool  onAll               = mySupport->isOnAllElements();

  cout << "mySupport->getValueLength           (MED_ALL_ELEMENTS) : " << valueLength << endl;
  cout << "mySupport->getNumberOfElements      (MED_ALL_ELEMENTS) : " << numberOf   << endl;
  cout << "mySupport->getNumberOfComponents    ()                 : " << numberOfComponents << endl;
  cout << "mySupport->getNumberOfGeometricType ()                 : " << numberOfGeometricType << endl;
  cout << "mySupport->getNumberOfElements(MED_ALL_ELEMENTS)       : " << numberOf << endl;
  assert(numberOf == myField->getNumberOfValues() );

  // S'il existe des profils, je récupère la liste des numéros d'éléments
  // pour tous les types géométriques
  for (int i=0; i < numberOfGeometricType; i++) {
    cout << "Number Of Elements on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbOfElements[i] << endl;
    cout << "Number Of Gauss Points on type "<< MED_EN::geoNames[typeList[i]]
         <<" : "<< nbGaussPoints[i] << endl;
  }


  int (* fct)(int,const int *);

  if (!onAll) {
    number = mySupport->getNumber(MED_ALL_ELEMENTS);
    fct=fct1;
  } else
    fct=fct2;

  cout << "- Valeurs :"<<endl;

  int elemno = 1;
  for (int ntyp=1; ntyp <= numberOfGeometricType; ntyp++ ) {
    for (int  i=0; i < nbOfElements[ntyp-1] ; i++ ) {
      for (int k=1; k <= nbGaussPoints[ntyp-1]; k++)
        for (int j=1; j <= numberOfComponents; j++) {
          cout << " value["<< fct(elemno-1,number) << "," <<j<<","<<k<<"] = "
          << myField->getValueIJK(fct(elemno-1,number),j,k);
        }
      elemno++;
      cout << endl;
    }
  }

  assert((elemno-1) == numberOf);

}


int main (int argc, char ** argv) {

  if ((argc !=3) && (argc != 5)) {
    cerr << "Usage : " << argv[0]
         << " filename fieldName [iterationNumber] [orderNumber]" << endl << endl;
    exit(-1);
  }

  string fileName  = argv[1] ;
  string fieldName = argv[2] ;
  int  iterationNumber=-1,orderNumber=-1;
  if ( argv[3] )  iterationNumber = atoi(argv[3]);
  if ( argv[4] )  orderNumber     = atoi(argv[4]);

  string meshName="";//"MAILTRQU";
  //goto mode2;
  /////////////////////////////////////////////////////////////////////////////////////////
    // TEST PREMIER MODE :
    // Le fichier MED lu contient le maillage associé au champ demandé (qui contient des profils )
    // Le driver du FIELD automatiquement crée est capable de lire les profils MEDFICHIER
    // (le SUPPORT est crée automatiquement, le nom du maillage associé est disponible dans
    // le SUPPORT mais la relation SUPPORT-MESH est non initialisée car le MESH n'est pas chargé).
    // Le driver utilise les informations du maillage dans le fichier pour transcrire les profils
    // de la numérotation locale MEDFICHIER à la numérotation globale MEDMEMOIRE).
    // A l'écriture, il se repose également sur le maillage contenu dans le fichier
    // pour effecuter la renumérotation.
    {
      FIELD<double,INTERLACING_MODE> * myField1  = new FIELD<double,INTERLACING_MODE>(MED_DRIVER,fileName,fieldName,
                                                                                      iterationNumber, orderNumber);
      affiche_fieldT(myField1);
      cout << endl;
      affiche_fieldT2(myField1);

      // Pour éviter de modifier le fichier d'origine,
      // on en crée une copie avec uniquement le maillage.
      // Rem : Pour le test, le chargement du maillage n'est pas nécessaire
      //       On pourrait réécrire le Champ dans le fichier d'origine
      //       sous un autre nom.
      // Attention si le driver MED_MESH modifie le nombre d'éléments d'un type géométrique :
      // le calcul de renumérotation à l'écriture du champ risque d'être faux !
      meshName = myField1->getSupport()->getMeshName();
      MESH * myMesh = new MESH(MED_DRIVER,fileName,meshName);
      MED_MESH_WRONLY_DRIVER myMeshDriver1("Copy_withmesh_"+fileName,myMesh);
      int current=myMesh->addDriver(myMeshDriver1);
      myMesh->write(current);
      myMesh->removeReference();

      // On ajoute un driver en écriture, comme la relation SUPPORT-MESH n'est pas
      // initialisée, le driver doit trouver le maillage dans le fichier cible
      // pour réaliser la transcription des profils MEDMEMOIRE à MEDFICHIER.
      MED_FIELD_WRONLY_DRIVER<double> myFieldDriver2("Copy_withmesh_"+fileName,myField1) ;
      current = myField1->addDriver(myFieldDriver2);
      myField1->write(current);
      myField1->removeReference();
    }

//  mode2:
//     /////////////////////////////////////////////////////////////////////////////
//       // TEST DEUXIEME MODE :
//       // Lecture idem 1er mode
//       // A l'écriture, le fichier cible ne contient pas le maillage mais la
//       // relation SUPPORT-MESH est établie, le driver peut donc utiliser les informations
//       // dans le maillage pour transcrire les profils.
//       // Selon le modèle MED FICHIER, ce mode est interdit : le fichier doit au moins
//       // contenir un lien sur le maillage (information pas encore exploitée dans MEDMEMOIRE
//       // : pas de gestion de montage/démontage des fichiers )
//       // Attention si le driver MED_MESH modifie le nombre d'éléments d'un type géométrique :
//       // le calcul de renumérotation à l'écriture du champ risque d'être faux car les
//       // profils crées à la lecture sont basés sur le nombre d'éléments par type géoémtrique
//       // du maillage contenu dans le fichier à la lecture.
//       // Une solution consisterait à prendre en compte le montage de fichiers distants
//       // et de prendre en compte la différence de nombre d'éléments par type géométrique
//       // entre le maillage MEDMEM et le maillage MEDFICHIER
//       // (Hum ! : Il serait plus simple que MEDMEMOIRE ne recalcule pas systématiquement
//       //  ce que l'on ne lui demande pas, ce qui permettrait aussi de réécrire à l'identique
//       // un fichier que l'on vient de lire)
//    {
//      FIELD<double,INTERLACING_MODE> * myField2  = new FIELD<double,INTERLACING_MODE>(MED_DRIVER,fileName,fieldName,
//                                                                                   iterationNumber, orderNumber);

//      meshName = myField2->getSupport()->getMeshName();
//      MESH * myMesh2 = new MESH(MED_DRIVER,fileName,meshName);

//      const SUPPORT * mySupport2=myField2->getSupport();
//      mySupport2->setMesh(myMesh2);

//      // On ajoute un driver en écriture, comme la relation SUPPORT-MESH est
//      // initialisée, le driver utilise le maillage en mémoire
//      // pour réaliser la transcription des profils MEDMEMOIRE à MEDFICHIER.
//      MED_FIELD_WRONLY_DRIVER<double> myFieldDriver3("Copy_nomesh_"+fileName,myField2) ;
//      int current = myField2->addDriver(myFieldDriver3);
//      myField2->write(current);

//      //Pour regarder le fichier produit avec MDUMP decommenter ces trois lignes
//      //car le fichier qui est produit n'est pas à la norme MED
//      //Il doit contenir soit le maillage associé soit un lien vers le maillage associé.
//      //MED_MESH_WRONLY_DRIVER myMeshDriver2("Copy_nomesh_"+fileName,myMesh2);
//      //current=myMesh2->addDriver(myMeshDriver2);
//      //myMesh2->write(current);

//      delete myField2;
//      delete myMesh2;


//    }
//  mode3:
//    // TEST TROISIEME MODE  :
//    // A la lecture, le fichier MED lu ne contient pas le maillage  associé au champ demandé
//    // (mais un lien MEDFICHIER qui n'est pas exploité à ce jour).
//    // Cependant avant sa lecture le FIELD a été associé à un SUPPORT
//    // avec le lien au MESH préalablement chargé.
//    // Le driver du FIELD (automatiquement crée) est capable de lire les profils MEDFICHIER
//    //   et utilise la relation SUPPORT-MESH initialisée pour transcrire les profils
//    //   de la numérotation locale MEDFICHIER à la numérotation globale MEDMEMOIRE).
//    // REM: Une fois le champ chargé, il possède un nouveau support, le premier peut être libéré.
//    //      En effet le driver du FIELD se fit uniquement au type géométriques définis dans le champ MEDFICHIER
//    //      pour créer son SUPPORT car un SUPPORT crée "onAll" à partir d'un MESH repose sur tous
//    //      les types géométriques du MESH ce qui n'est pas forcément le cas d'un champ MEDFICHIER
//    //      (même sans profil) lu à posteriori.
//    {
//      med_2_3::med_err err=-1;
//      med_2_3::med_idt id = med_2_3::MEDfileOpen(const_cast<char *> ( ("Copy_nomesh_"+fileName).c_str()),
//                                            med_2_3::MED_ACC_RDWR);
//      if (id <=0) cout << "Erreur dans MEDouvrir pour le fichier " << "Copy_nomesh_"+fileName <<endl;

//      err=med_2_3::MEDlienEcr(id, const_cast<char *> ( ("Copy_withmesh_"+fileName).c_str()),
//                           const_cast<char *> (meshName.c_str()) );
//      if (err !=0) cout << "Erreur dans MEDlienEcr pour le maillage distant " << meshName
//                     <<" contenu dans le fichier " << "Copy_withmesh_"+fileName <<endl;


//      err=med_2_3::MEDfermer(id);

//      MESH * myMesh3 = new MESH(MED_DRIVER,fileName,meshName);
//      const SUPPORT * mySupport3= new SUPPORT(myMesh3,"Temporary Support",MED_CELL);
//      FIELD<double,INTERLACING_MODE> * myField3  = new FIELD<double,INTERLACING_MODE>(mySupport3,MED_DRIVER,"Copy_nomesh_"+fileName,fieldName, iterationNumber, orderNumber);
//      delete mySupport3; // Il est déjà possible de libérer ce SUPPORT

//      //TEST à la réecriture (renommage des profils
//      // à cause de MEDprofilEcr qui ne prend pas en compte le mode
//      // MED_LECTURE_AJOUT) ):
//      string cpy("__Copy");
//      vector < string > pflNames = myField3->getSupport()->getProfilNames();
//      for (int i=0; i< pflNames.size(); ++i) {
//        pflNames[i].resize(pflNames[i].size()-cpy.size());
//        pflNames[i]+=cpy;
//      }
//      const_cast<SUPPORT*>(myField3->getSupport())->setProfilNames(pflNames);

//      MED_FIELD_WRONLY_DRIVER<double> myFieldDriver4("Copy_nomesh_"+fileName,myField3) ;
//      myFieldDriver4.setFieldName(myField3->getName()+"__Copy");
//      int current = myField3->addDriver(myFieldDriver4);
//      myField3->write(current);

//      delete myMesh3;
//      delete myField3;


//      //ESSAYER AVEC MAILLAGE DS FICHIER ET LIEN SUPORT-MESH PRESENTS SIMULTANEMENT
//      //EN VERSION COHERENTE ET NON COHERENTE
//    }
}
