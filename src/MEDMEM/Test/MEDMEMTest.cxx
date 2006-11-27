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

////#include "MEDMEM_nArray.hxx"
////#include "MEDMEM_ArrayConvert.hxx"
////#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
//#include "MEDMEM_Connectivity.hxx"
////#include "MEDMEM_Coordinate.hxx"
//#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_IndexCheckingPolicy.hxx"
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
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
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

// ============================================================================
/*!
 *  Set up the environment
 */
// ============================================================================
void MEDMEMTest::setUp()
{
}

// ============================================================================
/*!
 *  - delete
 */
// ============================================================================
void MEDMEMTest::tearDown()
{
}

// #1: MEDMEM_Array.hxx            \
// #2: MEDMEM_ArrayConvert.hxx      }  MEDMEMTest_Array.cxx
// #3: MEDMEM_ArrayInterface.hxx   /
// #4: MEDMEM_AsciiFieldDriver.hxx  }  MEDMEMTest_AsciiFieldDriver.cxx

// #5: MEDMEM_CellModel.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_CellModel.hxx:
 *   {
 *   (yetno)
 *  }
 *
 *  Use code of MEDMEM/tests/testUCellModel.cxx
 */
void MEDMEMTest::testCellModel()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #6: MEDMEM_Compatibility21_22.hxx  }  nothing to test
// #7: MEDMEM_Connectivity.hxx        }  MEDMEMTest_Connectivity.cxx
// #8: MEDMEM_Coordinate.hxx          }  MEDMEMTest_Coordinate.cxx
// #9: MEDMEM_DriverFactory.hxx       }  MEDMEMTest_DriverFactory.cxx

// #10: MEDMEM_DriversDef.hxx         }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_DriversDef.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testDriversDef()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #11: MEDMEM_DriverTools.hxx   }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_DriverTools.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testDriverTools()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #12: MEDMEM_Exception.hxx          }  MEDMEMTest_Exception.cxx
// #13: MEDMEM_Family.hxx             }  MEDMEMTest.cxx
// #14: MEDMEM_Field.hxx              \
// #15: MEDMEM_FieldConvert.hxx       /  MEDMEMTest_Field.cxx
// #16: MEDMEM_FieldForward.hxx       }  no methods to test
// #17: MEDMEM_Formulae.hxx           }  MEDMEMTest_Formulae.cxx
// #18: MEDMEM_GaussLocalization.hxx  }  MEDMEMTest_GaussLocalization.cxx

// #19: MEDMEM_GenDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_GenDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testGenDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #20: MEDMEM_GibiMeshDriver.hxx  }  MEDMEMTest_GibiMeshDriver.cxx

// #21: MEDMEM_Grid.cxx  }  MEDMEMTest_Grid.cxx

// #22: MEDMEM_Group.cxx  }  MEDMEMTest_Group.cxx

// #23: MEDMEM_IndexCheckingPolicy.cxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (4), defined in MEDMEM_IndexCheckingPolicy.hxx:
 *  class IndexCheckPolicy {
 *   (+)     void checkMoreThanZero(const std::string & classname, int index) const;
 *   (+)     void checkLessOrEqualThan(const std::string & classname, int max, int index) const;
 *   (+)     void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const;
 *   (+)     void checkEquality(const std::string & classname, int a, int b) const;
 *  }
 *  class NoIndexCheckPolicy {
 *   (+)     void checkMoreThanZero(const string &classname, int index) const;
 *   (+)     void checkLessOrEqualThan(const std::string & classname, int max, int index) const;
 *   (+)     void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const;
 *   (+)     void checkEquality(const std::string & classname, int a, int b) const;
 *  }
 */
