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

#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_STRING.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #18: MEDMEM_GaussLocalization.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (13), defined in MEDMEM_GaussLocalization.hxx:
 *  class GAUSS_LOCALIZATION_ {
 *   (+)     virtual MED_EN::medModeSwitch getInterlacingType() const;
 *   (+)     virtual ~GAUSS_LOCALIZATION_();
 *  }
 *  (+)     template <class INTERLACING_TAG=FullInterlace> class GAUSS_LOCALIZATION;
 *
 *  (+)     template <class INTERLACING_TAG> ostream & operator<<
 *                (ostream &os, const GAUSS_LOCALIZATION<INTERLACING_TAG> &loc);
 *
 *  template <class INTERLACING_TAG> class GAUSS_LOCALIZATION : public GAUSS_LOCALIZATION_ {
 *   (+)     GAUSS_LOCALIZATION() throw (MEDEXCEPTION);
 *   (+)     GAUSS_LOCALIZATION(const string & locName,
 *                              const MED_EN::medGeometryElement typeGeo,
 *                              const int  nGauss,
 *                              const ArrayNoGauss & cooRef,
 *                              const ArrayNoGauss & cooGauss,
 *                              const vector<double>  & wg) throw (MEDEXCEPTION);
 *   (+)     GAUSS_LOCALIZATION(const string & locName,
 *                              const MED_EN::medGeometryElement  typeGeo,
 *                              const int  nGauss,
 *                              const double  * const cooRef,
 *                              const double  * const cooGauss,
 *                              const double  * const wg) throw (MEDEXCEPTION);
 *   (+)     virtual ~GAUSS_LOCALIZATION();
 *
 *   (+)     GAUSS_LOCALIZATION & operator=(const GAUSS_LOCALIZATION & gaussLoc);
 *   (+)     bool operator == (const GAUSS_LOCALIZATION &loc) const;
 *
 *   (+)     string          getName()    const;
 *   (+)     MED_EN::medGeometryElement getType() const;
 *
 *   (+)     int             getNbGauss() const;
 *   (+)     ArrayNoGauss    getRefCoo () const;
 *   (+)     ArrayNoGauss    getGsCoo  () const;
 *   (+)     vector <double> getWeight () const;
 *
 *   (+)     inline MED_EN::medModeSwitch  getInterlacingType() const;
 *  }
 */
