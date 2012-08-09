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

#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_STRING.hxx"

#include <stdio.h>

#include <sstream>
#include <fstream>
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
 *  (+)     template<int N,unsigned int CODE> void fill(double *a, const double *b)
 *  (+)     template<int N> bool compare(const double* a, const double* b)
 *  (+)     template<> void fill<-1,0x3>(double *a, const double *b);
 *  (+)     template<> bool compare<-1>(const double *a, const double *b);
 *
 *  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
 *  class SDForSorting {
 *   (+)     SDForSorting(const double *coords, const T* comp, int nbComponents);
 *   (+)     SDForSorting(const SDForSorting& other);
 *   (+)     ~SDForSorting();
 *   (+)     bool operator< (const SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>& other) const;
 *   (+)     void writeLine(ofstream& file) const;
 *  }
 *
 *  template <class T>
 *  class ASCII_FIELD_DRIVER : public GENDRIVER {
 *
 *           //MUST BE PRIVATE as there is no possibility to set _ptrField after this constructor usage
 *   (-)     template <class INTERLACING_TAG> ASCII_FIELD_DRIVER();
 *
 *   (+)     template <class INTERLACING_TAG>
 *           ASCII_FIELD_DRIVER(const string & fileName, FIELD<T,INTERLACING_TAG> * ptrField,
 *                              MED_EN::med_sort_direc direction=MED_EN::ASCENDING, const char *priority="");
 *   (+)     ASCII_FIELD_DRIVER(const ASCII_FIELD_DRIVER<T>& other);
 *   (+)     void open() throw (MEDEXCEPTION);
 *   (+)     void close();
 *   (+)     void read (void) throw (MEDEXCEPTION);
 *   (+)     void write(void) const throw (MEDEXCEPTION);
 *   (+)     GENDRIVER* copy() const;
 *  }
 */