void MEDMEMTest::testIndexCheckingPolicy()
{
  MEDMEM::IndexCheckPolicy do_check;
  MEDMEM::NoIndexCheckPolicy no_check;

  int min = 3;
  int max = 10;

  string aClassName ("MEDMEMTest::testIndexCheckingPolicy");

  // IndexCheckPolicy: throw, if index does not satisfy criteria
  CPPUNIT_ASSERT_THROW(do_check.checkMoreThanZero(aClassName, -1), MEDMEM::MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(do_check.checkLessOrEqualThan(aClassName, max, 13), MEDMEM::MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(do_check.checkInInclusiveRange(aClassName, min, max, 1), MEDMEM::MEDEXCEPTION);
  // checkEquality() check that values are not equal
  CPPUNIT_ASSERT_THROW(do_check.checkEquality(aClassName, 21, 21), MEDMEM::MEDEXCEPTION);

  // NoIndexCheckPolicy: do not throw, if index does not satisfy criteria
  CPPUNIT_ASSERT_NO_THROW(no_check.checkMoreThanZero(aClassName, -1));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkLessOrEqualThan(aClassName, max, 13));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkInInclusiveRange(aClassName, min, max, 1));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkEquality(aClassName, 21, 21));

  // IndexCheckPolicy: do not throw, if index satisfy criteria
  CPPUNIT_ASSERT_NO_THROW(do_check.checkMoreThanZero(aClassName, 5));
  CPPUNIT_ASSERT_NO_THROW(do_check.checkLessOrEqualThan(aClassName, max, 7));
  CPPUNIT_ASSERT_NO_THROW(do_check.checkInInclusiveRange(aClassName, min, max, 6));
  CPPUNIT_ASSERT_NO_THROW(do_check.checkEquality(aClassName, 21, 28));

  // NoIndexCheckPolicy: do not throw, if index satisfy criteria
  CPPUNIT_ASSERT_NO_THROW(no_check.checkMoreThanZero(aClassName, 5));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkLessOrEqualThan(aClassName, max, 7));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkInInclusiveRange(aClassName, min, max, 6));
  CPPUNIT_ASSERT_NO_THROW(no_check.checkEquality(aClassName, 21, 28));
}

// #24: MEDMEM_Init.cxx  }  MEDMEMTest.cxx

/*!
 *  Check initialization, done in MEDMEM_Init.cxx:
 *  (-) MEDMEM::INIT init;
 */
void MEDMEMTest::testInit()
{
  // We cannot check here effect of initialization, done in MEDMEM_Init.cxx,
  // because environment variable, set there, is visible only in libmedmem.la
  // and its child processes (if any).

  // Check effect of MEDMEM_Init.cxx
  //char* traceKind = getenv("SALOME_trace");
  //CPPUNIT_ASSERT(traceKind);
}

// #25: MEDMEM_InterlacingPolicy.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingPolicy.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testInterlacingPolicy()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #26: MEDMEM_InterlacingTraits.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingTraits.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testInterlacingTraits()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #27: MEDMEM_MedFieldDriver21.hxx  }  MEDMEMTest_MedFieldDriver21.cxx

// #28: MEDMEM_MedFieldDriver22.hxx  }  MEDMEMTest_MedFieldDriver22.cxx

// #29: MEDMEM_MedFieldDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedFieldDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedFieldDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #30: MEDMEM_Med.hxx             }  MEDMEMTest_Med.cxx
// #31: MEDMEM_MedMedDriver21.hxx  }  MEDMEMTest_MedMedDriver21.cxx
// #32: MEDMEM_MedMedDriver22.hxx  }  MEDMEMTest_MedMedDriver22.cxx

// #33: MEDMEM_MedMedDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedMedDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedMedDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #34: MEDMEM_MEDMEMchampLire.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMchampLire.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMchampLire()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #35: MEDMEM_MEDMEMgaussEcr.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMgaussEcr.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMgaussEcr()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #36: MEDMEM_MEDMEMprofilEcr.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMprofilEcr.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMEDMEMprofilEcr()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #37: MEDMEM_MedMeshDriver21.hxx  }  MEDMEMTest_MedMeshDriver21.cxx
// #38: MEDMEM_MedMeshDriver22.hxx  }  MEDMEMTest_MedMeshDriver22.cxx

// #39: MEDMEM_MedMeshDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedMeshDriver.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testMedMeshDriver()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #40: MEDMEM_MedVersion.hxx         }  MEDMEMTest_MedVersion.cxx
// #41: MEDMEM_Mesh.hxx               \
// #42: MEDMEM_Meshing.hxx            /  MEDMEMTest_MeshAndMeshing.cxx
// #43: MEDMEM_ModulusArray.hxx       }  MEDMEMTest_ModulusArray.cxx
// #44: MEDMEM_nArray.hxx             }  MEDMEMTest_nArray.cxx
// #45: MEDMEM_PointerOf.hxx          }  MEDMEMTest_PointerOf.cxx
// #46: MEDMEM_PolyhedronArray.hxx    }  MEDMEMTest_PolyhedronArray.cxx
// #47: MEDMEM_PorflowMeshDriver.hxx  }  MEDMEMTest_PorflowMeshDriver.cxx

