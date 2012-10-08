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

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_IndexCheckingPolicy.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Remapper.hxx"

#include <sstream>
#include <cmath>
#include <stdexcept>

#ifdef WIN32
#include <windows.h>
#endif

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

/*
// #1: MEDMEM_Array.hxx                \
// #2: MEDMEM_ArrayConvert.hxx          }  MEDMEMTest_Array.cxx
// #3: MEDMEM_ArrayInterface.hxx (-)   /
// #4: MEDMEM_AsciiFieldDriver.hxx      }  MEDMEMTest_AsciiFieldDriver.cxx
// #5: MEDMEM_CellModel.hxx             }  MEDMEMTest.cxx (-)
// #7: MEDMEM_Connectivity.hxx          }  MEDMEMTest_Connectivity.cxx
// #8: MEDMEM_Coordinate.hxx            }  MEDMEMTest_Coordinate.cxx
// #9: MEDMEM_DriverFactory.hxx         }  MEDMEMTest_DriverFactory.cxx
// #10: MEDMEM_DriversDef.hxx           }  MEDMEMTest.cxx (-)
// #11: MEDMEM_DriverTools.hxx          }  MEDMEMTest.cxx (-)
// #12: MEDMEM_Exception.hxx            }  MEDMEMTest_Exception.cxx
// #13: MEDMEM_Family.hxx               }  MEDMEMTest.cxx
// #14: MEDMEM_Field.hxx                \
// #15: MEDMEM_FieldConvert.hxx         /  MEDMEMTest_Field.cxx
// #16: MEDMEM_FieldForward.hxx         }  no methods to test
// #17: MEDMEM_Formulae.hxx             }  MEDMEMTest_Formulae.cxx
// #18: MEDMEM_GaussLocalization.hxx    }  MEDMEMTest_GaussLocalization.cxx
// #19: MEDMEM_GenDriver.hxx  }  abstract class; implemented methods are tested during its successors testing
// #20: MEDMEM_GibiMeshDriver.hxx       }  MEDMEMTest_GibiMeshDriver.cxx
// #21: MEDMEM_Grid.cxx                 }  MEDMEMTest_Grid.cxx
// #22: MEDMEM_Group.cxx                }  MEDMEMTest_Group.cxx
// #23: MEDMEM_IndexCheckingPolicy.cxx  }  MEDMEMTest.cxx
// #24: MEDMEM_Init.cxx                 }  MEDMEMTest.cxx
// #25: MEDMEM_InterlacingPolicy.hxx    }  MEDMEMTest.cxx (-)
// #26: MEDMEM_InterlacingTraits.hxx    }  MEDMEMTest.cxx (-)
// #28: MEDMEM_MedFieldDriver.hxx       }  MEDMEMTest_MedFieldDriver.cxx
// #34: MEDMEM_MEDMEMchampLire.hxx      }  MEDMEMTest.cxx (-)
// #35: MEDMEM_MEDMEMgaussEcr.hxx       }  MEDMEMTest.cxx (-)
// #36: MEDMEM_MEDMEMprofilEcr.hxx      }  MEDMEMTest.cxx (-)
// #37: MEDMEM_MedMeshDriver.hxx        }  MEDMEMTest_MedMeshDriver.cxx
// #40: MEDMEM_MedVersion.hxx           }  MEDMEMTest_MedVersion.cxx
// #41: MEDMEM_Mesh.hxx                 \
// #42: MEDMEM_Meshing.hxx              /  MEDMEMTest_MeshAndMeshing.cxx
// #43: MEDMEM_ModulusArray.hxx         }  MEDMEMTest_ModulusArray.cxx
// #44: MEDMEM_nArray.hxx               }  MEDMEMTest_nArray.cxx
// #45: MEDMEM_PointerOf.hxx            }  MEDMEMTest_PointerOf.cxx
// #47: MEDMEM_PorflowMeshDriver.hxx    }  MEDMEMTest_PorflowMeshDriver.cxx
// #48: MEDMEM_RCBase.hxx               }  MEDMEMTest.cxx (-)
// #49: MEDMEM_SetInterlacingType.hxx   }  MEDMEMTest.cxx (-)
// #50: MEDMEM_SkyLineArray.hxx         }  MEDMEMTest_SkyLineArray.cxx
// #51: MEDMEM_STRING.hxx               }  MEDMEMTest.cxx
// #52: MEDMEM_Support.hxx              }  MEDMEMTest_Support.cxx
// #53: MEDMEM_Tags.hxx                 }  MEDMEMTest.cxx (-)
// #54: MEDMEM_TopLevel.hxx             }  MEDMEMTest_TopLevel.cxx
// #55: MEDMEM_TypeMeshDriver.hxx       }  MEDMEMTest.cxx
// #56: MEDMEM_Unit.hxx                 }  MEDMEMTest_Unit.cxx
// #57: MEDMEM_VtkFieldDriver.hxx       }  MEDMEMTest_VtkFieldDriver.cxx
// #58: MEDMEM_VtkMedDriver.hxx         }  MEDMEMTest_VtkMedDriver.cxx
// #59: MEDMEM_VtkMeshDriver.hxx        }  MEDMEMTest_VtkMeshDriver.cxx
// #60: MEDMEM_medimport_src.hxx        }  MEDMEMTest.cxx (-)
*/

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

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingPolicy.hxx:
 */
