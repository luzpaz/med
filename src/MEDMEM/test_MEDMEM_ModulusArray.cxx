#include "utilities.h"
#include "MEDMEM_ModulusArray.hxx"

int main (int argc, char ** argv) {

  int array[5]={0,1,2,1,4} ;
  MEDMODULUSARRAY modulusArray(5,array);

  MESSAGE("ModuloArray :");
  for(int i=-10;i<15;i++)
    MESSAGE("  - A["<<i<<"]="<<modulusArray[i]);
  MESSAGE("");

  // test compare
  int ret ;

  int array2[5]={1,4,0,1,2} ;
  MEDMODULUSARRAY modulusArray2(5,array2) ;
  MESSAGE("Same arrays, same order ... ");
  if ((ret=modulusArray2.compare(modulusArray))!=1)
    MESSAGE( "Error : two arrays must be identical !"<<ret)
  else
    MESSAGE( "OK" )

  int array3[5]={1,2,1,0,4} ;
  MEDMODULUSARRAY modulusArray3(5,array3) ;
  MESSAGE("Same arrays, reverse order ... ");
  if ((ret=modulusArray3.compare(modulusArray))!=-1)
    MESSAGE( "Error : two arrays are in reverse order !"<<ret)
  else
    MESSAGE( "OK" )

  int array4[6]={1,2,1,0} ;
  MEDMODULUSARRAY modulusArray4(4,array4) ;
  MESSAGE("Different arrays size ... ");
  if ((ret=modulusArray4.compare(modulusArray))!=0)
    MESSAGE( "Error : two arrays have different size !"<<ret)
  else
    MESSAGE( "OK" )
  
  int array5[5]={1,2,1,0,1} ;
  MEDMODULUSARRAY modulusArray5(5,array5) ;
  MESSAGE("Different array, same size ... ");
  if ((ret=modulusArray5.compare(modulusArray))!=0)
    MESSAGE( "Error : two arrays are not be identical !"<<ret)
  else
    MESSAGE( "OK" )
 
  // test small array :

  // 1
  int array6[1]={1} ;
  MEDMODULUSARRAY modulusArray6(1,array6);

  MESSAGE("ModuloArray 1 :");
  for(int i=-10;i<15;i++)
    MESSAGE("  - A["<<i<<"]="<<modulusArray6[i]);
  MESSAGE("");

  int array7[1]={1} ;
  MEDMODULUSARRAY modulusArray7(1,array7);
  MESSAGE("Same arrays, same order ... ");
  if ((ret=modulusArray6.compare(modulusArray7))!=1)
    MESSAGE( "Error : two arrays are identical !"<<ret)
  else
      MESSAGE( "OK" )

  int array8[1]={2} ;
  MEDMODULUSARRAY modulusArray8(1,array8);
  MESSAGE("Different arrays ... ");
  if ((ret=modulusArray6.compare(modulusArray8))!=0)
      MESSAGE( "Error : two arrays are different !"<<ret)
  else
      MESSAGE( "OK" )

  
  // 2
  int array60[2]={1,2} ;
  MEDMODULUSARRAY modulusArray60(2,array60);

  MESSAGE("ModuloArray 2 :");
  for(int i=-10;i<15;i++)
    MESSAGE("  - A["<<i<<"]="<<modulusArray60[i]);
  MESSAGE("");

  int array70[2]={1,2} ;
  MEDMODULUSARRAY modulusArray70(2,array70);
  MESSAGE("Same arrays, same order ... ");
  if ((ret=modulusArray60.compare(modulusArray70))!=1)
      MESSAGE( "Error : two arrays are identical !"<<ret)
  else
    MESSAGE( "OK" );
  int array80[2]={2,2} ;
  MEDMODULUSARRAY modulusArray80(2,array80);
  MESSAGE("Different arrays ... ");
  if ((ret=modulusArray60.compare(modulusArray80))!=0)
    MESSAGE( "Error : two arrays are different !"<<ret)
  else
    MESSAGE( "OK" )
  int array90[2]={2,1} ;
  MEDMODULUSARRAY modulusArray90(2,array90);
  MESSAGE("Same arrays, reverse order ... ");
  if ((ret=modulusArray60.compare(modulusArray90))!=-1)
    MESSAGE( "Error : two arrays are in reverse order !"<<ret)
  else
    MESSAGE( "OK" )

  

 

  return 0 ;
}