// #48: MEDMEM_RCBase.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_RCBase.hxx:
 *  class RCBASE {
 *   (reference counter presently disconnected in C++) virtual void addReference() const = 0;
 *   (reference counter presently disconnected in C++) virtual void removeReference() const = 0;
 *  }
 */
void MEDMEMTest::testRCBase()
{
  // nothing to test
}

// #49: MEDMEM_SetInterlacingType.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_SetInterlacingType.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testSetInterlacingType()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #50: MEDMEM_SkyLineArray.hxx  }  MEDMEMTest_SkyLineArray.cxx

// #51: MEDMEM_STRING.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (2), defined in MEDMEM_STRING.hxx:
 *  class STRING : public string {
 *   (yetno) STRING()
 *   (yetno) ~STRING()
 *   (yetno) operator const char * () const
 *   (yetno) template <class T> STRING(const T &valeur)
 *   (yetno) template <class T> STRING & operator<<(const T &valeur)
 *  }
 */
void MEDMEMTest::testSTRING()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #52: MEDMEM_Support.hxx  }  MEDMEMTest_Support.cxx

// #53: MEDMEM_Tags.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_Tags.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testTags()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

// #54: MEDMEM_TopLevel.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (6), defined in MEDMEM_TopLevel.hxx:
 *  (yetno) MED *readMedInFile(const std::string& fileName) throw(MEDEXCEPTION);
 *  (yetno) MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
 *  (yetno) template<class T> FIELD<T> *readFieldInFile
 *                            (const std::string& fileName, const std::string& fieldName);
 *  (yetno) void writeMedToFile(const MED *medObj, const std::string& fileName);
 *  (yetno) void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
 *  (yetno) template<class T> void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
 */