//void MEDMEMTest::testInterlacingPolicy()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_InterlacingTraits.hxx:
 */
//void MEDMEMTest::testInterlacingTraits()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedFieldDriver.hxx:
 */
//void MEDMEMTest::testMedFieldDriver()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMchampLire.hxx:
 */
//void MEDMEMTest::testMEDMEMchampLire()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMgaussEcr.hxx:
 */
//void MEDMEMTest::testMEDMEMgaussEcr()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_MEDMEMprofilEcr.hxx:
 */
//void MEDMEMTest::testMEDMEMprofilEcr()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedMeshDriver.hxx:
 */
//void MEDMEMTest::testMedMeshDriver()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_RCBase.hxx:
 *  class RCBASE {
 *   (reference counter presently disconnected in C++) virtual void addReference() const = 0;
 *   (reference counter presently disconnected in C++) virtual void removeReference() const = 0;
 *  }
 */
//void MEDMEMTest::testRCBase()
//{
//  // nothing to test
//}

/*!
 *  Check methods (not in spec), defined in MEDMEM_SetInterlacingType.hxx:
 */
//void MEDMEMTest::testSetInterlacingType()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (2), defined in MEDMEM_STRING.hxx:
 *  class STRING : public string {
 *   (+)     STRING()
 *   (+)     ~STRING()
 *   (+)     operator const char * () const
 *   (+)     template <class T> STRING(const T &valeur)
 *   (+)     template <class T> STRING & operator<<(const T &valeur)
 *  }
 */
void MEDMEMTest::testSTRING()
{
  {
    // empty constructor
    MEDMEM::STRING medstr;
    CPPUNIT_ASSERT(strcmp(medstr.c_str(), "") == 0);

    // template <class T> STRING & operator<<(const T &valeur) with T == char*
    //medstr << <char*>"filling 1";
    medstr << "filling " << 1 << " !!!";
    CPPUNIT_ASSERT(strcmp(medstr.c_str(), "filling 1 !!!") == 0);

    // operator const char * ()
    CPPUNIT_ASSERT(strcmp((const char *)medstr, "filling 1 !!!") == 0);
    CPPUNIT_ASSERT(strcmp(              medstr, "filling 1 !!!") == 0);
  }

  MEDMEM::MESH * aMesh1 = MEDMEMTest_createTestMesh();
  ostringstream ostr;
  ostr << *aMesh1;

  {
    // template <class T> STRING & operator<<(const T &valeur) with T == MEDMEM::MESH
    MEDMEM::STRING medstr;
    //medstr << <char*>"filling 1";
    medstr << *aMesh1;
    CPPUNIT_ASSERT(strcmp(medstr, ostr.str().c_str()) == 0);
  }

  {
    // Constructor STRING(const T &valeur) with T == char*
    //MEDMEM::STRING medstr<char*>("filling 2");
    MEDMEM::STRING medstr ("filling 2");
    CPPUNIT_ASSERT(strcmp(medstr.c_str(), "filling 2") == 0);
    CPPUNIT_ASSERT(strcmp(medstr, "filling 2") == 0);
  }

  {
    // Constructor STRING(const T &valeur) with T == MEDMEM::MESH
    MEDMEM::STRING medstr (*aMesh1);
    CPPUNIT_ASSERT(strcmp(medstr, ostr.str().c_str()) == 0);
  }
  if(aMesh1)
    aMesh1->removeReference();
}

