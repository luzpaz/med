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

#include "MEDMEM_nArray.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;
/*!
 *  Check methods (21), defined in MEDMEM_nArray.hxx:
 *  class MEDMEM_Array_ {
 *   (yetno) virtual bool getGaussPresence() const { return false; }
 *   (yetno) virtual MED_EN::medModeSwitch getInterlacingType() const {return MED_EN::MED_UNDEFINED_INTERLACE;}
 *   (yetno) ~MEDMEM_Array_() {};
 *  }
 *  template<class ARRAY_ELEMENT_TYPE,
 *           class INTERLACING_POLICY=FullInterlaceNoGaussPolicy,
 *           class CHECKING_POLICY=IndexCheckPolicy>
 *  class MEDMEM_Array : public INTERLACING_POLICY, public CHECKING_POLICY, public MEDMEM_Array_ {
 *   (+) MEDMEM_Array();
 *   (yetno) ~MEDMEM_Array();
 *   (+) inline MEDMEM_Array(int dim, int nbelem);
 *   (yetno) inline MEDMEM_Array(ElementType * values, int dim, int nbelem,
 *                                bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+) inline MEDMEM_Array(int dim, int nbelem, int nbtypegeo,
 *                               const int * const  nbelgeoc, const int * const nbgaussgeo);
 *   (yetno) inline MEDMEM_Array(ElementType * values, int dim, int nbelem, int nbtypegeo,
 *                               const int * const  nbelgeoc, const int * const  nbgaussgeo,
 *                               bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+) inline MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false);
 *   (+) inline MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> &
 *                  operator=(const MEDMEM_Array & array);
 *   (+) MED_EN::medModeSwitch getInterlacingType() const;
 *   (yetno) bool getGaussPresence() const;
 *   (+) ElementType * getPtr();
 *   (yetno) void setPtr(ElementType * values, bool shallowCopy=false, bool ownershipOfValues=false);
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
  bool test1ok = true;
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

  MEDMEM_Array<double>  myArray1(mdim,nbelem1);

  for (int i =1; i <= nbelem1; i++)
    for (int j =1; j <= mdim; j++)
    {
      CPPUNIT_ASSERT_NO_THROW(myArray1.setIJ(i,j,i*10+j));
    }

  myArray1Ptr = myArray1.getPtr();

  for (int i = 0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

  MEDMEM_Array<double> myArray1bis(myArray1, false);
  CPPUNIT_ASSERT(myArray1 == myArray1bis);

  myArray1Ptr = myArray1bis.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray1Ptr[i] == array1Ref[i] );

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

  /////////////////////////////////////
  // TEST 2   NoInterlace et NoGauss //
  /////////////////////////////////////
  bool test2ok = true;
  const double * myArray2Ptr = 0;

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2(mdim,nbelem1);

  for (int i =1; i <= nbelem1; i++)
    for (int j =1; j <= mdim; j++){
      try{
	myArray2.setIJ(i,j,i*10+j);
      }
      catch (MEDMEM::MEDEXCEPTION &m) {
	CPPUNIT_FAIL(m.what());
      }
      catch (...)
      {
	CPPUNIT_FAIL("Unknown exception");
      }
    }

  myArray2Ptr = myArray2.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2bis(myArray2, false);
  myArray2Ptr = myArray2bis.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2ter;
  myArray2ter = myArray2;
  myArray2Ptr = myArray2ter.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2qua(mdim,nbelem1);
  myArray2Ptr = myArray2qua.getPtr();

  for (int j =1; j <= mdim; j++){
    try{
      myArray2qua.setColumn(j,&array2Ref[nbelem1*(j-1)]);
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
    CPPUNIT_ASSERT( myArray2Ptr[i] == array2Ref[i] );

  CPPUNIT_ASSERT_THROW(myArray2qua.getRow(1), MEDEXCEPTION);

  MEDMEM_Array<double,FullInterlaceNoGaussPolicy> * myArray2cin = ArrayConvert(myArray2);
  myArray2Ptr = myArray2cin->getPtr();
  elemno = 0;
  for (int  i=1; i <= nbelem1 ; i++ )
    for (int j=1; j <= mdim; j++) {
      CPPUNIT_ASSERT(  myArray2cin->getIJ(i,j) ==  array1Ref[elemno] );
      CPPUNIT_ASSERT(  myArray2Ptr[elemno]     ==  array1Ref[elemno] );
      elemno++;
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

  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
	for (int j=1; j <= mdim; j++) {
	  try{
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

  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3ter;
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

  for (int i =0; i < myArray3qua.getArraySize(); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray3Ptr[i], array3Ref[i], EPS );

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

  ////////////////////////////////////
  // TEST 4   NoInterlace et Gauss  //
  ////////////////////////////////////
  const double * myArray4Ptr = 0;

  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
	for (int j=1; j <= mdim; j++) {
	  try{
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

  myArray4Ptr = myArray4.getPtr();
  for (int i =0; i < myArray4.getArraySize(); i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[i], array4Ref[i], EPS );
  }

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

  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4ter;
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

  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4qua(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);
  myArray4Ptr = myArray4qua.getPtr();

  for (int j =1; j <= mdim; j++){
    try{
      myArray4qua.setColumn(j,&array4Ref[(myArray4qua.getArraySize()/mdim)*(j-1)]);
    }
    catch (MEDMEM::MEDEXCEPTION &m) {
      CPPUNIT_FAIL(m.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  }

  for (int i =0; i < myArray4qua.getArraySize(); i++)
    CPPUNIT_ASSERT_DOUBLES_EQUAL( myArray4Ptr[i], array4Ref[i], EPS );


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

  CPPUNIT_ASSERT( myArray4bis.getInterlacingType() != MED_UNDEFINED_INTERLACE);

  CPPUNIT_ASSERT_THROW(myArray4.getIJ(0,2), MEDEXCEPTION);

  //cout<<myArray4<<endl;
  ostringstream os;
  os << myArray4;
  CPPUNIT_ASSERT(os.str() != "");
}
