using namespace std;
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


void affiche_medarray(MEDARRAY<double> & myMedArray)
{
  int numberof ;
  MESSAGE("Show all 1 :");
  numberof = myMedArray.getLeadingValue() ;
  for (int i=1; i<=myMedArray.getLengthValue() ; i++) {
    double * node = myMedArray.getI(MED_FULL_INTERLACE,i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE("Show all 2 :");
  numberof = myMedArray.getLengthValue() ;
  for (int i=1; i<=myMedArray.getLeadingValue() ; i++) {
    double * node = myMedArray.getI(MED_NO_INTERLACE,i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE("Show all 3 :");
  numberof = myMedArray.getLeadingValue() ;
  for (int i=1; i<=myMedArray.getLengthValue() ; i++) {
    cout << " - " ;
    for (int j=1;j<numberof+1;j++)
      cout << myMedArray.getIJ(i,j) << " " ;
    cout << endl ;
  }

  MESSAGE("Show all 0 :");
  numberof = myMedArray.getLeadingValue() ;
  int length = myMedArray.getLengthValue() ;
  double * NoInterlaceArray = myMedArray.get(MED_NO_INTERLACE) ;
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
      myMedArray->setIJ(i,j,(double) random());
  };

  affiche_medarray(* myMedArray);
  MEDARRAY<double> * myMedArray2 = new MEDARRAY<double>(* myMedArray);
  delete myMedArray;
  affiche_medarray(* myMedArray2);
  delete myMedArray2;

  return 0;
}
