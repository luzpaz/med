#include "utilities.h"
#include "MEDMEM_Array.hxx"

int main (int argc, char ** argv) {

  int SpaceDimension = 3 ;
  int NumberOfNodes = 4 ; 
  MEDARRAY<int> * myArray = new MEDARRAY<int>(SpaceDimension,NumberOfNodes,MED_FULL_INTERLACE) ;

  int * value = myArray->get(MED_FULL_INTERLACE) ;
  for (int i=0; i<SpaceDimension*NumberOfNodes; i++)
    value[i]=i ; 
  
  int numberof ;
  MESSAGE("Show all 1 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    int * node = myArray->getI(MED_FULL_INTERLACE,i) ;
    MESSAGE( " - " );
    for (int j=0;j<numberof;j++)
      MESSAGE( node[j] << " " );
    MESSAGE("");
  }
  MESSAGE("Show all 2 :");
  numberof = myArray->getLengthValue() ;
  for (int i=1; i<=myArray->getLeadingValue() ; i++) {
    int * node = myArray->getI(MED_NO_INTERLACE,i) ;
    MESSAGE( " - " );
    for (int j=0;j<numberof;j++)
      MESSAGE( node[j] << " " );
    MESSAGE("");
  }
  MESSAGE("Show all 3 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    MESSAGE( " - " );
    for (int j=1;j<numberof+1;j++)
      MESSAGE( myArray->getIJ(i,j) << " " );
    MESSAGE("");
  }

  MESSAGE("Show all 0 :");
  numberof = myArray->getLeadingValue() ;
  int length = myArray->getLengthValue() ;
  int * NoInterlaceArray = myArray->get(MED_NO_INTERLACE) ;
  for (int i=0; i<length ; i++) {
    MESSAGE( " - " );
    for (int j=0;j<numberof;j++)
      MESSAGE( NoInterlaceArray[j*length+i] << " " );
    MESSAGE("");
  }
  return 0 ;
}
  
