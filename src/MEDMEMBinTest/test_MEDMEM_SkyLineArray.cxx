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
#include "MEDMEM_SkyLineArray.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char ** argv) {
  int NumberOfCell = 3 ; // 1 triangle,1 quadrangle,1 triangle
  int Size = 10 ; // 10 nodes

  int * index = new int[NumberOfCell+1] ;
  index[0]=1;
  index[1]=4;
  index[2]=8;
  index[3]=11;
  int * value = new int[Size] ;
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
    const int * cell = myArray->getI(i) ;
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
  const int * index2 = myArray2->getIndex() ;
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
  
