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
/* Programme de test du constructeur de copies de la classe COORDINATE de MEDMEM
   jroy - 17/12/2002 */

#include <string>

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

static void affiche_tableau(const double * myArray, int nb_lignes, int nb_colonnes)
{
  cout << "Nombre de lignes : " << nb_lignes << endl;
  cout << "Nombre de colonnes : " << nb_colonnes << endl;
  cout << "Valeurs :" << endl;

  for (int i=0; i<nb_lignes; i++) {
    for (int j=0; j<nb_colonnes; j++)
      cout << myArray[j*nb_lignes+i] << " ";
    cout << endl;}
}

static void affiche_coordinate(COORDINATE & myCoordinate, int _numberofNodes, int _spaceDimension)
{
  cout << "- Système de coordonnées : " << myCoordinate.getCoordinatesSystem() << endl;
  cout << "- Tableau des coordonnées : " << endl;
  //  affiche_medarray(* myCoordinate.getCoordinatePtr());
  affiche_tableau(myCoordinate.getCoordinates(MED_NO_INTERLACE),_numberofNodes,_spaceDimension);
  //on récupère les dimensions
  //  int _spaceDimension = (int) myCoordinate.getCoordinatePtr()->getLeadingValue();
  //int _numberofNodes = (int) myCoordinate.getCoordinatePtr()->getLengthValue();
  cout << "- Nom des coordonnées : " << endl;
  for (int i=1; i<=_spaceDimension; i++)
    cout << i << "   " << myCoordinate.getCoordinateName(i) << endl;
  //  cout<<myCoordinate.getCoordinatesNames()<<endl;
  cout << "- Unité des coordonnées : " << endl;
  for (int i=1; i<=_spaceDimension; i++)
    cout << i << "   " << myCoordinate.getCoordinateUnit(i) << endl;
  cout << "- Indices des noeuds : " << endl;
  //  cout<<myCoordinate.getNodesNumbers()<<endl;
  for (int i=0; i<_numberofNodes; i++)
    cout << i << "   " << myCoordinate.getNodesNumbers()[i] << endl;
}


int main (int argc, char ** argv) {
  /*
  if ((argc !=3) && (argc != 4)) {
    cerr << "Usage : " << argv[0] 
         << " filename meshname fieldname" << endl << endl;
    exit(-1);
  }
  */
  const med_int numberofNodes = 5;
  const med_int spaceDimension = 3;
  const medModeSwitch mode = MED_FULL_INTERLACE;

  //construction tableau MEDARRAY des coordonnées
  MEDARRAY<double> * myMedArray = new MEDARRAY<double>(spaceDimension,numberofNodes,mode);
  for (med_int i=1; i<=myMedArray->getLengthValue(); i++) {
    for (med_int j=1; j<=myMedArray->getLeadingValue(); j++)
      myMedArray->setIJ(i,j,(double) rand());
  };

  //construction noms des coordonnées
  string * myCoordinatesNames = new string[spaceDimension];
  if (spaceDimension >= 1) myCoordinatesNames[0] = "x";
  if (spaceDimension >= 2) myCoordinatesNames[1] = "y";
  if (spaceDimension >= 3) myCoordinatesNames[2] = "z";

  //construction unités des coordonnées
  string * myCoordinatesUnits = new string[spaceDimension];
  if (spaceDimension >= 1) myCoordinatesUnits[0] = "m";
  if (spaceDimension >= 2) myCoordinatesUnits[1] = "m";
  if (spaceDimension >= 3) myCoordinatesUnits[2] = "m";

  //construction des indices des noeuds
  int * myNodeNumber = new int[numberofNodes];
  for (int i=0; i<numberofNodes; i++)
    myNodeNumber[i]=numberofNodes-i-1;

  //construction de l'objet COORDINATE
  COORDINATE * myCoordinate = new COORDINATE();
  myCoordinate->setCoordinates(myMedArray);
  myCoordinate->setCoordinatesNames(myCoordinatesNames);
  myCoordinate->setCoordinatesUnits(myCoordinatesUnits);
  myCoordinate->setNodesNumbers(myNodeNumber);

  //  myCoordinate->setCoordinatesNames((string *)NULL);
  //  myCoordinate->setNodesNumbers((int *) NULL);

  delete myMedArray ;
  delete[] myCoordinatesNames ;
  delete[] myCoordinatesUnits ;
  delete[] myNodeNumber ;

  affiche_coordinate(* myCoordinate,numberofNodes,spaceDimension);
  COORDINATE * myCoordinate2 = new COORDINATE(* myCoordinate);
  delete myCoordinate ;

  affiche_coordinate(* myCoordinate2,numberofNodes,spaceDimension);
  delete myCoordinate2 ;

  return 0;
}
