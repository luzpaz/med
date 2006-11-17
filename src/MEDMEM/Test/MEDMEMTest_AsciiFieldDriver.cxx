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
#include <cppunit/TestAssert.h>

#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_STRING.hxx"

#include <stdio.h>

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #4: MEDMEM_AsciiFieldDriver.hxx  }  MEDMEMTest_AsciiFieldDriver.cxx

/*!
 *  Check methods (8), defined in MEDMEM_AsciiFieldDriver.hxx:
 *
 *  (yetno) template<int N,unsigned int CODE> void fill(double *a, const double *b)
 *  (yetno) template<int N> bool compare(const double* a, const double* b)
 *  (yetno) template<> void fill<-1,0x3>(double *a, const double *b);
 *  (yetno) template<> bool compare<-1>(const double *a, const double *b);
 *
 *  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
 *  class SDForSorting {
 *   (yetno) SDForSorting(const double *coords, const T* comp, int nbComponents);
 *   (yetno) SDForSorting(const SDForSorting& other);
 *   (yetno) ~SDForSorting();
 *   (yetno) bool operator< (const SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>& other) const;
 *   (yetno) void writeLine(ofstream& file) const;
 *  }
 *
 *  template <class T>
 *  class ASCII_FIELD_DRIVER : public GENDRIVER {
 *   (yetno) template <class INTERLACING_TAG> ASCII_FIELD_DRIVER();
 *   (+)     template <class INTERLACING_TAG>
 *           ASCII_FIELD_DRIVER(const string & fileName, FIELD<T,INTERLACING_TAG> * ptrField,
 *                              MED_EN::med_sort_direc direction=MED_EN::ASCENDING, const char *priority="");
 *   (yetno) ASCII_FIELD_DRIVER(const ASCII_FIELD_DRIVER<T>& other);
 *   (+)     void open() throw (MEDEXCEPTION);
 *   (+)     void close();
 *   (+)     void read (void) throw (MEDEXCEPTION);
 *   (+)     void write(void) const throw (MEDEXCEPTION);
 *   (yetno) GENDRIVER* copy() const;
 *  }
 */
void MEDMEMTest::testAsciiFieldDriver()
{
  // read a mesh from a MED file
  string datadir   = getenv("DATA_DIR");
  string tmp_dir   = getenv("TMP");
  string filename  = datadir + "/MedFiles/pointe.med";
  string meshname  = "maa1";
  string fieldname = "fieldcelldouble";

  if (tmp_dir == "")
    tmp_dir = "/tmp";
  string anyfile1  = tmp_dir + "/anyfile1";

  FIELD<double> * aField1 = new FIELD<double> (MED_DRIVER, filename, fieldname);
  const SUPPORT * aSupport = aField1->getSupport();
  MESH * aMesh = new MESH (MED_DRIVER, filename, aSupport->getMeshName());
  aSupport->setMesh(aMesh);

  // create an ASCII driver for a field
  ASCII_FIELD_DRIVER<double> * aDriver1 =
    new ASCII_FIELD_DRIVER<double> (anyfile1, aField1, MED_EN::ASCENDING, "");
  CPPUNIT_ASSERT(aDriver1);

  CPPUNIT_ASSERT(aDriver1->getFileName() == anyfile1);
  CPPUNIT_ASSERT(aDriver1->getAccessMode() == MED_EN::MED_ECRI);

  // and write the field on disk

  // must throw because the file is not opened
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Invalid opened/closed state management
  CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aDriver1->close(), MEDEXCEPTION);
#endif

  aDriver1->open();

  // must throw because the file is opened
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Invalid opened/closed state management
  CPPUNIT_ASSERT_THROW(aDriver1->open(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aDriver1->setFileName("anyfile2"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aDriver1->setFileName(anyfile1), MEDEXCEPTION);
#endif

  // must throw because it is a writeonly driver
  CPPUNIT_ASSERT_THROW(aDriver1->read(), MEDEXCEPTION);

  aDriver1->write();
  aDriver1->close();

  // must throw because the file is not opened
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Invalid opened/closed state management
  CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aDriver1->close(), MEDEXCEPTION);
#endif

  // check priority definition
  int spaceDimension = aMesh->getSpaceDimension();
  if (spaceDimension == 3) {
    // good
    CPPUNIT_ASSERT_NO_THROW(ASCII_FIELD_DRIVER<double> aDriver2
                            ("anyfile2", aField1, MED_EN::ASCENDING, "XYZ"));
    // too long
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver3
                         ("anyfile3", aField1, MED_EN::ASCENDING, "XYZX"), MEDEXCEPTION);
    // too short
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver4
                         ("anyfile4", aField1, MED_EN::ASCENDING, "XY"), MEDEXCEPTION);
    // invalid
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) This assert fails because 'A'(and 'B', and 'C') < 'X'
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
                         ("anyfile5", aField1, MED_EN::ASCENDING, "ABC"), MEDEXCEPTION);
#endif
  }
  else if (spaceDimension == 2) {
    // good
    CPPUNIT_ASSERT_NO_THROW(ASCII_FIELD_DRIVER<double> aDriver2
                            ("anyfile2", aField1, MED_EN::ASCENDING, "XY"));
    // too long
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver3
                         ("anyfile3", aField1, MED_EN::ASCENDING, "XYZ"), MEDEXCEPTION);
    // too short
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver4
                         ("anyfile4", aField1, MED_EN::ASCENDING, "X"), MEDEXCEPTION);
    // invalid
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Invalid string is accepted for priority
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
                         ("anyfile5", aField1, MED_EN::ASCENDING, "AB"), MEDEXCEPTION);
#endif
  }
  else {
    CPPUNIT_FAIL("Cannot test ASCII_FIELD_DRIVER because file pointe.med"
                 " contains mesh of wrong dimension: must be 2 or 3");
  }

  // free memory
  delete aDriver1;

  delete aField1;
  delete aMesh;

  // remove temporary files from disk
  remove(anyfile1.c_str());

  CPPUNIT_FAIL("Case Not Complete.");
}