void MEDMEMTest::testGaussLocalization()
{
  // GAUSS_LOCALIZATION_
  GAUSS_LOCALIZATION_ aGL_;
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_UNDEFINED_INTERLACE, aGL_.getInterlacingType());

  // GAUSS_LOCALIZATION empty constructor and InterlaceType
  GAUSS_LOCALIZATION<> anEmptyGL1;
  MED_EN::medModeSwitch aDefaultInterlaceType = anEmptyGL1.getInterlacingType();
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FULL_INTERLACE, aDefaultInterlaceType);

  GAUSS_LOCALIZATION<NoInterlace> anEmptyGL2;
  MED_EN::medModeSwitch anInterlaceType2 = anEmptyGL2.getInterlacingType();
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_NO_INTERLACE, anInterlaceType2);

  // Construction from C++ arrays
  double cooRef[6] = {1.,1., 2.,4., 3.,9.}; // xy xy xy
  double cooGauss[10] = {7.,7., 6.,6., 5.,5., 4.,3., 2.,1.}; // x1,y1  x2,y2  x3,y3  x4,y4  x5,y5
  double wg[5] = {1., 2., 3., 4., 5.};

  GAUSS_LOCALIZATION<> gl1 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss, wg);
  GAUSS_LOCALIZATION<> gl2 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss, wg);

  // getXXX
  CPPUNIT_ASSERT(gl1.getName() == "GL1");
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_TRIA3, gl1.getType());
  CPPUNIT_ASSERT_EQUAL(5, gl1.getNbGauss());

  GAUSS_LOCALIZATION<>::ArrayNoGauss cooRefBack   = gl1.getRefCoo();
  GAUSS_LOCALIZATION<>::ArrayNoGauss cooGaussBack = gl1.getGsCoo();
  vector<double> wgBack = gl1.getWeight();

  double * cooRefBack_ptr = cooRefBack.getPtr();
  double * cooGaussBack_ptr = cooGaussBack.getPtr();

  // (BUG)Reading uninitialized memory here
  // TODO fix
  for (int i = 0; i < 10; i++) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(cooRef[i%6], cooRefBack_ptr[i%6], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(cooGauss[i], cooGaussBack_ptr[i], 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(wg[i%5]    , wgBack[i%5]        , 0.000001);
  }

  // Operators == and =
  CPPUNIT_ASSERT(gl1 == gl2);
  CPPUNIT_ASSERT_EQUAL(gl1, gl2);

  GAUSS_LOCALIZATION<> gl1_c;
  gl1_c = gl1;

  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Badly copyed arrays as a result of operator= usage
  CPPUNIT_ASSERT_EQUAL(gl1, gl1_c);
  // Invalid read of size 8
  //    at 0x342B157A: MEDMEM::MEDMEM_Array<double, MEDMEM::FullInterlaceNoGaussPolicy, MEDMEM::IndexCheckPolicy>::operator==(MEDMEM::MEDMEM_Array<double, MEDMEM::FullInterlaceNoGaussPolicy, MEDMEM::IndexCheckPolicy> const&) const (MEDMEM_nArray.hxx:255)
  //    by 0x342A1CDA: MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace>::operator==(MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&) const (MEDMEM_GaussLocalization.hxx:175)
  //    by 0x34315F98: CppUnit::assertion_traits<MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> >::equal(MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&, MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&) (TestAssert.h:40)
  //    by 0x34314D35: void CppUnit::assertEquals<MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> >(MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&, MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&, CppUnit::SourceLine, std::string const&) (TestAssert.h:62)
  //    by 0x34311D6F: MEDMEMTest::testGaussLocalization() (MEDMEMTest_GaussLocalization.cxx:132)
  //  Address 0x35B5FC98 is 0 bytes inside a block of size 48 free'd
  //    at 0x3414CD61: operator delete[](void*) (vg_replace_malloc.c:161)
  //    by 0x341D88CA: MEDMEM::PointerOf<double>::~PointerOf() (MEDMEM_PointerOf.hxx:141)
  //    by 0x341D56C7: MEDMEM::MEDMEM_Array<double, MEDMEM::FullInterlaceNoGaussPolicy, MEDMEM::IndexCheckPolicy>::~MEDMEM_Array() (MEDMEM_nArray.hxx:52)
  //    by 0x34314F58: MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace>::operator=(MEDMEM::GAUSS_LOCALIZATION<MEDMEM::FullInterlace> const&) (MEDMEM_GaussLocalization.hxx:166)
  //    by 0x34311CCC: MEDMEMTest::testGaussLocalization() (MEDMEMTest_GaussLocalization.cxx:128)
  //#endif

  // same name, type and nb.gauss, but different coords or weights
  double cooRef_ch[6] = {1.,1., 2.,8., 3.,9.};
  double cooGauss_ch[10] = {7.,8., 6.,6., 5.,5., 4.,3., 2.,1.};
  double wg_ch[5] = {1., 2., 8., 4., 5.};

  GAUSS_LOCALIZATION<> gl3 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef_ch, cooGauss, wg);
  GAUSS_LOCALIZATION<> gl4 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss_ch, wg);
  GAUSS_LOCALIZATION<> gl5 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss, wg_ch);

  CPPUNIT_ASSERT(!(gl1 == gl3));
  CPPUNIT_ASSERT(!(gl1 == gl4));
  CPPUNIT_ASSERT(!(gl1 == gl5));

  // different name or type, or nb.gauss
  double cooRef_quad[8] = {1.,1., 2.,4., 3.,9., 4., 16.};
  double cooGauss_4[8] = {7.,8., 6.,6., 5.,5., 4.,3.};
  double wg_4[4] = {1., 2., 8., 4.};

  GAUSS_LOCALIZATION<> gl6 ("GL6", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef, cooGauss, wg);
  GAUSS_LOCALIZATION<> gl7 ("GL1", MED_EN::MED_QUAD4, /*nGauss*/5, cooRef_quad, cooGauss, wg);
  GAUSS_LOCALIZATION<> gl8 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/4, cooRef, cooGauss_4, wg_4);

  CPPUNIT_ASSERT(!(gl1 == gl6));
  CPPUNIT_ASSERT(!(gl1 == gl7));
  CPPUNIT_ASSERT(!(gl1 == gl8));

  // Operator <<
  ostringstream ostr1;
  ostr1 << gl1;
  CPPUNIT_ASSERT(ostr1.str() != "");

  ostringstream ostr2;
  ostr2 << gl2;
  CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr2.str());

  // Construction from ArrayNoGauss and vector<double>
  GAUSS_LOCALIZATION<> gl9 ("GL1", MED_EN::MED_TRIA3, /*nGauss*/5, cooRefBack, cooGaussBack, wgBack);
  CPPUNIT_ASSERT(gl1 == gl9);

  // NoInterlace
  double cooRef_ni[6] = {1.,2.,3., 1.,4.,9.}; // xxx yyy
  double cooGauss_ni[10] = {7.,6.,5.,4.,2., 7.,6.,5.,3.,1.}; // x1,x2,x3,x4,x5  y1,y2,y3,y4,y5

  GAUSS_LOCALIZATION<NoInterlace> gl10 ("GL10", MED_EN::MED_TRIA3, /*nGauss*/5, cooRef_ni, cooGauss_ni, wg);

  GAUSS_LOCALIZATION<NoInterlace>::ArrayNoGauss cooRefBack_ni   = gl10.getRefCoo();
  GAUSS_LOCALIZATION<NoInterlace>::ArrayNoGauss cooGaussBack_ni = gl10.getGsCoo();

  for (int i = 1; i <= 3; i++) { // nb. nodes
    for (int j = 1; j <= 2; j++) { // dim
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cooRefBack.getIJ(i, j), cooRefBack_ni.getIJ(i, j), 0.000001);
    }
  }

  for (int i = 1; i <= 5; i++) { // nGauss
    for (int j = 1; j <= 2; j++) { // dim
      CPPUNIT_ASSERT_DOUBLES_EQUAL(cooGaussBack.getIJ(i, j), cooGaussBack_ni.getIJ(i, j), 0.000001);
    }
  }
}