void MEDMEMTest::testAsciiFieldDriver()
{
  // read a mesh from a MED file
  string filename  = getResourceFile("pointe.med");
  string meshname  = "maa1";
  string fieldname = "fieldcelldoublescalar";

  string anyfile1    = makeTmpFile( "anyfile1" );
  string SDFfilename = makeTmpFile( "myfile" );
  ofstream aFile(SDFfilename.c_str());

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(anyfile1);
  aRemover.Register(SDFfilename);

  //Test SDForSorting class
  {
    double coord_1[10] = { 1.0, 2.0,
                           -1.0, 2.0,
                           3.6, -8.7,
                           10.0, -10.0,
                           12.3, 9.3};

    int comp_1[5] = {1, 3, 5, 7, 9};
    SDForSorting<int, 2, 48> aSDF_1(coord_1, comp_1, 5);

    SDForSorting<int, 2, 48> aSDFCpy_1 = SDForSorting<int, 2, 48>(aSDF_1);
    CPPUNIT_ASSERT_EQUAL(aSDFCpy_1 < aSDF_1, false);
    CPPUNIT_ASSERT_NO_THROW(aSDF_1.writeLine(aFile));
  }

  // Why functions
  // template<> void MEDMEM::fill<-1,0x3>(double *a, const double *b)
  // and
  // template<> bool MEDMEM::compare<-1>(const double *a, const double *b)
  // declared in MEDMEM_AsciiFieldDriver.hxx,
  // are implemented in MEDMEM_DriverFactory.cxx?

  // template<int N,unsigned int CODE> void fill(double *a, const double *b)
  {
    // 0x3 = 110000
    double aa[3];
    double bb[3] = {1,2,3};

    fill<2,198>(aa, bb); // ZYX // 11000110 // 012
    //MEDMEMTest_DumpArray<double>(cout, aa, 3, "aa filled with ZYX");
    CPPUNIT_ASSERT_EQUAL(aa[0], bb[0]);
    CPPUNIT_ASSERT_EQUAL(aa[1], bb[1]);
    CPPUNIT_ASSERT_EQUAL(aa[2], bb[2]);

    fill<2,210>(aa, bb); // ZXY // 11010010 // 102
    //MEDMEMTest_DumpArray<double>(cout, aa, 3, "aa filled with ZXY");
    CPPUNIT_ASSERT_EQUAL(aa[0], bb[1]);
    CPPUNIT_ASSERT_EQUAL(aa[1], bb[0]);
    CPPUNIT_ASSERT_EQUAL(aa[2], bb[2]);

    fill<2,228>(aa, bb); // XYZ // 11100100 // 210
    //MEDMEMTest_DumpArray<double>(cout, aa, 3, "aa filled with XYZ");
    CPPUNIT_ASSERT_EQUAL(aa[0], bb[2]);
    CPPUNIT_ASSERT_EQUAL(aa[1], bb[1]);
    CPPUNIT_ASSERT_EQUAL(aa[2], bb[0]);
  }

  // template<int N> bool compare(const double* a, const double* b)
  {
    {
      // aa < bb
      double aa[8] = {1,1,1,1,1,1,1,1};
      double bb[8] = {1,1,1,1,1,1,1,2};
      CPPUNIT_ASSERT(compare<7>(aa, bb));
    }

    {
      // aa < bb
      double aa[8] = {1,1,1,1,1,1,1,1};
      double bb[8] = {2,1,1,1,1,1,1,1};
      CPPUNIT_ASSERT(compare<7>(aa, bb));
    }

    {
      // aa > bb
      double aa[8] = {2,1,1,1,1,1,1,1};
      double bb[8] = {1,1,1,1,1,1,1,1};
      CPPUNIT_ASSERT(!compare<7>(aa, bb));
    }

    {
      // aa == bb
      double aa[8] = {1,1,1,1,1,1,1,1};
      double bb[8] = {1,1,1,1,1,1,1,1};
      CPPUNIT_ASSERT(!compare<7>(aa, bb));
    }

    {
      // compare<-1>
      double aa[8] = {2,1,1,1,1,1,1,1};
      double bb[8] = {1,1,1,1,1,1,1,1};
      CPPUNIT_ASSERT(!compare<-1>(aa, bb));
    }
  }

  // Test ASCII_FIELD_DRIVER
  FIELD<double> * aField1 = new FIELD<double> (MED_DRIVER, filename, fieldname);
  const SUPPORT * aSupport = aField1->getSupport();
  MESH * aMesh = new MESH(MED_DRIVER, filename, aSupport->getMeshName());
  aSupport->setMesh(aMesh);

  // create an ASCII driver for a field
  ASCII_FIELD_DRIVER<double> * aDriver1 =
    new ASCII_FIELD_DRIVER<double> (anyfile1, aField1, MED_EN::ASCENDING, "");
  CPPUNIT_ASSERT(aDriver1);

  CPPUNIT_ASSERT(aDriver1->getFileName() == anyfile1);
  CPPUNIT_ASSERT(aDriver1->getAccessMode() == MED_EN::WRONLY);

  // and write the field on disk

  // must throw because the file is not opened

  CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);

  aDriver1->open();

  // must throw because the file is opened
  CPPUNIT_ASSERT_THROW(aDriver1->setFileName("anyfile2"), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aDriver1->setFileName(anyfile1), MEDEXCEPTION);

  CPPUNIT_ASSERT_THROW(aDriver1->open(), MEDEXCEPTION);


  // must throw because it is a writeonly driver
  CPPUNIT_ASSERT_THROW(aDriver1->read(), MEDEXCEPTION);

  aDriver1->write();
  aDriver1->close();

  // must throw because the file is not opened

  CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);

  //CPPUNIT_ASSERT_THROW(aDriver1->close(), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW(aDriver1->close()); // do not make troubles to the user

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
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
                         ("anyfile5", aField1, MED_EN::ASCENDING, "ABC"), MEDEXCEPTION);
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
    CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
                         ("anyfile5", aField1, MED_EN::ASCENDING, "AB"), MEDEXCEPTION);
  }
  else {
    CPPUNIT_FAIL("Cannot test ASCII_FIELD_DRIVER because file pointe.med"
                 " contains mesh of wrong dimension: must be 2 or 3");
  }

  //Copy constructor
  ASCII_FIELD_DRIVER<double> aDriver1_Cpy1 = ASCII_FIELD_DRIVER<double> (*aDriver1);

  //Test copy() function
  ASCII_FIELD_DRIVER<double> *aDriver1_Cpy2 = (ASCII_FIELD_DRIVER<double>*)aDriver1->copy();
  CPPUNIT_ASSERT(aDriver1_Cpy2);
  delete aDriver1_Cpy2;
  // free memory
  delete aDriver1;
  aField1->removeReference();
  aMesh->removeReference();
}
