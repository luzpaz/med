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

#include "MEDMEM_nArray.hxx"
#include "MEDMEM_ArrayConvert.hxx"
#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_STRING.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #1,2,3: MEDMEMTest_Array.cxx
// Check methods from MEDMEM_Array.hxx, MEDMEM_ArrayConvert.hxx, MEDMEM_ArrayInterface.hxx

/*!
 *  Check methods (20), defined in MEDMEM_Array.hxx:
 *  template <class T> class MEDARRAY {
 *   (+)     inline  MEDARRAY();
 *   (+)     inline ~MEDARRAY();
 *   (+)     MEDARRAY (const int ld_values, const int length_values,
 *             const MED_EN::medModeSwitch mode=MED_EN::MED_FULL_INTERLACE);
 *   (+)     MEDARRAY (T* values, const int ld_values,
 *             const int length_values, const MED_EN::medModeSwitch mode=MED_EN::MED_FULL_INTERLACE,
 *             bool shallowCopy=false, bool ownershipOfValues=false);
 *   (+)     MEDARRAY (MEDARRAY const &m);
 *   (+)     MEDARRAY (MEDARRAY const &m, bool copyOther);
 *   (+)     MEDARRAY & operator = (const MEDARRAY & m);
 *   (+)     MEDARRAY & shallowCopy(const MEDARRAY & m);
 *   (+)     inline int getLeadingValue() const;
 *   (+)     inline int getLengthValue()  const;
 *   (+)     const T * get        (const MED_EN::medModeSwitch mode);
 *   (+)     const T * getRow     (const int i);
 *   (+)     const T * getColumn  (const int j);
 *   (+)     const T   getIJ (const int i, const int j) const;
 *   (+)     inline MED_EN::medModeSwitch getMode() const;
 *   (+)     void set   (const MED_EN::medModeSwitch mode,const T* value);
 *   (+)     void setI  (const int i, const T* value);
 *   (+)     void setJ  (const int j, const T* value);
 *   (+)     void setIJ (const int i, const int j, const T  value);
 *   (+)     void calculateOther();
 *   (+)     bool isOtherCalculated() const {return (const T*)_valuesOther != NULL;}
 *   (+)     void clearOtherMode();
 *  }
 *
 *  Use code of MEDMEM/tests/testUArray.cxx,
 *              MEDMEM/test_MEDMEM_Array.cxx
 */
