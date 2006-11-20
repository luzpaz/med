// Copyright (C) 2006  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
//#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_MedMeshDriver.hxx"

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

  cout <<"ModulusArray"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray[i]<<endl;
  cout <<endl;

  // test compare
  int ret ;

  int array2[5]={1,4,0,1,2} ;
  MEDMODULUSARRAY modulusArray2(5,array2);

  try{
    ret=modulusArray2.compare(modulusArray);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  cout<<"!!!!!!ret"<<ret<<endl;
  CPPUNIT_ASSERT_MESSAGE("Two identical arrays - KO", ret == 1);
  
  int array3[5]={1,2,1,0,4} ;
  MEDMODULUSARRAY modulusArray3(5,array3) ;
  try{
    ret=modulusArray3.compare(modulusArray);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two arrays are in reverse order - KO",ret == -1); 

  int array4[6]={1,2,1,0} ;
  MEDMODULUSARRAY modulusArray4(4,array4) ;
  try{
    ret=modulusArray4.compare(modulusArray);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two arrays are have different size - KO",ret == 0); 
  
  int array5[5]={1,2,1,0,1} ;
  MEDMODULUSARRAY modulusArray5(5,array5) ;
  
  try{
    ret=modulusArray5.compare(modulusArray);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two different arrays, same size - KO",ret == 0); 
   
  // test small array :

  // 1
  int array6[1]={1} ;
  MEDMODULUSARRAY modulusArray6(1,array6);

  cout <<"Test small array"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray6[i]<<endl;


  int array7[1]={1} ;
  MEDMODULUSARRAY modulusArray7(1,array7);
  try{
    ret=modulusArray6.compare(modulusArray7);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two identical arrays - KO", ret == 1);

  int array8[1]={2} ;
  MEDMODULUSARRAY modulusArray8(1,array8);
  try{
    ret=modulusArray6.compare(modulusArray8);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two different arrays - KO",ret == 0); 
  
  // 2
  int array60[2]={1,2} ;
  MEDMODULUSARRAY modulusArray60(2,array60);

  cout<<"ModulusArray 2 :"<<endl;
  for(int i=-10;i<15;i++)
    cout <<"  - A["<<i<<"]="<<modulusArray60[i]<<endl;
  cout <<endl;

  int array70[2]={1,2} ;
  MEDMODULUSARRAY modulusArray70(2,array70);
  
  try{
    ret=modulusArray60.compare(modulusArray70);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO",ret == 1); 

  int array80[2]={2,2} ;
  MEDMODULUSARRAY modulusArray80(2,array80);
  try{
    ret=modulusArray60.compare(modulusArray80);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Different arrays - KO",ret == 0); 

  int array90[2]={2,1} ;
  MEDMODULUSARRAY modulusArray90(2,array90);

  try{
    ret=modulusArray60.compare(modulusArray90);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Two arrays are in reverse order - KO",ret == -1); 
  
  //test not vertex nodes
  int array100[2]={1,2} ;
  MEDMODULUSARRAY modulusArray100(2,2,array100);

  try{
    ret=modulusArray60.compare(modulusArray100);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Different arrays -KO",ret != 1); 

  int array110[4]={1,2,4,3};
  MEDMODULUSARRAY modulusArray110(2,4,array110);

  int array120[4]={1,2,4,3};
  MEDMODULUSARRAY modulusArray120(2,4,array120);

  try{
    ret=modulusArray120.compare(modulusArray110);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_MESSAGE("Same arrays, same order - KO",ret == 1); 
}
