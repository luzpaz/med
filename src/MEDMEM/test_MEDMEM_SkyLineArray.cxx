#include "utilities.h"
#include "MEDMEM_SkyLineArray.hxx"
using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv) {

  int NumberOfCell = 3 ; // 1 triangle,1 quadrangle,1 triangle
  int Size = 10 ; // 10 nodes

//CCRT  int * index = new int[NumberOfCell+1] ;
  med_int * index = new med_int[NumberOfCell+1] ;
  index[0]=1;
  index[1]=4;
  index[2]=8;
  index[3]=11;
//CCRT  int * value = new int[Size] ;
  med_int * value = new med_int[Size] ;
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

  MEDSKYLINEARRAY * myArray = new MEDSKYLINEARRAY(NumberOfCell,Size,index,value) ;

  cout << "Show all 1 :" << endl ;
  for (int i=1; i<NumberOfCell+1 ; i++) {
//CCRT    const int * cell = myArray->getI(i) ;
    const med_int * cell = myArray->getI(i) ;
    int numberof = myArray->getNumberOfI(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << cell[j] << " " ;
    cout << endl ;
  }
  cout << "Show all 2 :" << endl ;
  for (int i=1; i<NumberOfCell+1 ; i++) {
    cout << " - " ;
    int numberof = myArray->getNumberOfI(i) ;
    for (int j=1;j<numberof+1;j++)
      cout << myArray->getIJ(i,j) << " " ;
    cout << endl ;
  }

  MEDSKYLINEARRAY * myArray2 = new MEDSKYLINEARRAY(*myArray) ;
  delete myArray ;
  
  cout << "Show all 3 :" << endl ;
//CCRT  const int * index2 = myArray2->getIndex() ;
  const med_int * index2 = myArray2->getIndex() ;
  for (int i=1; i<=NumberOfCell ; i++) {
    cout << " - " ;
    for (int j=index2[i-1];j<index2[i];j++)
      cout << myArray2->getIndexValue(j) << " " ;
    cout << endl ;
  }

  delete myArray2 ;
  delete[] index ;
  delete[] value ;

  return 0 ;
}
  