/*!
 *  Check methods (not in spec), defined in MEDMEM_Tags.hxx:
 */
//void MEDMEMTest::testTags()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

/*!
 *  Check methods (15), defined in MEDMEM_TypeMeshDriver.hxx:
 *  class TYPE_MESH_DRIVER : public GENDRIVER {
 *   (-) TYPE_MESH_DRIVER();
 *   (-) TYPE_MESH_DRIVER(const string & fileName, MESH * ptrMesh, med_mode_acces accessMode);
 *   (-) TYPE_MESH_DRIVER(const TYPE_MESH_DRIVER & driver);
 *   (-) virtual ~TYPE_MESH_DRIVER();
 *   (-) void open() throw (MEDEXCEPTION);
 *   (-) void close() throw (MEDEXCEPTION);
 *   (-) virtual void write(void) const = 0;
 *   (-) virtual void read (void) = 0;
 *   (-) void   setMeshName(const string & meshName);
 *   (-) string getMeshName() const;
 *  }
 *  class TYPE_MESH_RDONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (-) TYPE_MESH_RDONLY_DRIVER();
 *   (-) TYPE_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (-) TYPE_MESH_RDONLY_DRIVER(const TYPE_MESH_RDONLY_DRIVER & driver);
 *   (-) virtual ~TYPE_MESH_RDONLY_DRIVER();
 *   (-) void write(void) const throw (MEDEXCEPTION);
 *   (-) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_WRONLY_DRIVER : public virtual TYPE_MESH_DRIVER {
 *   (-) TYPE_MESH_WRONLY_DRIVER();
 *   (-) TYPE_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (-) TYPE_MESH_WRONLY_DRIVER(const TYPE_MESH_WRONLY_DRIVER & driver);
 *   (-) virtual ~TYPE_MESH_WRONLY_DRIVER();
 *   (-) void write(void) const throw (MEDEXCEPTION);
 *   (-) void read (void) throw (MEDEXCEPTION);
 *  }
 *  class TYPE_MESH_RDWR_DRIVER : public TYPE_MESH_RDONLY_DRIVER, public TYPE_MESH_WRONLY_DRIVER {
 *   (-) TYPE_MESH_RDWR_DRIVER();
 *   (-) TYPE_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh);
 *   (-) TYPE_MESH_RDWR_DRIVER(const TYPE_MESH_RDWR_DRIVER & driver);
 *   (-) ~TYPE_MESH_RDWR_DRIVER();
 *   (-) void write(void) const throw (MEDEXCEPTION);
 *   (-) void read (void) throw (MEDEXCEPTION);
 *  }
 */
void MEDMEMTest::testTypeMeshDriver()
{
  CPPUNIT_FAIL("TYPE_MESH_DRIVER: not implemented");
}

/*!
 *  Check methods (not in spec), defined in MEDMEM_medimport_src.hxx:
 */
