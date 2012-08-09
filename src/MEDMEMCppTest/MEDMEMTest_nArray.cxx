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

#include "MEDMEM_nArray.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (21), defined in MEDMEM_nArray.hxx:
 *  class MEDMEM_Array_ {
 *   (-) virtual bool getGaussPresence() const { return false; }
 *   (-) virtual MED_EN::medModeSwitch getInterlacingType() const {return MED_EN::MED_UNDEFINED_INTERLACE;}
 *   (+) ~MEDMEM_Array_() {};
 *  }
 *  template<class ARRAY_ELEMENT_TYPE,
 *           class INTERLACING_POLICY=FullInterlaceNoGaussPolicy,
 *           class CHECKING_POLICY=IndexCheckPolicy>
 *  class MEDMEM_Array : public INTERLACING_POLICY, public CHECKING_POLICY, public MEDMEM_Array_ {
 *   (+) MEDMEM_Array();
 *   (+) ~MEDMEM_Array();
 *   (+) inline MEDMEM_Array(int dim, int nbelem);
 *   (+) inline MEDMEM_Array(ElementType * values, int dim, int nbelem,
 *                                bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+) inline MEDMEM_Array(int dim, int nbelem, int nbtypegeo,
 *                               const int * const  nbelgeoc, const int * const nbgaussgeo);
 *   (+) inline MEDMEM_Array(ElementType * values, int dim, int nbelem, int nbtypegeo,
 *                               const int * const  nbelgeoc, const int * const  nbgaussgeo,
 *                               bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+) inline MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false);
 *   (+) inline MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> &
 *                  operator=(const MEDMEM_Array & array);
 *   (+) MED_EN::medModeSwitch getInterlacingType() const;
 *   (+) bool getGaussPresence() const;
 *   (+) ElementType * getPtr();
 *   (+) void setPtr(ElementType * values, bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+) inline const ElementType * getRow(int i) const;
 *   (+) void setRow(int i,const ElementType * const value);
 *   (+) inline const ElementType * getColumn(int j) const;
 *   (+) void setColumn(int j, const ElementType * const value);
 *   (+) inline const ElementType & getIJ(int i, int j) const;
 *   (+) inline const ElementType & getIJK(int i, int j, int k) const;
 *   (+) inline void setIJ(int i, int j, const ElementType & value);
 *   (+) inline void setIJK(int i, int j, int k, const ElementType & value);
 *   (+) bool operator == (const MEDMEM_Array & array) const;
 *   (+) friend ostream & operator<<(ostream & os, const MEDMEM_Array & array);
 *  }
 */
