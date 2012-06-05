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

#include "MEDMEM_Array.hxx"

using namespace std;
using namespace MEDMEM;
int main (int argc, char ** argv) {

  int SpaceDimension = 3 ;
  int NumberOfNodes = 4 ; 
  MEDARRAY<int> * myArray = new MEDARRAY<int>(SpaceDimension,NumberOfNodes,MED_EN::MED_FULL_INTERLACE) ;

  //const int * value = myArray->get(MED_FULL_INTERLACE) ;
  for (int i=1; i<=NumberOfNodes; i++)
    for (int j=1; j<=SpaceDimension; j++)
      myArray->setIJ(i,j,i) ;
  
  int numberof ;
  MESSAGE_MED("Show all 1 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    //int * node = myArray->getI(MED_FULL_INTERLACE,i) ;
    const int * node = myArray->getRow(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE_MED("Show all 2 :");
  numberof = myArray->getLengthValue() ;
  for (int i=1; i<=myArray->getLeadingValue() ; i++) {
    //int * node = myArray->getJ(MED_NO_INTERLACE,i) ;
    const int * node = myArray->getColumn(i) ;
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << node[j] << " " ;
    cout << endl ;
  }
  MESSAGE_MED("Show all 3 :");
  numberof = myArray->getLeadingValue() ;
  for (int i=1; i<=myArray->getLengthValue() ; i++) {
    cout << " - " ;
    for (int j=1;j<numberof+1;j++)
      cout << myArray->getIJ(i,j) << " " ;
    cout << endl ;
  }

  MESSAGE_MED("Show all 0 :");
  numberof = myArray->getLeadingValue() ;
  int length = myArray->getLengthValue() ;
  const int * NoInterlaceArray = myArray->get(MED_EN::MED_NO_INTERLACE) ;
  for (int i=0; i<length ; i++) {
    cout << " - " ;
    for (int j=0;j<numberof;j++)
      cout << NoInterlaceArray[j*length+i] << " " ;
    cout << endl ;
  }

  delete myArray ;

  return 0 ;
}
