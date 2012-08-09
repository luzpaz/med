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
#include <cppunit/TestAssert.h>
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_define.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;

/*!
 *  Check methods (17), defined in MEDMEM_SkyLineArray.hxx:
 *  class MEDSKYLINEARRAY {
 *   (+) MEDSKYLINEARRAY();
 *   (+) MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray);
 *   (+) MEDSKYLINEARRAY(const int count, const int length);
 *   (+) MEDSKYLINEARRAY(const int count, const int length,
 *                       const int* index, const int* value, bool shallowCopy=false);
 *   (+) ~MEDSKYLINEARRAY();
 *   (+) inline int  getNumberOf()       const;
 *   (+) inline int  getLength()         const;
 *   (+) inline const int*  getIndex()   const;
 *   (+) inline const int*  getValue()   const;
 *   (+) inline int  getNumberOfI(int i) const throw (MEDEXCEPTION);
 *   (+) inline const int*  getI(int i)  const throw (MEDEXCEPTION);
 *   (+) inline int  getIJ(int i, int j) const throw (MEDEXCEPTION);
 *   (+) inline int  getIndexValue(int i) const throw (MEDEXCEPTION);
 *   (+) inline void setIndex(const int* index);
 *   (+) inline void setI(const int i, const int* values) throw (MEDEXCEPTION);
 *   (+) inline void setIJ(int i, int j, int value) throw (MEDEXCEPTION);
 *   (+) inline void setIndexValue(int i, int value) throw (MEDEXCEPTION);
 *   (+) friend ostream& operator<<(ostream &os, const MEDSKYLINEARRAY &sky);
 *  }
 *
 */
void MEDMEMTest::testSkyLineArray()
{
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
  const int * ArrayIndex ;
  try
  {
    ArrayIndex = myArray->getIndex() ;
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  const int * ArrayValue ;
  try
  {
    ArrayValue  = myArray->getValue() ;
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }


  try
  {
    CPPUNIT_ASSERT(myArray->getNumberOf()==NumberOfCell);

  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT(myArray->getLength()==Size);
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  for (int i=1; i< NumberOfCell+1 ; i++) 
  {
    const int * cell;
    CPPUNIT_ASSERT_NO_THROW(cell = myArray->getI(i));
    int numberof;
    CPPUNIT_ASSERT_NO_THROW(numberof= myArray->getNumberOfI(i));
    CPPUNIT_ASSERT(numberof==index[i]-index[i-1]);
    int verif;
    for (int j=0;j<numberof;j++)
    {
      CPPUNIT_ASSERT_NO_THROW(verif=myArray->getIJ(i,j+1));
      CPPUNIT_ASSERT(verif == cell[j]);
    }
  }

  MEDSKYLINEARRAY * myArray2 = new MEDSKYLINEARRAY(*myArray) ;
  delete myArray ;
  
  const int * index2 = myArray2->getIndex() ;
  for (int i=1; i<=NumberOfCell ; i++) {
    for (int j=index2[i-1];j<index2[i];j++)
      CPPUNIT_ASSERT_NO_THROW(myArray2->getIndexValue(j));
  }

  delete myArray2 ;

  MEDSKYLINEARRAY myArray3(NumberOfCell,Size);
  try
  { 
    myArray3.setIndex(index);
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_THROW(myArray3.setI(0, 0),MEDEXCEPTION);

  for (int i=1; i<NumberOfCell+1 ; i++) {
    CPPUNIT_ASSERT_NO_THROW(myArray3.setI(i, &value[index[i-1]-1]));
  }

  for (int i=1; i<NumberOfCell+1 ; i++) {
    int numberof;
    CPPUNIT_ASSERT_NO_THROW(numberof = myArray3.getNumberOfI(i));
    for (int j=1;j<numberof+1;j++)
      CPPUNIT_ASSERT_NO_THROW(myArray3.setIJ(i,j,myArray3.getIJ(i,j))) ;
  }
  
  for (int i=1; i<=NumberOfCell ; i++) {
    for (int j=index[i-1];j<index[i];j++)
      CPPUNIT_ASSERT_NO_THROW(myArray3.setIndexValue(j,value[j-1]));
  }

  delete[] index ;
  delete[] value ;
  ostringstream out;
  MEDSKYLINEARRAY myEmptyArray;
  out<<myEmptyArray<<endl;
}