void MEDMEMTest::testnArray()
{
  const int mdim = 2;
  const int nbelem1 = 20;
  const double EPS = 0.00001;

  /////////////////////////////////////////
  // TEST 1  FullInterlace et NoGauss    //
  ////////////////////////////////////////
  const double * myArray1Ptr = 0;
  const double array1Ref[]  = { 11 , 12 , 21 , 22 , 31 , 32 , 41 , 42 , 51 , 52 ,
                               61 , 62 , 71 , 72 , 81 , 82 , 91 , 92 , 101 , 102 ,
                               111 , 112 , 121 , 122 , 131 , 132 , 141 , 142 ,
                               151 , 152 , 161 , 162 , 171 , 172 , 181 , 182 ,
                               191 , 192 , 201 , 202 };

  const double array2Ref[] = { 11 , 21 , 31 , 41 , 51 , 61 , 71 , 81 , 91 , 101
                               , 111 , 121 , 131 , 141 , 151 , 161 , 171 , 181
                               , 191 , 201 ,
                               12 , 22 , 32 , 42 , 52 , 62 , 72 , 82 , 92 , 102
                               , 112 , 122 , 132 , 142 , 152 , 162 , 172 , 182
                               , 192 , 202 };

  //test MEDMEM_Array(int dim, int nbelem)
  MEDMEM_Array<double>  myArray1(mdim,nbelem1);
  //test getGaussPresence
  CPPUNIT_ASSERT( !myArray1.getGaussPresence());

  //test setIJ. Fill array
  for (int i =1; i <= nbelem1; i++)
    for (int j =1; j <= mdim; j++)
    {
      CPPUNIT_ASSERT_NO_THROW(myArray1.setIJ(i,j,i*10+j));
    }

  //test getPtr
  myArray1Ptr = myArray1.getPtr();
  for (int i = 0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

  {
    //test MEDMEM_Array(ElementType * values, int dim, int nbelem, bool shallowCopy=false, bool ownershipOfValues=false)
    MEDMEM_Array<double> myArray1val ((double*)array1Ref, mdim, nbelem1);
    myArray1Ptr = myArray1val.getPtr();
    for (int i = 0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

    //test MEDMEM_Array(ElementType * values, int dim, int nbelem, bool shallowCopy=true, bool ownershipOfValues=false);
    MEDMEM_Array<double> myArray1valsh ((double*)array1Ref, mdim, nbelem1, true);
    myArray1Ptr = myArray1valsh.getPtr();
    for (int i = 0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

    //test setPtr
    double myPtr[10] = {-1, -2, -3, -4, -5, -6, -7, -8, -9, 0};
    CPPUNIT_ASSERT_NO_THROW(myArray1valsh.setPtr(myPtr));
    //size the array stays the same, only first 10 element are being overwritten
    myArray1Ptr = myArray1valsh.getPtr();
    for (int i = 0; i < 10; i++)
      CPPUNIT_ASSERT_DOUBLES_EQUAL(myArray1Ptr[i], myPtr[i], EPS);
  }

  //test MEDMEM_Array(ElementType * values, int dim, int nbelem, bool shallowCopy=true, bool ownershipOfValues=true);
  {
    double * array1Ref_do_not_delete = new double[40];
    for (int i = 0; i < 40; i = i + 2) {
      array1Ref_do_not_delete[i  ] = 10.0 * (i+1) + 1;
      array1Ref_do_not_delete[i+1] = 10.0 * (i+1) + 2;
    }
    MEDMEM_Array<double> myArray1valow (array1Ref_do_not_delete, mdim, nbelem1, true, true);
  }

  //test MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
  MEDMEM_Array<double> myArray1bis (myArray1, false);
  CPPUNIT_ASSERT(myArray1 == myArray1bis);

  myArray1Ptr = myArray1bis.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

  //test MEDMEM_Array()
  MEDMEM_Array<double> myArray1ter;
  try
  {
    myArray1ter = myArray1;
    myArray1Ptr = myArray1ter.getPtr();
  }
  catch ( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

  MEDMEM_Array<double> myArray1qua(mdim,nbelem1);
  myArray1Ptr = myArray1qua.getPtr();

  for (int i =1; i <= nbelem1; i++)
  {
    try {
      //test setRow(int i,const ElementType * const value)
      myArray1qua.setRow(i,&array1Ref[(i-1)*mdim]);
    }
    catch (MEDMEM::MEDEXCEPTION &m) {
      CPPUNIT_FAIL(m.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  }

  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

  //test getColumn(int j)
  CPPUNIT_ASSERT_THROW(myArray1qua.getColumn(1), MEDEXCEPTION);

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> * myArray1cin = ArrayConvert(myArray1);
  myArray1Ptr = myArray1cin->getPtr();
  int elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i <= nbelem1 ; i++ ) {
      CPPUNIT_ASSERT(  myArray1cin->getIJ(i,j) ==  array2Ref[elemno] );
      CPPUNIT_ASSERT(  myArray1Ptr[elemno]     ==  array2Ref[elemno] );
      elemno++;
    }
  delete myArray1cin;
  /////////////////////////////////////
  // TEST 2   NoInterlace et NoGauss //
  /////////////////////////////////////
  {
    const double * myArray2Ptr = 0;
    //MEDMEM_Array(int dim, int nbelem)
    MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2(mdim,nbelem1);

    for (int i =1; i <= nbelem1; i++) {
      for (int j =1; j <= mdim; j++) {
        //test setIJ(int i, int j, const ElementType & value)
        CPPUNIT_ASSERT_NO_THROW(myArray2.setIJ(i,j,i*10+j));
      }
    }

    //test getPtr()
    myArray2Ptr = myArray2.getPtr();
    for (int i =0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

    //test MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
    MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2bis(myArray2, false);
    myArray2Ptr = myArray2bis.getPtr();
    for (int i =0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

    //test MEDMEM_Array()
    MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2ter;
    //test operator=(const MEDMEM_Array & array)
    myArray2ter = myArray2;
    //test getPtr
    myArray2Ptr = myArray2ter.getPtr();
    for (int i =0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

    MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2qua(mdim,nbelem1);
    myArray2Ptr = myArray2qua.getPtr();

    for (int j = 1; j <= mdim; j++) {
      //test setColumn(int j, const ElementType * const value)
      CPPUNIT_ASSERT_NO_THROW(myArray2qua.setColumn(j,&array2Ref[nbelem1*(j-1)]));
    }

    for (int i =0; i < mdim*nbelem1; i++)
      CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

    //test getRow(int i)
    CPPUNIT_ASSERT_THROW(myArray2qua.getRow(1), MEDEXCEPTION);

    MEDMEM_Array<double,FullInterlaceNoGaussPolicy> * myArray2cin = ArrayConvert(myArray2);
    myArray2Ptr = myArray2cin->getPtr();
    elemno = 0;
    for (int  i=1; i <= nbelem1 ; i++) {
      for (int j=1; j <= mdim; j++) {
        CPPUNIT_ASSERT(  myArray2cin->getIJ(i,j) ==  array1Ref[elemno] );
        CPPUNIT_ASSERT(  myArray2Ptr[elemno]     ==  array1Ref[elemno] );
        elemno++;
      }
    }
    delete myArray2cin;
  }

  ///////////////////////////////////////
  // TEST 3   FullInterlace et Gauss  //
  //////////////////////////////////////
  const int nbelem2 = 5;
  const int nbtypegeo = 2;
  const int nbelgeoc[nbtypegeo+1]   = {1,3,6};
  const int nbgaussgeo[nbtypegeo+1] = {-1,2,3};

  const double * myArray3Ptr = 0;
  const double array3Ref[] = {1.11 , 1.12 , 1.21 , 1.22 ,
                              2.11 , 2.12 , 2.21 , 2.22 ,
                              13.11 , 13.12 , 13.21 , 13.22 , 13.31 , 13.32 ,
                              14.11 , 14.12 , 14.21 , 14.22 , 14.31 , 14.32 ,
                              15.11 , 15.12 , 15.21 , 15.22 , 15.31 , 15.32 };

  const double array4Ref[] = { 1.11 , 1.21 , 2.11 , 2.21,
                               13.11 , 13.21 , 13.31 , 14.11 , 14.21 , 14.31 ,
                               15.11 , 15.21 , 15.31 ,
                               1.12 , 1.22 , 2.12 , 2.22 ,
                               13.12 , 13.22 , 13.32 , 14.12 , 14.22 , 14.32 ,
                               15.12 , 15.22 , 15.32 };

  const int array5Ref[] =    { 1 , 1 , 2 , 2,
                               13 , 13 , 13 , 14 , 14 , 14 ,
                               15 , 15 , 15 ,
                               1 , 1 , 2 , 2 ,
                               13 , 13 , 13 , 14 , 14 , 14 ,
                               15 , 15 , 15 };

  //test MEDMEM_Array(int dim, int nbelem, int nbtypegeo, const int * const  nbelgeoc, const int * const nbgaussgeo)
  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
        for (int j=1; j <= mdim; j++) {
          try{
            //test setIJK(int i, int j, int k, const ElementType & value)
            myArray3.setIJK(elemno,j,k,elemno+(ntyp-1)*10+0.1*k+0.01*j);
          }
          catch (MEDMEM::MEDEXCEPTION &m) {
            CPPUNIT_FAIL(m.what());
          }
          catch (...)
          {
            CPPUNIT_FAIL("Unknown exception");
          }
        }
      elemno++;
    }
  }

  myArray3Ptr = myArray3.getPtr();
  for (int i =0; i < myArray3.getArraySize(); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL(myArray3Ptr[i], array3Ref[i], EPS);

  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3bis(myArray3, false);
  myArray3Ptr = myArray3bis.getPtr();

  elemno = 0;
  for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
    for (int k=1; k <= myArray3bis.getNbGauss(i); k++)
      for (int j=1; j <= mdim; j++) {
          CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3bis.getIJK(i,j,k), array3Ref[elemno], EPS );
          CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3Ptr[elemno], array3Ref[elemno], EPS );
        elemno++;
      }

  //test MEDMEM_Array()
  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3ter;
  //test operator=(const MEDMEM_Array & array)
  myArray3ter = myArray3;
  myArray3Ptr = myArray3ter.getPtr();

  elemno = 0;
  for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
    for (int k=1; k <= myArray3ter.getNbGauss(i); k++)
      for (int j=1; j <= mdim; j++) {
         CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3ter.getIJK(i,j,k), array3Ref[elemno], EPS );
         CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3Ptr[elemno], array3Ref[elemno], EPS );
        elemno++;
      }

  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3qua(mdim,nbelem2,
                                                            nbtypegeo,nbelgeoc,nbgaussgeo);
  myArray3Ptr = myArray3qua.getPtr();

  int cumul = 0;
  for (int i =1; i <= nbelem2; i++){
    try{
      myArray3qua.setRow(i,&array3Ref[cumul]);
      cumul+=myArray3qua.getNbGauss(i)*mdim;
    }
    catch (MEDMEM::MEDEXCEPTION &m) {
      CPPUNIT_FAIL(m.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  };

  for (int i =0; i < myArray3qua.getArraySize(); i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3Ptr[i], array3Ref[i], EPS );
  }

  MEDMEM_Array<double,NoInterlaceGaussPolicy> * myArray3cin = ArrayConvert(myArray3);
  myArray3Ptr = myArray3cin->getPtr();
  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray3cin->getNbGauss(i); k++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3cin->getIJK(i,j,k), array4Ref[elemno], EPS );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(myArray3Ptr[elemno], array4Ref[elemno], EPS );
        elemno++;
      }
  delete myArray3cin;
  ////////////////////////////////////
  // TEST 4   NoInterlace et Gauss  //
  ////////////////////////////////////
  const double * myArray4Ptr = 0;
  //test MEDMEM_Array(int dim, int nbelem, int nbtypegeo, const int * const  nbelgeoc, const int * const nbgaussgeo)
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
        for (int j=1; j <= mdim; j++) {
          try{
            //test setIJK(int i, int j, int k, const ElementType & value)
            myArray4.setIJK(elemno,j,k,elemno+(ntyp-1)*10+0.1*k+0.01*j);
          }
          catch (MEDMEM::MEDEXCEPTION &m) {
            CPPUNIT_FAIL(m.what());
          }
          catch (...)
          {
            CPPUNIT_FAIL("Unknown exception");
          }
        }
      elemno++;
    }
  }

   //test getPtr()
  myArray4Ptr = myArray4.getPtr();
  for (int i =0; i < myArray4.getArraySize(); i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[i], array4Ref[i], EPS );
  }

  //test MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4bis(myArray4, false);
  myArray4Ptr = myArray4bis.getPtr();

  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray4bis.getNbGauss(i); k++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4bis.getIJK(i,j,k), array4Ref[elemno], EPS );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[elemno], array4Ref[elemno], EPS );
        elemno++;
      }

  //test MEDMEM_Array()
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4ter;
  //test operator=(const MEDMEM_Array & array)
  myArray4ter = myArray4;
  myArray4Ptr = myArray4ter.getPtr();

  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray4ter.getNbGauss(i); k++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4ter.getIJK(i,j,k), array4Ref[elemno], EPS );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[elemno], array4Ref[elemno], EPS );
        elemno++;
      }

  {
    MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4qua (mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);
    myArray4Ptr = myArray4qua.getPtr();

    for (int j = 1; j <= mdim; j++) {
      int k = (myArray4qua.getArraySize() / mdim) * (j - 1);
      CPPUNIT_ASSERT_NO_THROW(myArray4qua.setColumn(j, &array4Ref[k]));
    }

    for (int i =0; i < myArray4qua.getArraySize(); i++)
      CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[i], array4Ref[i], EPS );
  }

  {
    MEDMEM_Array<double,FullInterlaceGaussPolicy> * myArray4cin = ArrayConvert(myArray4);
    myArray4Ptr = myArray4cin->getPtr();
    elemno = 0;
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray4cin->getNbGauss(i); k++)
        for (int j=1; j <= mdim; j++) {
          CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4cin->getIJK(i,j,k), array3Ref[elemno], EPS );
          CPPUNIT_ASSERT( myArray4cin->getIJK(i,j,k) == myArray3.getIJK(i,j,k) );
          CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[elemno], array3Ref[elemno], EPS );
          elemno++;
        }
    delete myArray4cin;
  }

  CPPUNIT_ASSERT(myArray4bis.getInterlacingType() != MED_UNDEFINED_INTERLACE);

  CPPUNIT_ASSERT_THROW(myArray4.getIJ(0,2), MEDEXCEPTION);

  ostringstream os;
  //test operator<<(ostream & os, const MEDMEM_Array & array)
  os << myArray4;
  CPPUNIT_ASSERT(os.str() != "");

  const int * myArray5Ptr = 0;
  //test MEDMEM_Array(ElementType * values, int dim, int nbelem, int nbtypegeo,
  //                  const int * const  nbelgeoc, const int * const  nbgaussgeo,
  //                  bool shallowCopy=false, bool ownershipOfValues=false)
  MEDMEM_Array<int, NoInterlaceGaussPolicy> myArray5 ((int*)array5Ref, mdim, nbelem2,
                                                      nbtypegeo, nbelgeoc, nbgaussgeo);
  //test getGaussPresence
  CPPUNIT_ASSERT( myArray5.getGaussPresence());

  myArray5Ptr = myArray5.getPtr();
  for (int i =0; i < myArray5.getArraySize(); i++) {
    CPPUNIT_ASSERT_EQUAL( myArray5Ptr[i], array5Ref[i]);
  }

  //test MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
  MEDMEM_Array<int,NoInterlaceGaussPolicy> myArray5bis(myArray5, false);
  myArray5Ptr = myArray5bis.getPtr();

  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray5bis.getNbGauss(i); k++) {
        CPPUNIT_ASSERT_EQUAL( myArray5bis.getIJK(i,j,k), array5Ref[elemno] );
        CPPUNIT_ASSERT_EQUAL( myArray5Ptr[elemno], array5Ref[elemno] );
        elemno++;
      }

  //MEDMEM_Array()
  MEDMEM_Array<int,NoInterlaceGaussPolicy> myArray5ter;
  //test setPtr(ElementType * values, bool shallowCopy=false, bool ownershipOfValues=false)
  CPPUNIT_ASSERT_NO_THROW(myArray5.setPtr((int*)array5Ref));

  //test getPtr()
  myArray5Ptr = myArray5ter.getPtr();
  for (int i =0; i < myArray5ter.getArraySize(); i++) {
    CPPUNIT_ASSERT_EQUAL( myArray5Ptr[i], array5Ref[i]);
  }
}
