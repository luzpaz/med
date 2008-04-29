// Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
//           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
int fct2(int i,const int * number) { return i;};
int fct1(int i,const int * number) { return number[i]; };

void affiche_field_(FIELD_ * myField)
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

// Cas de traitement des valeurs sans sp�cificit� concernant
// les points de Gauss ou les profils
// Pas de sp�cificit� concernant le type g�om�trique
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

// Sp�cialisation du traitement pour le mode FullInterlace
// Sp�cifit� de traitement par rapport aux profils (utilisation du SUPPORT)
// Pas de sp�cificit� concernant le type g�om�trique
// Pas de sp�cificit� concernant les points de Gauss
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

  // S'il existe des profils, je r�cup�re la liste des num�ros d'�l�ments
  // pour tous les types g�om�triques
  for (int i=0; i < numberOfGeometricType; i++) {
    cout << "Number Of Elements on type "<< MED_EN::geoNames[typeList[i]]
	 <<" : "<< nbOfElements[i] << endl;
    cout << "Number Of Gauss Points on type "<< MED_EN::geoNames[typeList[i]]
	 <<" : "<< nbGaussPoints[i] << endl;
    cout << "Localization description : " << endl << myField->getGaussLocalization(typeList[i]) << endl;
  }

  // On r�cup�re la liste compl�te
  if (!onAll) number = mySupport->getNumber(MED_ALL_ELEMENTS);

  int elNo = -1;
  cout << "- Valeurs :" << endl;
  for (int i=1; i<=numberOf; i++) {
    if (onAll) elNo = i; else elNo = number[i-1];
    //cout << endl << "myField->getRow("<<elNo<<") : "<< myField->getRow(elNo) << endl;
    value = myField->getRow(elNo) ;
    //UP: getRow prend un num�ro d'�l�ment qui existe, getRow(1) n'existe pas forc�ment si il y a un profil
    //qui ne d�fini pas cet indice
    //cout << endl << " Valeur de getNbGaussI("<<elNo<<") :" << myField->getNbGaussI(elNo) << endl;
    for (int j=0; j<numberOfComponents*myField->getNbGaussI(elNo); j++)
      //UP : Prend en compte le nombre de points de Gauss de l'�l�ment elNo
      cout << "value["<< elNo << "] = " << value[j] << " ";
    cout<<endl;
  }

}

// Sp�cialisation du traitement pour le mode NoInterlace
// Sp�cifit� de traitement par rapport aux profils (utilisation du SUPPORT)
// Pas de sp�cificit� concernant le type g�om�trique
// Pas de sp�cificit� concernant les points de Gauss
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

  // S'il existe des profils, je r�cup�re la liste des num�ros d'�l�ments
  // pour tous les types g�om�triques
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
{};