//void MEDMEMTest::testmedimport_src()
//{
//  CPPUNIT_FAIL("Case Not Implemented (not in spec)");
//}

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
MEDMEM::MESH * MEDMEMTest_createTestMesh ()
{
  // MESH DATA
  int SpaceDimension = 3;

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

  myMeshing->setConnectivity(MED_EN::MED_CELL, MED_EN::MED_TETRA4, ConnectivityTetra );
  myMeshing->setConnectivity(MED_EN::MED_CELL, MED_EN::MED_PYRA5 , ConnectivityPyra  );
  myMeshing->setConnectivity(MED_EN::MED_CELL, MED_EN::MED_HEXA8 , ConnectivityHexa  );

  myMeshing->setNumberOfTypes(NumberOfFaceTypes, MED_EN::MED_FACE);
  myMeshing->setTypes(FaceTypes, MED_EN::MED_FACE);
  myMeshing->setNumberOfElements(NumberOfFaces, MED_EN::MED_FACE);

  myMeshing->setConnectivity(MED_EN::MED_FACE, MED_EN::MED_TRIA3,ConnectivityTria);
  myMeshing->setConnectivity(MED_EN::MED_FACE, MED_EN::MED_QUAD4,ConnectivityQua);

  // edges connectivities
  // not yet implemented : if set, results are unpredictable.

  // groups of nodes
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("SomeNodes");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_NODE);
    myGroup->setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = {4};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,5};
    const int value[4] = {1,4,5,7};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("OtherNodes");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_NODE);
    myGroup->setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_NONE};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = {3};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,4};
    const int value[3] = {2,3,6};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }

  // groups of cells
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("SomeCells");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_CELL);
    myGroup->setNumberOfGeometricType(3);
    MED_EN::medGeometryElement myTypes[3] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5,MED_EN::MED_HEXA8};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[3] = {4,1,2};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[3+1] = {1,5,6,8};
    const int value[4+1+2] = {2,7,8,12,  13,  15,16};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("OtherCells");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_CELL);
    myGroup->setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[] = {MED_EN::MED_TETRA4,MED_EN::MED_PYRA5};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[] = {4,1};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,5,6};
    const int value[4+1] = {3,4,5,9,  14};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }

  // groups of faces
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("SomeFaces");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_FACE);
    myGroup->setNumberOfGeometricType(2);
    MED_EN::medGeometryElement myTypes[2] = {MED_EN::MED_TRIA3,MED_EN::MED_QUAD4};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[2] = {2,3};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = {1,3,6};
    const int value[2+3] = {2,4,  5,6,8};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }
  {
    MEDMEM::GROUP *myGroup=new MEDMEM::GROUP;
    myGroup->setName("OtherFaces");
    myGroup->setMesh(myMeshing);
    myGroup->setEntity(MED_EN::MED_FACE);
    myGroup->setNumberOfGeometricType(1);
    MED_EN::medGeometryElement myTypes[1] = {MED_EN::MED_TRIA3};
    myGroup->setGeometricType(myTypes);
    const int myNumberOfElements[1] = {2};
    myGroup->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = {1,3};
    const int value[2] = {1,3};
    myGroup->setNumber(index,value);
    myMeshing->addGroup(*myGroup);
    myGroup->removeReference();
  }

  return myMeshing;
}

/*!
 *  Tool to remove temporary files.
 *  Allows automatique removal of temporary files in case of test failure.
 */
MEDMEMTest_TmpFilesRemover::~MEDMEMTest_TmpFilesRemover()
{
  set<string>::iterator it = myTmpFiles.begin();
  for (; it != myTmpFiles.end(); it++) {
#ifdef WIN32
    //if (GetFileAttributes((*it).data()) & FILE_ATTRIBUTE_NORMAL)
    if (GetFileAttributes((*it).data()) != INVALID_FILE_ATTRIBUTES)
#else
    if (access((*it).data(), F_OK) == 0)
#endif
      remove((*it).data());
  }
  myTmpFiles.clear();
  //cout << "~MEDMEMTest_TmpFilesRemover()" << endl;
}

bool MEDMEMTest_TmpFilesRemover::Register(const string theTmpFile)
{
  return (myTmpFiles.insert(theTmpFile)).second;
}