void MEDMEMTest::testTopLevel()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #55: MEDMEM_TypeMeshDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (15), defined in MEDMEM_TypeMeshDriver.hxx:
 *  class TYPE_MESH_DRIVER : public GENDRIVER {
 *   (yetno) TYPE_MESH_DRIVER();
 *   (yetno) TYPE_MESH_DRIVER(const string & fileName, MESH * ptrMesh, med_mode_acces accessMode);
 *   (yetno) TYPE_MESH_DRIVER(const TYPE_MESH_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_DRIVER();
 *   (yetno) void open() throw (MEDEXCEPTION);
 *   (yetno) void close() throw (MEDEXCEPTION);
 *   (yetno) virtual void write(void) const = 0;
 *   (yetno) virtual void read (void) = 0;
 *   (yetno) void   setMeshName(const string & meshName);
 *   (yetno) string getMeshName() const;
 *  }
 *  class TYPE_MESH_RDONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (yetno) TYPE_MESH_RDONLY_DRIVER();
 *   (yetno) TYPE_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_RDONLY_DRIVER(const TYPE_MESH_RDONLY_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_RDONLY_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_WRONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (yetno) TYPE_MESH_WRONLY_DRIVER();
 *   (yetno) TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_WRONLY_DRIVER(const TYPE_MESH_WRONLY_DRIVER & driver);
 *   (yetno) virtual ~TYPE_MESH_WRONLY_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_RDWR_DRIVER : public TYPE_MESH_RDONLY_DRIVER, public TYPE_MESH_WRONLY_DRIVER {
 *   (yetno) TYPE_MESH_RDWR_DRIVER();
 *   (yetno) TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (yetno) TYPE_MESH_RDWR_DRIVER(const TYPE_MESH_RDWR_DRIVER & driver);
 *   (yetno) ~TYPE_MESH_RDWR_DRIVER();
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testTypeMeshDriver()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #56: MEDMEM_Unit.hxx  }  MEDMEMTest_Unit.cxx


// #57: MEDMEM_VtkFieldDriver.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (14), defined in MEDMEM_VtkFieldDriver.hxx:
 *  template <class T> class VTK_FIELD_DRIVER : public GENDRIVER {
 *   (yetno) template <class INTERLACING_TAG> VTK_FIELD_DRIVER();
 *   (yetno) template <class INTERLACING_TAG> VTK_FIELD_DRIVER
 *               (const string & fileName, FIELD<T, INTERLACING_TAG> * ptrField)
 *   (yetno) VTK_FIELD_DRIVER(const VTK_FIELD_DRIVER & fieldDriver)
 *   (yetno) ~VTK_FIELD_DRIVER()
 *   (yetno) void openConst() const throw (MEDEXCEPTION)
 *   (yetno) void openConstAppend() const throw (MEDEXCEPTION)
 *   (yetno) void open() throw (MEDEXCEPTION)
 *   (yetno) void openAppend() throw (MEDEXCEPTION)
 *   (yetno) void closeConst() const throw (MEDEXCEPTION)
 *   (yetno) void close()
 *   (yetno) void setFieldName(const string & fieldName);
 *   (yetno) string getFieldName() const;
 *   (yetno) void read (void) throw (MEDEXCEPTION);
 *   (yetno) void write(void) const throw (MEDEXCEPTION);
 *   (yetno) void writeAppend(void) const throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testVtkFieldDriver()
{
  CPPUNIT_FAIL("Case Not Implemented");
}

// #58: MEDMEM_VtkMedDriver.hxx   }  MEDMEMTest_VtkMedDriver.cxx
// #59: MEDMEM_VtkMeshDriver.hxx  }  MEDMEMTest_VtkMeshDriver.cxx

// #60: MEDMEM_medimport_src.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (not in spec), defined in MEDMEM_medimport_src.hxx:
 *   {
 *   (yetno)
 *  }
 */
void MEDMEMTest::testmedimport_src()
{
  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
}

//60
//21 not in spec

//////////////////////////////////////
// HELP METHODS, used in many tests //
//////////////////////////////////////

/*!
 *  Create a mesh with certain filling
 *                 .n4
 *                 |
 *                 |
 *  n16,n12,n8.---------.n7,n11,n15
 *            |    |    |
 *            |    |    |
 *     n5.----|----.----|----.n3
 *            |    |    |
 *            |    |    |
 *  n17,n13,n9.---------.n10,n14,n18
 *                 |
 *                 |
 *                 .n6
 *
 *                 .n19 (0,0,5)
 *                 |
 *                 |
 *                 |
 *                 |
 *           n16.--|-.----.n15
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *       n17.----.----.n18
 *                 |
 *                 |
 *           n12.--|-.----.n11
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *       n13.----.----.n14
 *                 |
 *                 |
 *            n8.--|-.----.n7
 *             /   |/    /
 *            .    .    .
 *           /    /|   /
 *        n9.----.----.n10
 *                 |   .n4 (0,2,1)
 *                 |  /
 *                 | .
 *                 |/
 *     n5.----.----.----.----.n3 (2,0,1)
 *   (-2,0,1)     /n2 (0,0,1)
 *               . |
 *              /  |
 *             .n6 |
 *                 |
 *                 |
 *                 .n1 (0,0,0)
 */
MEDMEM::MESH * MEDMEMTest::createTestMesh ()
{
  // MESH DATA
  int SpaceDimension = 3;
  int meshDimension = SpaceDimension; // because there 3D cells in the mesh

  // coordinates
  int NumberOfNodes = 19;

  string Names[3] = { "X","Y","Z" };
  string Units[3] = { "cm","cm","cm" };

  double Coordinates[57] = {
     0.0,  0.0, 0.0, // n1
     0.0,  0.0, 1.0, // n2
     2.0,  0.0, 1.0, // n3
     0.0,  2.0, 1.0, // n4
    -2.0,  0.0, 1.0, // n5
     0.0, -2.0, 1.0, // n6
     1.0,  1.0, 2.0, // n7
    -1.0,  1.0, 2.0, // n8
    -1.0, -1.0, 2.0, // n9
     1.0, -1.0, 2.0, // n10
     1.0,  1.0, 3.0, // n11
    -1.0,  1.0, 3.0, // n12
    -1.0, -1.0, 3.0, // n13
     1.0, -1.0, 3.0, // n14
     1.0,  1.0, 4.0, // n15
    -1.0,  1.0, 4.0, // n16
    -1.0, -1.0, 4.0, // n17
     1.0, -1.0, 4.0, // n18
     0.0,  0.0, 5.0  // n19
  };

  // cells connectivities
  const int NumberOfCellTypes = 3;
  MED_EN::medGeometryElement CellTypes[NumberOfCellTypes] =
    {MED_EN::MED_TETRA4, MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};
  const int NumberOfCells[NumberOfCellTypes] = {12, 2, 2};

  int ConnectivityTetra[12 * 4] = {
    1,2,3,6,
    1,2,4,3,
    1,2,5,4,
    1,2,6,5,
    2,7,4,3,
    2,8,5,4,
    2,9,6,5,
    2,10,3,6,
    2,7,3,10,
    2,8,4,7,
    2,9,5,8,
    2,10,6,9
  };

  int ConnectivityPyra[2 * 5] = {
    7,8,9,10,2,
    15,18,17,16,19
  };

  int ConnectivityHexa[2 * 8] = {
    11,12,13,14,7,8,9,10,
    15,16,17,18,11,12,13,14
  };

  // faces connectivities
  const int NumberOfFaceTypes = 2;
  MED_EN::medGeometryElement FaceTypes[NumberOfFaceTypes] = {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4};
  const int NumberOfFaces[NumberOfFaceTypes] = {4, 4};

  int ConnectivityTria[4 * 3] = {
    1,4,3,
    1,5,4,
    1,6,5,
    1,3,6
  };

  int ConnectivityQua[4 * 4] = {
    7,8,9,10,
    11,12,13,14,
    11,7,8,12,
    12,8,9,13
  };


  // CREATE THE MESH
  MEDMEM::MESHING* myMeshing = new MEDMEM::MESHING;

  myMeshing->setName("meshing");

  myMeshing->setCoordinates(SpaceDimension, NumberOfNodes, Coordinates,
                           "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
  myMeshing->setCoordinatesNames(Names);
  myMeshing->setCoordinatesUnits(Units);

  // define connectivities
  myMeshing->setNumberOfTypes(NumberOfCellTypes, MED_EN::MED_CELL);
  myMeshing->setTypes(CellTypes, MED_EN::MED_CELL);
  myMeshing->setNumberOfElements(NumberOfCells, MED_EN::MED_CELL);

  myMeshing->setConnectivity(ConnectivityTetra, MED_EN::MED_CELL, MED_EN::MED_TETRA4);
  myMeshing->setConnectivity(ConnectivityPyra, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
  myMeshing->setConnectivity(ConnectivityHexa, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

  myMeshing->setNumberOfTypes(NumberOfFaceTypes, MED_EN::MED_FACE);
  myMeshing->setTypes(FaceTypes, MED_EN::MED_FACE);
  myMeshing->setNumberOfElements(NumberOfFaces, MED_EN::MED_FACE);

  myMeshing->setConnectivity(ConnectivityTria, MED_EN::MED_FACE, MED_EN::MED_TRIA3);
  myMeshing->setConnectivity(ConnectivityQua, MED_EN::MED_FACE, MED_EN::MED_QUAD4);

  // mesh dimension
  myMeshing->setMeshDimension(meshDimension);

  // edges connectivities
  // not yet implemented : if set, results are unpredictable.

  // groups of nodes
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeNodes");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {4};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,5};
    const int value[4] = {1,4,5,7};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherNodes");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_NODE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {3};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,4};
    const int value[3] = {2,3,6};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  // groups of cells
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeCells");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_CELL);
    myGroup.setNumberOfGeometricType(3);
    MED_EN::medGeometryElement myTypes[3] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5,MED_EN::MED_HEXA8};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[3] = {4,1,2};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[3+1] = {1,5,6,8};
    const int value[4+1+2] = {2,7,8,12,  13,  15,16};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherCells");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_CELL);
    myGroup.setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[] = {4,1};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,5,6};
    const int value[4+1] = {3,4,5,9,  14};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  // groups of faces
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("SomeFaces");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_FACE);
    myGroup.setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[2] = {MED_EN::MED_TRIA3,MED_EN::MED_QUAD4};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[2] = {2,3};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,3,6};
    const int value[2+3] = {2,4,  5,6,8};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }
  {
    MEDMEM::GROUP myGroup;
    myGroup.setName("OtherFaces");
    myGroup.setMesh(myMeshing);
    myGroup.setEntity(MED_EN::MED_FACE);
    myGroup.setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_TRIA3};
    myGroup.setGeometricType(myTypes);
    const int myNumberOfElements[1] = {2};
    myGroup.setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,3};
    const int value[2] = {1,3};
    myGroup.setNumber(index,value);
    myMeshing->addGroup(myGroup);
  }

  return myMeshing;
}
