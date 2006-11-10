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

//#include "MEDMEM_nArray.hxx"
//#include "MEDMEM_ArrayConvert.hxx"
//#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Connectivity.hxx"
//#include "MEDMEM_Coordinate.hxx"
//#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
//#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_FieldForward.hxx"
//#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
//#include "MEDMEM_Group.hxx"
//#include "MEDMEM_IndexCheckingPolicy.hxx"
//#include "MEDMEM_InterlacingPolicy.hxx"
//#include "MEDMEM_InterlacingTraits.hxx"
//#include "MEDMEM_MedFieldDriver21.hxx"
//#include "MEDMEM_MedFieldDriver22.hxx"
//#include "MEDMEM_MedFieldDriver.hxx"
//#include "MEDMEM_Med.hxx"
//#include "MEDMEM_medimport_src.hxx"
//#include "MEDMEM_MedMedDriver21.hxx"
//#include "MEDMEM_MedMedDriver22.hxx"
//#include "MEDMEM_MedMedDriver.hxx"
//#include "MEDMEM_MEDMEMchampLire.hxx"
//#include "MEDMEM_MEDMEMgaussEcr.hxx"
//#include "MEDMEM_MEDMEMprofilEcr.hxx"
//#include "MEDMEM_MedMeshDriver21.hxx"
//#include "MEDMEM_MedMeshDriver22.hxx"
//#include "MEDMEM_MedMeshDriver.hxx"
//#include "MEDMEM_MedVersion.hxx"
//#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_Meshing.hxx"
//#include "MEDMEM_ModulusArray.hxx"
//#include "MEDMEM_PointerOf.hxx"
//#include "MEDMEM_PolyhedronArray.hxx"
//#include "MEDMEM_PorflowMeshDriver.hxx"
//#include "MEDMEM_RCBase.hxx"
//#include "MEDMEM_SetInterlacingType.hxx"
//#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_STRING.hxx"
//#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Tags.hxx"
//#include "MEDMEM_TopLevel.hxx"
//#include "MEDMEM_TypeMeshDriver.hxx"
//#include "MEDMEM_Unit.hxx"
//#include "MEDMEM_Utilities.hxx"
//#include "MEDMEM_VtkFieldDriver.hxx"
//#include "MEDMEM_VtkMedDriver.hxx"
//#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

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
  string filename  = datadir + "/MedFiles/pointe.med";
  string meshname  = "maa1";
  string fieldname = "fieldcelldouble";
  FIELD<double> * aField1 = new FIELD<double> (MED_DRIVER, filename, fieldname);
  const SUPPORT * aSupport = aField1->getSupport();
  MESH * aMesh = new MESH (MED_DRIVER, filename, aSupport->getMeshName());
  aSupport->setMesh(aMesh);

  // create an ASCII driver for a field
  ASCII_FIELD_DRIVER<double> * aDriver1 =
    new ASCII_FIELD_DRIVER<double> ("anyfile1", aField1, MED_EN::ASCENDING, "");
  CPPUNIT_ASSERT(aDriver1);

  CPPUNIT_ASSERT(aDriver1->getFileName() == "anyfile1");
  CPPUNIT_ASSERT(aDriver1->getAccessMode() == MED_EN::MED_ECRI);

  // and write the field on disk (in a current folder)

  // must throw because the file is not opened
  //CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aDriver1->close(), MEDEXCEPTION);

  aDriver1->open();

  // must throw because the file is opened
  //CPPUNIT_ASSERT_THROW(aDriver1->open(), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aDriver1->setFileName("anyfile2"), MEDEXCEPTION);

  // must throw because it is a writeonly driver
  CPPUNIT_ASSERT_THROW(aDriver1->read(), MEDEXCEPTION);

  aDriver1->write();
  aDriver1->close();

  // must throw because the file is not opened
  //CPPUNIT_ASSERT_THROW(aDriver1->write(), MEDEXCEPTION);
  //CPPUNIT_ASSERT_THROW(aDriver1->close(), MEDEXCEPTION);

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
    // invalid, but this assert fails because 'A'(and 'B', and 'C') < 'X'
    //CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
    //                     ("anyfile5", aField1, MED_EN::ASCENDING, "ABC"), MEDEXCEPTION);
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
    //CPPUNIT_ASSERT_THROW(ASCII_FIELD_DRIVER<double> aDriver5
    //                     ("anyfile5", aField1, MED_EN::ASCENDING, "AB"), MEDEXCEPTION);
  }
  else {
    CPPUNIT_FAIL("Cannot test ASCII_FIELD_DRIVER because file pointe.med"
                 " contains mesh of wrong dimension: must be 2 or 3");
  }

  // free memory
  delete aDriver1;

  delete aField1;
  delete aMesh;

  // TO DO: remove anyfile1 from disk after test
  CPPUNIT_FAIL("Case Not Complete. And uncomment asserts.");
}