void MEDMEMTest::testArray()
{
  int SpaceDimension = 3;
  int NumberOfNodes  = 4;

  ////////////
  // TEST 1 //
  ////////////

  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                      INITIALISATION                        */
  /*                                                            */
  /* - constructeur new MEDARRAY<int>(int,int,mode)             */
  /* - get sur le pointeur MED_FULL_INTERLACE                   */
  /* - initialisation des valeurs (coordonnées ieme elt = I+1)  */
  /* - get sur le pointeur MED_NO_INTERLACE                     */
  /* ---------------------------------------------------------- */

  MEDARRAY<int> * myArrayfull =
    new MEDARRAY<int> (SpaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);
  CPPUNIT_ASSERT(myArrayfull != NULL);

  const int * myValues = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
  CPPUNIT_ASSERT(myValues != NULL);

  for (int i = 0; i < NumberOfNodes; i++) {
    for (int k = 0; k < SpaceDimension; k++) {
      myArrayfull->setIJ(i+1, k+1, i+1);
    }
  }

  const int * myOthers = myArrayfull->get(MED_EN::MED_NO_INTERLACE);
  CPPUNIT_ASSERT(myOthers != NULL);

  MEDARRAY<int> * myArrayno = new MEDARRAY<int>(SpaceDimension, NumberOfNodes, MED_EN::MED_NO_INTERLACE);
  CPPUNIT_ASSERT(myArrayno != NULL);
  const int * myValuesno = myArrayno->get(MED_EN::MED_NO_INTERLACE);
  CPPUNIT_ASSERT(myValuesno != NULL);

  for (int k = 0; k < SpaceDimension; k++) {
    for (int i = 0; i < NumberOfNodes; i++) {
      myArrayno->setIJ(i+1, k+1, k+1);
    }
  }

  const int * myOthersno = myArrayno->get(MED_EN::MED_FULL_INTERLACE);
  CPPUNIT_ASSERT(myOthersno != NULL);

  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                  Tests des Fonctions Set                   */
  /*                                                            */
  /* - setI sur l'element 1 avec des coordonnees a 100          */
  /* - setJ sur l'element 1 avec des coordonnees a 100          */
  /* - setIJ sur (1,2) avec une coordonnee = 1992               */
  /* - set   avec l ensemble des coordonnes remises à i         */
  /* ---------------------------------------------------------- */

  int * myNewLine = new int[SpaceDimension];
  for (int i = 0; i < SpaceDimension; i++)
    myNewLine[i] = myValues[i] * 100;
  try
  {
    myArrayfull->setI(1, myNewLine);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception in myArrayfull->setI()");
  }

  delete [] myNewLine;

  int * myNewCol = new int[NumberOfNodes];
  for (int i = 0; i < NumberOfNodes; i++)
    myNewCol[i] = 100;
  try
  {
    myArrayno->setJ(1, myNewCol);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception in myArrayno->setJ()");
  }

  delete [] myNewCol;

  try
  {
    myArrayfull->setIJ(1, 2, 1992);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception in myArrayfull->setIJ()");
  }

  try
  {
    myArrayno->setIJ(1, 2, 1992);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception in myArrayno->setIJ()");
  }

  int * mynewvalues = new int [ NumberOfNodes*SpaceDimension ];
  for (int i = 0; i < NumberOfNodes*SpaceDimension; i++) {
    mynewvalues[i] = i;
  }
  try
  {
    myArrayfull->set(MED_EN::MED_FULL_INTERLACE, mynewvalues);
    myValues = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
    myOthers = myArrayfull->get(MED_EN::MED_NO_INTERLACE);
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myArrayno->set(MED_EN::MED_FULL_INTERLACE, mynewvalues);
    myValuesno = myArrayfull->get(MED_EN::MED_FULL_INTERLACE);
    myOthersno = NULL;
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  /* ---------------------------------------------------------- */
  /*                                                            */
  /*                Tests des constructeurs                     */
  /*                Tests des Fonctions Get                     */
  /*                                                            */
  /* ---------------------------------------------------------- */

  // check sharing
  MEDARRAY<int> * myArrayShare = new MEDARRAY<int>(*myArrayfull);
  const int * sharevalues = myArrayShare->get(MED_EN::MED_FULL_INTERLACE);
  const int * shareno = myArrayShare->get(MED_EN::MED_NO_INTERLACE);

  myArrayfull->setIJ(1, 2, 1993);
  CPPUNIT_ASSERT(myArrayShare->getIJ(1,2) == 1993);

  myArrayShare->setIJ(1, 2, 1995);
  CPPUNIT_ASSERT(myArrayfull->getIJ(1,2) == 1995);

  delete myArrayShare;

  // check no sharing
  MEDARRAY<int> * myArrayShare2 = new MEDARRAY<int>(*myArrayfull, true);
  sharevalues = myArrayShare2->get(MED_EN::MED_FULL_INTERLACE);
  shareno = myArrayShare2->get(MED_EN::MED_NO_INTERLACE);

  myArrayfull->setIJ(1, 2, 18);
  CPPUNIT_ASSERT(myArrayShare2->getIJ(1,2) == 1995);
  myArrayShare2->setIJ(1, 2, 19);
  CPPUNIT_ASSERT(myArrayfull->getIJ(1,2) == 18);

  // check myArrayno
  myArrayno->set(MED_EN::MED_NO_INTERLACE, mynewvalues);
  myArrayno->setIJ(2, 1, 1);
  myValuesno = myArrayno->get(MED_EN::MED_NO_INTERLACE);
  myOthersno = myArrayno->get(MED_EN::MED_FULL_INTERLACE);

  MEDARRAY<int> * myArrayShare3 = new MEDARRAY<int>(*myArrayno);
  sharevalues = myArrayShare3->get(MED_EN::MED_FULL_INTERLACE);
  shareno = myArrayShare3->get(MED_EN::MED_NO_INTERLACE);

  myArrayno->setIJ(1, 2, 1992);
  CPPUNIT_ASSERT(myArrayShare3->getIJ(1,2) == 1992);

  myArrayShare3->setIJ(1, 2, 1995);
  CPPUNIT_ASSERT(myArrayno->getIJ(1,2) == 1995);

  delete myArrayno;
  delete [] mynewvalues;
  delete myArrayfull;
  delete myArrayShare2;
  delete myArrayShare3;


  ////////////
  // TEST 2 //
  ////////////

  MEDARRAY<int> * myArray =
    new MEDARRAY<int> (SpaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);

  // - 1 1 1
  // - 2 2 2
  // - 3 3 3
  // - 4 4 4
  for (int i = 1; i <= NumberOfNodes; i++)
    for (int j = 1; j <= SpaceDimension; j++)
      myArray->setIJ(i, j, i);

  CPPUNIT_ASSERT(myArray->getLengthValue()  == NumberOfNodes);
  CPPUNIT_ASSERT(myArray->getLeadingValue() == SpaceDimension);

  int numberof = myArray->getLeadingValue(); // nb. of columns ~ space dimension
  for (int i = 1; i <= myArray->getLengthValue(); i++) {
    const int * node = myArray->getRow(i);
    for (int j = 0; j < numberof; j++) {
      CPPUNIT_ASSERT(node[j] == i);
    }
  }

  numberof = myArray->getLengthValue(); // nb. of rows ~ nb. of nodes
  for (int i = 1; i <= myArray->getLeadingValue(); i++) {
    const int * node = myArray->getColumn(i);
    for (int j = 0; j < numberof; j++) {
      CPPUNIT_ASSERT(node[j] == j + 1);
    }
  }

  numberof = myArray->getLeadingValue();
  for (int i = 1; i <= myArray->getLengthValue(); i++) {
    for (int j = 1; j < numberof + 1; j++) {
      CPPUNIT_ASSERT(myArray->getIJ(i,j) == i);
    }
  }

  numberof = myArray->getLeadingValue();
  int length = myArray->getLengthValue();
  const int * NoInterlaceArray = myArray->get(MED_EN::MED_NO_INTERLACE);
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < numberof; j++) {
      CPPUNIT_ASSERT(NoInterlaceArray[j*length+i] == i + 1);
    }
  }

  delete myArray;


  ////////////
  // TEST 3 //
  ////////////

  // MEDARRAY (T* values, const int ld_values,
  //           const int length_values, const MED_EN::medModeSwitch mode=MED_EN::MED_FULL_INTERLACE,
  //           bool shallowCopy=false, bool ownershipOfValues=false);

  int ld = 7, nb = 4;
  double rr = 13.0, h0 = 7.0;
  double * aValues = new double [ld * nb];
  for (int nn = 0; nn < nb; nn++) {
    aValues[nn*ld + 0] = nn;
    aValues[nn*ld + 1] = rr * cos(nn * M_PI/4.);
    aValues[nn*ld + 2] = rr * sin(nn * M_PI/4.);
    aValues[nn*ld + 3] = h0 + nn;
    aValues[nn*ld + 4] = 2.0 * rr * cos(nn * M_PI/4.);
    aValues[nn*ld + 5] = 2.0 * rr * sin(nn * M_PI/4.);
    aValues[nn*ld + 6] = h0 - nn;
  }

  MEDARRAY<double> * aSpiral =
    new MEDARRAY<double> (aValues, ld, nb, MED_EN::MED_FULL_INTERLACE,
                          /*shallowCopy=*/false, /*ownershipOfValues=*/false);
                          ///*shallowCopy=*/true, /*ownershipOfValues=*/true);

  if (aSpiral->isOtherCalculated())
    aSpiral->clearOtherMode();
  CPPUNIT_ASSERT(!aSpiral->isOtherCalculated());

  CPPUNIT_ASSERT(aSpiral->getMode() == MED_EN::MED_FULL_INTERLACE);
  aSpiral->calculateOther();
  CPPUNIT_ASSERT(aSpiral->getMode() == MED_EN::MED_FULL_INTERLACE);
  CPPUNIT_ASSERT(aSpiral->getLeadingValue() == ld);
  CPPUNIT_ASSERT(aSpiral->getLengthValue() == nb);

  //CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, delta);
  double delta = 0.000001;
  const double* comp5 = aSpiral->getColumn(5);
  for (int nn = 0; nn < nb; nn++) {
    const double* pnt = aSpiral->getRow(nn + 1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0 * rr * cos(nn * M_PI/4.), pnt[4], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(comp5[nn],                    pnt[4], delta);
  }

  // Check operator= and method shallowCopy()
  double c7n3 = aSpiral->getIJ(3, 7);
  MEDARRAY<double> shc, cpy;

  //#ifdef ENABLE_FAULTS
  // (BUG) MEDARRAY<T>::shallowCopy() usage leads to memory leaks,
  // because on attemp to free both arrays we have Segmentation Fault

  // EAP: It's misusage of shallowcopy - BUT what is a correct usage ???
  // TODO: check ownership of qrgument
  //???skl shc.shallowCopy(*aSpiral);

  // hangs up
  //MEDARRAY<double> arr (10, 10);
  //MEDARRAY<double> shc;
  //shc.shallowCopy(arr);

  // Segmentation fault
  //MEDARRAY<double>* arr = new MEDARRAY<double> (10, 10);
  //MEDARRAY<double>* shc = new MEDARRAY<double> (10, 10);
  //shc->shallowCopy(*arr);
  //delete arr;
  //delete shc;
  //#endif
  //#ifdef ENABLE_FORCED_FAILURES
  //  CPPUNIT_FAIL("MEDARRAY<T>::shallowCopy(): wrong memory management");
  //#endif

  cpy = *aSpiral;

  aSpiral->setIJ(3, 7, c7n3 * 2.0);

  //#ifdef ENABLE_FAULTS
  //???skl CPPUNIT_ASSERT_DOUBLES_EQUAL(c7n3 * 2.0, shc.getIJ(3, 7), delta);
  //#endif
        
  //CPPUNIT_ASSERT_DOUBLES_EQUAL(c7n3 * 2.0, cpy.getIJ(3, 7), delta);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(c7n3, cpy.getIJ(3, 7), delta);

// #ifdef ENABLE_FORCED_FAILURES
//   // Comments to deep copy constructor do not correspond to actual implementation
//   CPPUNIT_FAIL("Comments to MEDARRAY<T> deep copy constructor do not correspond to actual implementation");
// #endif

  aSpiral->clearOtherMode();
  //CPPUNIT_ASSERT_THROW(MEDARRAY<double> co (*aSpiral, true), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(MEDARRAY<double> co (*aSpiral, true));

  // invalid cases
  //double arr3 [3] = {1., 2., 3.};
  double arr4 [4] = {1., 2., 3., 4.};
  double arr7 [7] = {1., 2., 3., 4., 5., 6., 7.};
  CPPUNIT_ASSERT_THROW(aSpiral->getRow(0), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->getRow(nb + 1), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->getColumn(0), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->getColumn(ld + 1), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aSpiral->set(MED_EN::MED_FULL_INTERLACE, NULL), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->setI(-1, arr7), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aSpiral->setI(1, NULL), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->setJ(-1, arr4), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aSpiral->setJ(1, NULL), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->setIJ(-1, -1, 13.), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aSpiral->setIJ(1, 0, 13.), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aSpiral->setI(1, arr3), MEDEXCEPTION); // user must control itself

  delete aSpiral;

  // check empty array

  MEDARRAY<double> anEmptyArray;
  //CPPUNIT_ASSERT_NO_THROW(MEDARRAY<double> anArr1 (anEmptyArray)); // INTERRUPTION
  //CPPUNIT_ASSERT_NO_THROW(MEDARRAY<double> anArr2 (anEmptyArray, false)); // fails
  //CPPUNIT_ASSERT_NO_THROW(MEDARRAY<double> anArr4 = anEmptyArray); // INTERRUPTION
  //MEDARRAY<double> anArr5;
  //CPPUNIT_ASSERT_NO_THROW(anArr5.shallowCopy(anEmptyArray));

  // invalid cases
  CPPUNIT_ASSERT_THROW(anEmptyArray.get(MED_EN::MED_FULL_INTERLACE), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.get(MED_EN::MED_NO_INTERLACE), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.getRow(0), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.getRow(5), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.getColumn(0), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.getIJ(0, 0), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.getIJ(1, 1), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(anEmptyArray.set(MED_EN::MED_FULL_INTERLACE, aValues));
  CPPUNIT_ASSERT_THROW(anEmptyArray.setI(1, aValues), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.setIJ(0, 0, 3.), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(anEmptyArray.setIJ(1, 1, 3.), MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(MEDARRAY<double> bad (-1, 10), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(MEDARRAY<double> bad (10, -1), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(MEDARRAY<double> bad (-1, -1), MEDEXCEPTION);

  delete [] aValues;

  // construction with taking ownership of values
  int ld1 = 7, nb1 = 4;
  double * val1 = new double [ld * nb];
  for (int nn = 0; nn < nb1; nn++) {
    for (int cc = 0; cc < ld1; cc++) {
      val1[nn*ld1 + cc] = nn;
    }
  }
  MEDARRAY<double> * arr1 =
    new MEDARRAY<double> (val1, ld1, nb1, MED_EN::MED_FULL_INTERLACE,
                          /*shallowCopy=*/true, /*ownershipOfValues=*/true);
  for (int nn = 1; nn <= nb1; nn++) {
    for (int cc = 1; cc <= ld1; cc++) {
      CPPUNIT_ASSERT_DOUBLES_EQUAL(nn - 1, arr1->getIJ(nn, cc), delta);
    }
  }
  delete arr1;
}

/*!
 *  Check methods (4), defined in MEDMEM_ArrayConvert.hxx:
 *
 *  (+) Gauss NoInterlace->FullInterlace:
 *  template <class T, class CHECKING_POLICY >
 *  MEDMEM_Array<T, FullInterlaceGaussPolicy, CHECKING_POLICY> *
 *  ArrayConvert(const MEDMEM_Array< T, NoInterlaceGaussPolicy, CHECKING_POLICY > &array, T* values=0)
 *
 *  (+) Gauss FullInterlace->NoInterlace:
 *  template <class T, class CHECKING_POLICY>
 *  MEDMEM_Array<T, NoInterlaceGaussPolicy, CHECKING_POLICY> *
 *  ArrayConvert(const MEDMEM_Array< T, FullInterlaceGaussPolicy, CHECKING_POLICY > &array, T* values=0)
 *
 *  (+) NoGauss FullInterlace->NoInterlace:
 *  template <class T, class CHECKING_POLICY>
 *  MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> *
 *  ArrayConvert(const MEDMEM_Array< T, FullInterlaceNoGaussPolicy, CHECKING_POLICY > &array, T* values=0)
 *
 *  (+) NoGauss NoInterlace->FullInterlace:
 *  template <class T, class CHECKING_POLICY>
 *  MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> *
 *  ArrayConvert(const MEDMEM_Array< T, NoInterlaceNoGaussPolicy, CHECKING_POLICY > &array, T* values=0)
 *
 *  Use code of MEDMEM/test_MEDMEM_nArray.cxx
 */
void MEDMEMTest::testArrayConvert()
{
  // TEST n°1 : FullInterlaceNoGaussPolicy -> NoInterlaceNoGaussPolicy

  const int mdim = 2;
  const int nbelem1 = 20;

  const double array1Ref[] = { 11 , 12 , 21 , 22 , 31 , 32 , 41 , 42 , 51 , 52 ,
                               61 , 62 , 71 , 72 , 81 , 82 , 91 , 92 , 101, 102,
                               111, 112, 121, 122, 131, 132, 141, 142, 151, 152,
                               161, 162, 171, 172, 181, 182, 191, 192, 201, 202 };

  const double array2Ref[] = { 11 , 21 , 31 , 41 , 51 , 61 , 71 , 81 , 91 , 101,
                               111, 121, 131, 141, 151, 161, 171, 181, 191, 201,
                               12 , 22 , 32 , 42 , 52 , 62 , 72 , 82 , 92 , 102,
                               112, 122, 132, 142, 152, 162, 172, 182, 192, 202 };

  MEDMEM_Array<double> myArray1 (mdim, nbelem1);

  for (int i = 1; i <= nbelem1; i++)
    for (int j = 1; j <= mdim; j++)
      myArray1.setIJ(i, j, i*10+j);

  const double * myArray1Ptr = myArray1.getPtr();
  for (int i = 0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT(myArray1Ptr[i] == array1Ref[i]);

  MEDMEM_Array<double, NoInterlaceNoGaussPolicy> * myArray1cin = ArrayConvert(myArray1);
  myArray1Ptr = myArray1cin->getPtr();
  int elemno = 0;
  for (int j = 1; j <= mdim; j++) {
    for (int  i = 1; i <= nbelem1; i++) {
      CPPUNIT_ASSERT(myArray1cin->getIJ(i,j) == array2Ref[elemno]);
      CPPUNIT_ASSERT(myArray1Ptr[elemno]     == array2Ref[elemno]);
      elemno++;
    }
  }
  delete myArray1cin;
  // TEST n°2 : NoInterlaceNoGaussPolicy -> FullInterlaceNoGaussPolicy

  MEDMEM_Array<double, NoInterlaceNoGaussPolicy> myArray2 (mdim, nbelem1);

  for (int i = 1; i <= nbelem1; i++)
    for (int j = 1; j <= mdim; j++)
      myArray2.setIJ(i, j, i*10+j);

  const double * myArray2Ptr = myArray2.getPtr();
  for (int i = 0; i < mdim*nbelem1; i++)
    CPPUNIT_ASSERT(myArray2Ptr[i] == array2Ref[i]);

  MEDMEM_Array<double, FullInterlaceNoGaussPolicy> * myArray2cin = ArrayConvert(myArray2);
  myArray2Ptr = myArray2cin->getPtr();
  elemno = 0;
  for (int i = 1; i <= nbelem1; i++) {
    for (int j = 1; j <= mdim; j++) {
      CPPUNIT_ASSERT(myArray2cin->getIJ(i,j) == array1Ref[elemno]);
      CPPUNIT_ASSERT(myArray2Ptr[elemno]     == array1Ref[elemno]);
      elemno++;
    }
  }
  delete myArray2cin;
  // TEST n°3 : FullInterlaceGaussPolicy -> NoInterlaceGaussPolicy

  const int nbelem2 = 5;
  const int nbtypegeo = 2;
  const int nbelgeoc[nbtypegeo+1]   = {1,3,6};
  const int nbgaussgeo[nbtypegeo+1] = {-1,2,3};

  const double * myArray3Ptr = 0;
  const double array3Ref[] = {  1.11,  1.12,  1.21,  1.22,
                                2.11,  2.12,  2.21,  2.22,
                               13.11, 13.12, 13.21, 13.22, 13.31, 13.32,
                               14.11, 14.12, 14.21, 14.22, 14.31, 14.32,
                               15.11, 15.12, 15.21, 15.22, 15.31, 15.32 };

  const double array4Ref[] = {  1.11,  1.21,  2.11,  2.21,
                               13.11, 13.21, 13.31, 14.11, 14.21, 14.31,
                               15.11, 15.21, 15.31,
                                1.12,  1.22,  2.12,  2.22,
                               13.12, 13.22, 13.32, 14.12, 14.22, 14.32,
                               15.12, 15.22, 15.32 };

  MEDMEM_Array<double, FullInterlaceGaussPolicy> myArray3
    (mdim, nbelem2, nbtypegeo, nbelgeoc, nbgaussgeo);

  elemno = 1;
  for (int ntyp = 1; ntyp <= nbtypegeo; ntyp++) {
    for (int i = 0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]); i++) {
      for (int k = 1; k <= nbgaussgeo[ntyp]; k++) {
        for (int j = 1; j <= mdim; j++) {
          myArray3.setIJK(elemno, j, k, elemno+(ntyp-1)*10+0.1*k+0.01*j);
        }
      }
      elemno++;
    }
  }
  myArray3Ptr = myArray3.getPtr();
  for (int i = 0; i < myArray3.getArraySize(); i++)
    CPPUNIT_ASSERT( abs(myArray3Ptr[i] - array3Ref[i]) < 1e-7);

  MEDMEM_Array<double, NoInterlaceGaussPolicy> * myArray3cin = ArrayConvert(myArray3);
  myArray3Ptr = myArray3cin->getPtr();
  elemno = 0;
  for (int j = 1; j <= mdim; j++)
    for (int i = 1; i < nbelgeoc[nbtypegeo]; i++)
      for (int k = 1; k <= myArray3cin->getNbGauss(i); k++) {
        CPPUNIT_ASSERT( abs(myArray3cin->getIJK(i,j,k) - array4Ref[elemno]) < 1e-7);
        CPPUNIT_ASSERT( abs(myArray3Ptr[elemno] - array4Ref[elemno]) < 1e-7);
        elemno++;
      }
  delete myArray3cin;
  // TEST n°4 : NoInterlaceGaussPolicy -> FullInterlaceGaussPolicy

  const double * myArray4Ptr = 0;

  MEDMEM_Array<double, NoInterlaceGaussPolicy> myArray4
    (mdim, nbelem2, nbtypegeo, nbelgeoc, nbgaussgeo);

  elemno = 1;
  for (int ntyp = 1; ntyp <= nbtypegeo; ntyp++) {
    for (int i = 0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]); i++) {
      for (int k = 1; k <= nbgaussgeo[ntyp]; k++) {
        for (int j = 1; j <= mdim; j++) {
          myArray4.setIJK(elemno, j, k, elemno+(ntyp-1)*10+0.1*k+0.01*j);
        }
      }
      elemno++;
    }
  }
  myArray4Ptr = myArray4.getPtr();

  for (int i = 0; i < myArray4.getArraySize(); i++)
    CPPUNIT_ASSERT( abs(myArray4Ptr[i] - array4Ref[i]) < 1e-7);

  MEDMEM_Array<double, FullInterlaceGaussPolicy> * myArray4cin = ArrayConvert(myArray4);
  myArray4Ptr = myArray4cin->getPtr();
  elemno = 0;
  for (int i = 1; i < nbelgeoc[nbtypegeo]; i++)
    for (int k = 1; k <= myArray4cin->getNbGauss(i); k++)
      for (int j = 1; j <= mdim; j++) {
        CPPUNIT_ASSERT( abs(myArray4cin->getIJK(i,j,k) - array3Ref[elemno]) < 1e-7);
        CPPUNIT_ASSERT( abs(myArray4cin->getIJK(i,j,k) - myArray3.getIJK(i,j,k)) < 1e-7);
        CPPUNIT_ASSERT( abs(myArray4Ptr[elemno] - array3Ref[elemno]) < 1e-7);
        elemno++;
      }
  delete myArray4cin;
}
/*!
 *  Check methods (not in spec), defined in MEDMEM_ArrayInterface.hxx:
 */
//void MEDMEMTest::testArrayInterface()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}