// Sp�cialisation du traitement pour le mode FullInterlace
// Sp�cifit� de traitement par rapport aux profils (utilisation du SUPPORT)
// Sp�cificit� concernant le type g�om�trique
// Sp�cificit� concernant les points de Gauss
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

  // S'il existe des profils, je r�cup�re la liste des num�ros d'�l�ments
  // pour tous les types g�om�triques
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
    // Le fichier MED lu contient le maillage associ� au champ demand� (qui contient des profils )
    // Le driver du FIELD automatiquement cr�e est capable de lire les profils MEDFICHIER
    // (le SUPPORT est cr�e automatiquement, le nom du maillage associ� est disponible dans
    // le SUPPORT mais la relation SUPPORT-MESH est non initialis�e car le MESH n'est pas charg�).
    // Le driver utilise les informations du maillage dans le fichier pour transcrire les profils
    // de la num�rotation locale MEDFICHIER � la num�rotation globale MEDMEMOIRE).
    // A l'�criture, il se repose �galement sur le maillage contenu dans le fichier
    // pour effecuter la renum�rotation.
    {
      FIELD<double,INTERLACING_MODE> * myField1  = new FIELD<double,INTERLACING_MODE>(MED_DRIVER,fileName,fieldName,
										      iterationNumber, orderNumber);
      affiche_fieldT(myField1);
      cout << endl;
      affiche_fieldT2(myField1);

      // Pour �viter de modifier le fichier d'origine,
      // on en cr�e une copie avec uniquement le maillage.
      // Rem : Pour le test, le chargement du maillage n'est pas n�cessaire
      //       On pourrait r��crire le Champ dans le fichier d'origine
      //       sous un autre nom.
      // Attention si le driver MED_MESH modifie le nombre d'�l�ments d'un type g�om�trique :
      // le calcul de renum�rotation � l'�criture du champ risque d'�tre faux !
      meshName = myField1->getSupport()->getMeshName();
      MESH * myMesh = new MESH(MED_DRIVER,fileName,meshName);
      MED_MESH_WRONLY_DRIVER myMeshDriver1("Copy_withmesh_"+fileName,myMesh);
      int current=myMesh->addDriver(myMeshDriver1);
      myMesh->write(current);
      delete myMesh;

      // On ajoute un driver en �criture, comme la relation SUPPORT-MESH n'est pas
      // initialis�e, le driver doit trouver le maillage dans le fichier cible
      // pour r�aliser la transcription des profils MEDMEMOIRE � MEDFICHIER.
      MED_FIELD_WRONLY_DRIVER<double> myFieldDriver2("Copy_withmesh_"+fileName,myField1) ;
      current = myField1->addDriver(myFieldDriver2);
      myField1->write(current);
      delete myField1;
    }

 mode2:
    /////////////////////////////////////////////////////////////////////////////
      // TEST DEUXIEME MODE :
      // Lecture idem 1er mode
      // A l'�criture, le fichier cible ne contient pas le maillage mais la
      // relation SUPPORT-MESH est �tablie, le driver peut donc utiliser les informations
      // dans le maillage pour transcrire les profils.
      // Selon le mod�le MED FICHIER, ce mode est interdit : le fichier doit au moins
      // contenir un lien sur le maillage (information pas encore exploit�e dans MEDMEMOIRE
      // : pas de gestion de montage/d�montage des fichiers )
      // Attention si le driver MED_MESH modifie le nombre d'�l�ments d'un type g�om�trique :
      // le calcul de renum�rotation � l'�criture du champ risque d'�tre faux car les
      // profils cr�es � la lecture sont bas�s sur le nombre d'�l�ments par type g�o�mtrique
      // du maillage contenu dans le fichier � la lecture.
      // Une solution consisterait � prendre en compte le montage de fichiers distants
      // et de prendre en compte la diff�rence de nombre d'�l�ments par type g�om�trique
      // entre le maillage MEDMEM et le maillage MEDFICHIER
      // (Hum ! : Il serait plus simple que MEDMEMOIRE ne recalcule pas syst�matiquement
      //  ce que l'on ne lui demande pas, ce qui permettrait aussi de r��crire � l'identique
      // un fichier que l'on vient de lire)
   {
     FIELD<double,INTERLACING_MODE> * myField2  = new FIELD<double,INTERLACING_MODE>(MED_DRIVER,fileName,fieldName,
										     iterationNumber, orderNumber);

     meshName = myField2->getSupport()->getMeshName();
     MESH * myMesh2 = new MESH(MED_DRIVER,fileName,meshName);

     const SUPPORT * mySupport2=myField2->getSupport();
     mySupport2->setMesh(myMesh2);

     // On ajoute un driver en �criture, comme la relation SUPPORT-MESH est
     // initialis�e, le driver utilise le maillage en m�moire
     // pour r�aliser la transcription des profils MEDMEMOIRE � MEDFICHIER.
     MED_FIELD_WRONLY_DRIVER<double> myFieldDriver3("Copy_nomesh_"+fileName,myField2) ;
     int current = myField2->addDriver(myFieldDriver3);
     myField2->write(current);

     //Pour regarder le fichier produit avec MDUMP decommenter ces trois lignes
     //car le fichier qui est produit n'est pas � la norme MED
     //Il doit contenir soit le maillage associ� soit un lien vers le maillage associ�.
     //MED_MESH_WRONLY_DRIVER myMeshDriver2("Copy_nomesh_"+fileName,myMesh2);
     //current=myMesh2->addDriver(myMeshDriver2);
     //myMesh2->write(current);

     delete myField2;
     delete myMesh2;


   }
 mode3:
   // TEST TROISIEME MODE  :
   // A la lecture, le fichier MED lu ne contient pas le maillage  associ� au champ demand�
   // (mais un lien MEDFICHIER qui n'est pas exploit� � ce jour).
   // Cependant avant sa lecture le FIELD a �t� associ� � un SUPPORT
   // avec le lien au MESH pr�alablement charg�.
   // Le driver du FIELD (automatiquement cr�e) est capable de lire les profils MEDFICHIER
   //   et utilise la relation SUPPORT-MESH initialis�e pour transcrire les profils
   //   de la num�rotation locale MEDFICHIER � la num�rotation globale MEDMEMOIRE).
   // REM: Une fois le champ charg�, il poss�de un nouveau support, le premier peut �tre lib�r�.
   //      En effet le driver du FIELD se fit uniquement au type g�om�triques d�finis dans le champ MEDFICHIER
   //      pour cr�er son SUPPORT car un SUPPORT cr�e "onAll" � partir d'un MESH repose sur tous
   //      les types g�om�triques du MESH ce qui n'est pas forc�ment le cas d'un champ MEDFICHIER
   //      (m�me sans profil) lu � posteriori.
   {
     med_2_3::med_err err=-1;
     med_2_3::med_idt id = med_2_3::MEDouvrir(const_cast<char *> ( ("Copy_nomesh_"+fileName).c_str()),
					      med_2_3::MED_LECTURE_ECRITURE);
     if (id <=0) cout << "Erreur dans MEDouvrir pour le fichier " << "Copy_nomesh_"+fileName <<endl;

     err=med_2_3::MEDlienEcr(id, const_cast<char *> ( ("Copy_withmesh_"+fileName).c_str()),
			     const_cast<char *> (meshName.c_str()) );
     if (err !=0) cout << "Erreur dans MEDlienEcr pour le maillage distant " << meshName
		       <<" contenu dans le fichier " << "Copy_withmesh_"+fileName <<endl;


     err=med_2_3::MEDfermer(id);

     MESH * myMesh3 = new MESH(MED_DRIVER,fileName,meshName);
     const SUPPORT * mySupport3= new SUPPORT(myMesh3,"Temporary Support",MED_CELL);
     FIELD<double,INTERLACING_MODE> * myField3  = new FIELD<double,INTERLACING_MODE>(mySupport3,MED_DRIVER,"Copy_nomesh_"+fileName,fieldName, iterationNumber, orderNumber);
     delete mySupport3; // Il est d�j� possible de lib�rer ce SUPPORT

     //TEST � la r�ecriture (renommage des profils
     // � cause de MEDprofilEcr qui ne prend pas en compte le mode
     // MED_LECTURE_AJOUT) ):
     string cpy("__Copy");
     vector < string > pflNames = myField3->getSupport()->getProfilNames();
     for (int i=0; i< pflNames.size(); ++i) {
       pflNames[i].resize(pflNames[i].size()-cpy.size());
       pflNames[i]+=cpy;
     }
     const_cast<SUPPORT*>(myField3->getSupport())->setProfilNames(pflNames);

     MED_FIELD_WRONLY_DRIVER<double> myFieldDriver4("Copy_nomesh_"+fileName,myField3) ;
     myFieldDriver4.setFieldName(myField3->getName()+"__Copy");
     int current = myField3->addDriver(myFieldDriver4);
     myField3->write(current);

     delete myMesh3;
     delete myField3;


     //ESSAYER AVEC MAILLAGE DS FICHIER ET LIEN SUPORT-MESH PRESENTS SIMULTANEMENT
     //EN VERSION COHERENTE ET NON COHERENTE
   }
}
