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
/* Programme de test du constructeur de copies de la classe MEDARRAY de MEDMEM
   jroy - 16/12/2002 */

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

static void affiche_medarray(MEDARRAY<double> & myMedArray)
{
  int numberof ;
  MESSAGE_MED("Show all 1 :");
  numberof = myMedArray.getLeadingValue() ;
  for (int i=1; i<=myMedArray.getLengthValue() ; i++) {
    const double * node = myMedArray.getRow(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE_MED("Show all 2 :");
  numberof = myMedArray.getLengthValue() ;
  for (int i=1; i<=myMedArray.getLeadingValue() ; i++) {
    const double * node = myMedArray.getColumn(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE_MED("Show all 3 :");
  numberof = myMedArray.getLeadingValue() ;
  for (int i=1; i<=myMedArray.getLengthValue() ; i++) {
    cout << " - " ;
    for (int j=1;j<numberof+1;j++)
      cout << myMedArray.getIJ(i,j) << " " ;
    cout << endl ;
  }

  MESSAGE_MED("Show all 0 :");
  numberof = myMedArray.getLeadingValue() ;
  int length = myMedArray.getLengthValue() ;
  const double * NoInterlaceArray = myMedArray.get(MED_NO_INTERLACE) ;
  for (int i=0; i<length ; i++) {
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << NoInterlaceArray[j*length+i] << " " ;
    cout << endl ;
  }


  /*
  cout << "Nombre de lignes : " << myMedArray.getLengthValue() << endl;
  cout << "Nombre de colonnes : " << myMedArray.getLeadingValue() << endl;
  cout << "Mode d'écriture en mémoire : " << myMedArray.getMode() << endl;
  cout << "Valeurs (_valuesDefault) :" << endl;

  for (med_int i=1; i<=myMedArray.getLengthValue(); i++) {
    for (med_int j=1; j<=myMedArray.getLeadingValue(); j++)
      cout << i << "  " << j << "    " << myMedArray.getIJ(i,j) << " " << endl;
  }
  */
}


int main (int argc, char ** argv) {
  /*
  if ((argc !=3) && (argc != 4)) {
    cerr << "Usage : " << argv[0] 
         << " filename meshname fieldname" << endl << endl;
    exit(-1);
  }
  */
  const med_int nb_noeuds = 8;
  const med_int dimension = 3;
  const medModeSwitch mode = MED_NO_INTERLACE;


  MEDARRAY<double> * myMedArray = new MEDARRAY<double>(dimension,nb_noeuds,mode);
  for (med_int i=1; i<=myMedArray->getLengthValue(); i++) {
    for (med_int j=1; j<=myMedArray->getLeadingValue(); j++)
      myMedArray->setIJ(i,j,(double) rand());
  };

  affiche_medarray(* myMedArray);
  MEDARRAY<double> * myMedArray2 = new MEDARRAY<double>();
  * myMedArray2 = * myMedArray;
  //  delete myMedArray;  // si on recopie les tableaux
  affiche_medarray(* myMedArray2);
  delete myMedArray;      // si on ne recopie pas les tableaux
  delete myMedArray2;

  return 0;
}
