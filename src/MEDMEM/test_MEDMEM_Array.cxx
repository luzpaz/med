using namespace std;
#include "utilities.h"
#include "MEDMEM_Array.hxx"

int main (int argc, char ** argv) {

  int SpaceDimension = 3 ;
  int NumberOfNodes = 4 ; 
  MEDARRAY<int> * myArray = new MEDARRAY<int>(SpaceDimension,NumberOfNodes,MED_FULL_INTERLACE) ;

  //const int * value = myArray->get(MED_FULL_INTERLACE) ;
  for (int i=1; i<=NumberOfNodes; i++)
    for (int j=1; j<=SpaceDimension; j++)
      myArray->setIJ(i,j,i) ;
  
  int numberof ;
  MESSAGE("Show all 1 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    //int * node = myArray->getI(MED_FULL_INTERLACE,i) ;
    const int * node = myArray->getRow(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE("Show all 2 :");
  numberof = myArray->getLengthValue() ;
  for (int i=1; i<=myArray->getLeadingValue() ; i++) {
    //int * node = myArray->getJ(MED_NO_INTERLACE,i) ;
    const int * node = myArray->getColumn(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE("Show all 3 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    cout << " - " ;
    for (int j=1;j<numberof+1;j++)
      cout << myArray->getIJ(i,j) << " " ;
    cout << endl ;
  }

  MESSAGE("Show all 0 :");
  numberof = myArray->getLeadingValue() ;
  int length = myArray->getLengthValue() ;
  const int * NoInterlaceArray = myArray->get(MED_NO_INTERLACE) ;
  for (int i=0; i<length ; i++) {
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << NoInterlaceArray[j*length+i] << " " ;
    cout << endl ;
  }

  delete myArray ;

  return 0 ;
}
