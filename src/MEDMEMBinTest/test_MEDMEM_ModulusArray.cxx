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
#include "MEDMEM_ModulusArray.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv) {
  using namespace std ;

  int array[5]={0,1,2,1,4} ;
  MEDMODULUSARRAY modulusArray(5,array);

  cout<<"ModuloArray :"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray[i]<<endl;
  cout <<endl;

  // test compare
  int ret ;

  int array2[5]={1,4,0,1,2} ;
  MEDMODULUSARRAY modulusArray2(5,array2) ;
  cout<<"Same arrays, same order ... ";
  if ((ret=modulusArray2.compare(modulusArray))!=1)
    cout << "Error : two arrays must be identical !"<<ret<<endl;
  else
    cout << "OK" <<endl;

  int array3[5]={1,2,1,0,4} ;
  MEDMODULUSARRAY modulusArray3(5,array3) ;
  cout<<"Same arrays, reverse order ... ";
  if ((ret=modulusArray3.compare(modulusArray))!=-1)
    cout << "Error : two arrays are in reverse order !"<<ret<<endl;
  else
    cout << "OK" <<endl;

  int array4[6]={1,2,1,0} ;
  MEDMODULUSARRAY modulusArray4(4,array4) ;
  cout<<"Different arrays size ... ";
  if ((ret=modulusArray4.compare(modulusArray))!=0)
    cout << "Error : two arrays have different size !"<<ret<<endl;
  else
    cout << "OK" <<endl;
  
  int array5[5]={1,2,1,0,1} ;
  MEDMODULUSARRAY modulusArray5(5,array5) ;
  cout<<"Different array, same size ... ";
  if ((ret=modulusArray5.compare(modulusArray))!=0)
    cout << "Error : two arrays are not be identical !"<<ret<<endl;
  else
    cout << "OK" <<endl;
 
  // test small array :

  // 1
  int array6[1]={1} ;
  MEDMODULUSARRAY modulusArray6(1,array6);

  cout<<"ModuloArray 1 :"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray6[i]<<endl;
  cout <<endl;

  int array7[1]={1} ;
  MEDMODULUSARRAY modulusArray7(1,array7);
  cout<<"Same arrays, same order ... ";
  if ((ret=modulusArray6.compare(modulusArray7))!=1)
    cout << "Error : two arrays are identical !"<<ret<<endl;
  else
    cout << "OK" <<endl;
  int array8[1]={2} ;
  MEDMODULUSARRAY modulusArray8(1,array8);
  cout<<"Different arrays ... ";
  if ((ret=modulusArray6.compare(modulusArray8))!=0)
    cout << "Error : two arrays are different !"<<ret<<endl;
  else
    cout << "OK" <<endl;

  
  // 2
  int array60[2]={1,2} ;
  MEDMODULUSARRAY modulusArray60(2,array60);

  cout<<"ModuloArray 2 :"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray60[i]<<endl;
  cout <<endl;

  int array70[2]={1,2} ;
  MEDMODULUSARRAY modulusArray70(2,array70);
  cout<<"Same arrays, same order ... ";
  if ((ret=modulusArray60.compare(modulusArray70))!=1)
    cout << "Error : two arrays are identical !"<<ret<<endl;
  else
    cout << "OK" <<endl;
  int array80[2]={2,2} ;
  MEDMODULUSARRAY modulusArray80(2,array80);
  cout<<"Different arrays ... ";
  if ((ret=modulusArray60.compare(modulusArray80))!=0)
    cout << "Error : two arrays are different !"<<ret<<endl;
  else
    cout << "OK" <<endl;
  int array90[2]={2,1} ;
  MEDMODULUSARRAY modulusArray90(2,array90);
  cout<<"Same arrays, reverse order ... ";
  if ((ret=modulusArray60.compare(modulusArray90))!=-1)
    cout << "Error : two arrays are in reverse order !"<<ret<<endl;
  else
    cout << "OK" <<endl;

  

 

  return 0 ;
}
