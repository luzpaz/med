#include "utilities.h"
#include "MEDMEM_SkyLineArray.hxx"

int main (int argc, char ** argv) {

  int NumberOfCell = 3 ; // 1 triangle,1 quadrangle,1 triangle
  int Size = 10 ; // 10 nodes
  MEDSKYLINEARRAY * myArray = new MEDSKYLINEARRAY(NumberOfCell,Size) ;

  int * index = myArray->getIndex() ;
  index[0]=1;
  index[1]=4;
  index[2]=8;
  index[3]=11;
  int * value = myArray->getValue() ;
  value[0]=1; // first
  value[1]=2;
  value[2]=5;
  value[3]=2; // second
  value[4]=3;
  value[5]=5;
  value[6]=6;
  value[7]=3; // thirst
  value[8]=4;
  value[9]=6;
  //  value[]=; // forth

  MESSAGE( "Show all 1 :" );
  for (int i=1; i<NumberOfCell+1 ; i++) {
    int * cell = myArray->getI(i) ;
    int numberof = myArray->getNumberOfI(i) ;
    MESSAGE( " - " );
    for (int j=0;j<numberof;j++)
      MESSAGE( cell[j] << " " );
    MESSAGE("");
  }
  MESSAGE( "Show all 2 :" );
  for (int i=1; i<NumberOfCell+1 ; i++) {
    MESSAGE( " - " );
    int numberof = myArray->getNumberOfI(i) ;
    for (int j=1;j<numberof+1;j++)
      MESSAGE( myArray->getIJ(i,j) << " " );
    MESSAGE("");
  }

  MEDSKYLINEARRAY * myArray2 = new MEDSKYLINEARRAY(*myArray) ;
  delete myArray ;
  
  MESSAGE( "Show all 3 :" );
  for (int i=1; i<NumberOfCell+1 ; i++) {
    int * cell = myArray2->getI(i) ;
    int numberof = myArray2->getNumberOfI(i) ;
    MESSAGE( " - " );
    for (int j=0;j<numberof;j++)
      MESSAGE( cell[j] << " " );
    MESSAGE("");
  }

  return 0 ;
}
  
