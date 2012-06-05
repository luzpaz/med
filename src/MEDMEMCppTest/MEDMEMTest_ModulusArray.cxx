// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEMTest.hxx"
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_ModulusArray.hxx"
#include "MEDMEM_define.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (4), defined in MEDMEM_ModulusArray.hxx:
 *  class MEDMODULUSARRAY {
 *   (+) MEDMODULUSARRAY(int length, const int * array);
 *   (+) MEDMODULUSARRAY(int vertexLength, int totalLength, const int * array);
 *   (+) ~MEDMODULUSARRAY();
 *   (+) const int operator[](const int &i) const;
 *   (+) int compare(const MEDMODULUSARRAY &modulusArray) const;
 *  }
 */
void MEDMEMTest::testModulusArray()
{

  int array[5]={0,1,2,1,4} ;
  MEDMODULUSARRAY modulusArray(5,array);

  for(int i=-10, j=0; i<15; i++, j++)
    CPPUNIT_ASSERT_EQUAL( array[ j%5 ], modulusArray[i] );

  // test compare
  int ret ;

  int array2[5]={1,4,0,1,2} ;
  MEDMODULUSARRAY modulusArray2(5,array2);

  ret=modulusArray2.compare(modulusArray);

  CPPUNIT_ASSERT_MESSAGE("Two identical arrays - KO", ret == 1);

  int array3[5]={1,2,1,0,4} ;
  MEDMODULUSARRAY modulusArray3(5,array3) ;
  ret=modulusArray3.compare(modulusArray);

  CPPUNIT_ASSERT_MESSAGE("Two arrays are in reverse order - KO",ret == -1);

  int array4[6]={1,2,1,0} ;
  MEDMODULUSARRAY modulusArray4(4,array4) ;
  ret=modulusArray4.compare(modulusArray);

  CPPUNIT_ASSERT_MESSAGE("Two arrays are have different size - KO",ret == 0);

  int array5[5]={1,2,1,0,1} ;
  MEDMODULUSARRAY modulusArray5(5,array5) ;

  ret=modulusArray5.compare(modulusArray);

  CPPUNIT_ASSERT_MESSAGE("Two different arrays, same size - KO",ret == 0);

  // test small array :

  // 1
  int array6[1]={1} ;
  MEDMODULUSARRAY modulusArray6(1,array6);

  for(int i=-10;i<15;i++)
    CPPUNIT_ASSERT_EQUAL( 1, modulusArray6[i] );


  int array7[1]={1} ;
  MEDMODULUSARRAY modulusArray7(1,array7);
  ret=modulusArray6.compare(modulusArray7);

  CPPUNIT_ASSERT_MESSAGE("Two identical arrays - KO", ret == 1);

  int array8[1]={2} ;
  MEDMODULUSARRAY modulusArray8(1,array8);
  ret=modulusArray6.compare(modulusArray8);

  CPPUNIT_ASSERT_MESSAGE("Two different arrays - KO",ret == 0);

  // 2
  int array60[2]={1,2} ;
  MEDMODULUSARRAY modulusArray60(2,array60);

  for(int i=-10, j = 0;i<15;i++,j++)
    CPPUNIT_ASSERT_EQUAL( array60[ j%2 ], modulusArray60[i] );

  int array70[2]={1,2} ;
  MEDMODULUSARRAY modulusArray70(2,array70);

  ret=modulusArray60.compare(modulusArray70);

  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO",ret == 1);

  int array80[2]={2,2} ;
  MEDMODULUSARRAY modulusArray80(2,array80);
  ret=modulusArray60.compare(modulusArray80);

  CPPUNIT_ASSERT_MESSAGE("Different arrays - KO",ret == 0);

  int array90[2]={2,1} ;
  MEDMODULUSARRAY modulusArray90(2,array90);

  ret=modulusArray60.compare(modulusArray90);

  CPPUNIT_ASSERT_MESSAGE("Two arrays are in reverse order - KO",ret == -1);

  //test not vertex nodes
  int array100[2]={1,2} ;
  MEDMODULUSARRAY modulusArray100(2,2,array100); // == MEDMODULUSARRAY(2,array100);

  ret = modulusArray60.compare(modulusArray100);

  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO", ret == 1);

  // check comparison of equal arrays, containing non-vertex nodes

  int array110[4] = {1,2,4,3};
  MEDMODULUSARRAY modulusArray110(2,4,array110);

  int array120[4] = {1,2,4,3};
  MEDMODULUSARRAY modulusArray120(2,4,array120);

  int array130[4] = {1,2,3,4};
  MEDMODULUSARRAY modulusArray130(2,4,array130);

  // same order of non-vertex nodes
  CPPUNIT_ASSERT_NO_THROW(ret = modulusArray120.compare(modulusArray110));
  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO", ret == 1);

  // different order of non-vertex nodes
  CPPUNIT_ASSERT_NO_THROW(ret = modulusArray130.compare(modulusArray110));
  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO", ret == 1);

  // check comparison of different arrays, containing non-vertex nodes

  // difference is in vertex nodes
  int array140[4] = {1,5,4,3};
  MEDMODULUSARRAY modulusArray140 (2,4,array140);

  CPPUNIT_ASSERT_NO_THROW(ret = modulusArray120.compare(modulusArray140));
  CPPUNIT_ASSERT_MESSAGE("Different arrays - KO", ret == 0);

  // difference is in non-vertex nodes
  int array150[4] = {1,2,4,5};
  MEDMODULUSARRAY modulusArray150 (2,4,array150);

  CPPUNIT_ASSERT_NO_THROW(ret = modulusArray120.compare(modulusArray150));
  CPPUNIT_ASSERT_MESSAGE("Different arrays - KO", ret == 0);

  // check that operator[] returns only vertex nodes
  CPPUNIT_ASSERT_EQUAL(1, modulusArray120[0]);
  CPPUNIT_ASSERT_EQUAL(2, modulusArray120[1]);
  CPPUNIT_ASSERT_EQUAL(1, modulusArray120[2]);
  CPPUNIT_ASSERT_EQUAL(2, modulusArray120[3]);
  CPPUNIT_ASSERT_EQUAL(1, modulusArray120[4]);
  CPPUNIT_ASSERT_EQUAL(2, modulusArray120[5]);
}
