//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _MEDMEMTEST_HXX_
#define _MEDMEMTEST_HXX_

#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <string>
//#include <ostream>
#include <iostream>

namespace MEDMEM {
  class MESH;
};

class MEDMEMTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( MEDMEMTest );
  //0
  CPPUNIT_TEST( testArray );
  CPPUNIT_TEST( testArrayConvert );
    //CPPUNIT_TEST( testArrayInterface /* not in spec */ );
  CPPUNIT_TEST( testAsciiFieldDriver );
  //CPPUNIT_TEST( testCellModel /* not in spec */ );
  CPPUNIT_TEST( testConnectivity );
  CPPUNIT_TEST( testCoordinate );
  CPPUNIT_TEST( testDriverFactory );
  //CPPUNIT_TEST( testDriversDef /* not in spec */ );
  //10
  //CPPUNIT_TEST( testDriverTools /* not in spec */ );
  //CPPUNIT_TEST( testException /* not in spec */ );
  CPPUNIT_TEST( testFamily );
  CPPUNIT_TEST( testField );
  CPPUNIT_TEST( testFieldConvert );
  CPPUNIT_TEST( testFormulae );
  CPPUNIT_TEST( testGaussLocalization );
  CPPUNIT_TEST( testGibiMeshDriver );
  //20
  //CPPUNIT_TEST( testGrid );
  CPPUNIT_TEST( testGroup );
  CPPUNIT_TEST( testIndexCheckingPolicy );
  CPPUNIT_TEST( testInit /* cxx */ );
  //CPPUNIT_TEST( testInterlacingPolicy /* not in spec */ );
  //CPPUNIT_TEST( testInterlacingTraits /* not in spec */ );
  CPPUNIT_TEST( testMedFieldDriver21 );
  CPPUNIT_TEST( testMedFieldDriver22 );
  //CPPUNIT_TEST( testMedFieldDriver /* not in spec */ );
    CPPUNIT_TEST( testMed );
  //30
  CPPUNIT_TEST( testMedMedDriver21 );
  CPPUNIT_TEST( testMedMedDriver22 );
  //CPPUNIT_TEST( testMedMedDriver /* not in spec */ );
  //CPPUNIT_TEST( testMEDMEMchampLire /* not in spec */ );
  //CPPUNIT_TEST( testMEDMEMgaussEcr /* not in spec */ );
  //CPPUNIT_TEST( testMEDMEMprofilEcr /* not in spec */ );
   CPPUNIT_TEST( testMedMeshDriver21 );
  CPPUNIT_TEST( testMedMeshDriver22 );
  //CPPUNIT_TEST( testMedMeshDriver /* not in spec */ );
  CPPUNIT_TEST( testMedVersion );
  //40
  CPPUNIT_TEST( testMeshAndMeshing );
  CPPUNIT_TEST( testModulusArray );
  CPPUNIT_TEST( testnArray );
  CPPUNIT_TEST( testPointerOf );
  CPPUNIT_TEST( testPolyhedronArray );
  CPPUNIT_TEST( testPorflowMeshDriver );
  //CPPUNIT_TEST( testRCBase /* not in spec */ );
  //CPPUNIT_TEST( testSetInterlacingType /* not in spec */ );
  CPPUNIT_TEST( testSkyLineArray );
  //50
  CPPUNIT_TEST( testSTRING );
  CPPUNIT_TEST( testSupport );
  //CPPUNIT_TEST( testTags /* not in spec */ );
  //CPPUNIT_TEST( testTopLevel );
	//  CPPUNIT_TEST( testTypeMeshDriver );
  CPPUNIT_TEST( testUnit );
  CPPUNIT_TEST( testVtkFieldDriver );
  CPPUNIT_TEST( testVtkMedDriver );
  CPPUNIT_TEST( testVtkMeshDriver );
  //CPPUNIT_TEST( testmedimport_src /* not in spec */ );
  //60
  CPPUNIT_TEST( testExtractor );
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp();
  void tearDown();

  void testArray();
  void testArrayConvert();
  //void testArrayInterface() /* not in spec */;
  void testAsciiFieldDriver();
  //void testCellModel() /* not in spec */;
  void testConnectivity();
  void testCoordinate();
  void testDriverFactory();
  //void testDriversDef() /* not in spec */;
  //10
  //void testDriverTools() /* not in spec */;
  void testException() /* not in spec */;
  void testFamily();
  void testField();
  void testFieldConvert();
  void testFormulae();
  void testGaussLocalization();
  void testGibiMeshDriver();
  //20
  void testGrid();
  void testGroup();
  void testIndexCheckingPolicy();
  void testInit() /* cxx */;
  //void testInterlacingPolicy() /* not in spec */;
  //void testInterlacingTraits() /* not in spec */;
  void testMedFieldDriver21();
  void testMedFieldDriver22();
  //void testMedFieldDriver /* not in spec */();
  void testMed();
  //30
  void testMedMedDriver21();
  void testMedMedDriver22();
  //void testMedMedDriver() /* not in spec */;
  //void testMEDMEMchampLire() /* not in spec */;
  //void testMEDMEMgaussEcr() /* not in spec */;
  //void testMEDMEMprofilEcr() /* not in spec */;
  void testMedMeshDriver21();
  void testMedMeshDriver22();
  //void testMedMeshDriver() /* not in spec */;
  void testMedVersion();
  //40
  void testMeshAndMeshing();
  void testModulusArray();
  void testnArray();
  void testPointerOf();
  void testPolyhedronArray();
  void testPorflowMeshDriver();
  //void testRCBase() /* not in spec */;
  //void testSetInterlacingType /*  IMED_MESH_RDWR_DRIVER(driver), not in spec */();
  void testSkyLineArray();
  //50
  void testSTRING();
  void testSupport();
  //void testTags() /* not in spec */;
  void testTopLevel();
  void testTypeMeshDriver();
  void testUnit();
  void testVtkFieldDriver();
  void testVtkMedDriver();
  void testVtkMeshDriver();
  //void testmedimport_src() /* not in spec */;
  //60
  void testExtractor();
};

// to create a mesh with certain filling
MEDMEM::MESH * MEDMEMTest_createTestMesh();

// to automatically remove temporary files from disk
class MEDMEMTest_TmpFilesRemover
{
public:
  MEDMEMTest_TmpFilesRemover() {}
  ~MEDMEMTest_TmpFilesRemover();
  bool Register(const std::string theTmpFile);

private:
  std::set<std::string> myTmpFiles;
};

/*!
 *  Tool to print array to stream.
 */
template<class T>
void MEDMEMTest_DumpArray (std::ostream & stream, const T* array, const int length, const std::string text)
{
  stream << text << ": {";
  if (length > 0) {
    stream << array[0];
    for (int i = 1; i < length; i++) {
      stream << ", " << array[i];
    }
  }
  stream << "}" << std::endl;
};

#endif
