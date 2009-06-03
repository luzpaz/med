//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#ifndef _MEDSPLITTERTEST_HXX_
#define _MEDSPLITTERTEST_HXX_

#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <string>
#include <iostream>

namespace MEDSPLITTER {
  class MESH;
};

class MEDSPLITTERTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( MEDSPLITTERTest );
  CPPUNIT_TEST( testMESHCollection_constructor );
  CPPUNIT_TEST( testMESHCollection_read_seq);
  CPPUNIT_TEST( testMESHCollection_read_para);
  CPPUNIT_TEST( testMESHCollection_square);
  CPPUNIT_TEST( testMESHCollection_square_with_faces);
  CPPUNIT_TEST( testMESHCollection_indivisible);
  CPPUNIT_TEST( testMESHCollection_user_partition);
  CPPUNIT_TEST( testParallelTopology_graph_constructor);
  CPPUNIT_TEST( testMESHCollection_complete_sequence);
  CPPUNIT_TEST( testMESHCollection_complete_sequence_with_polygon);
	CPPUNIT_TEST( testMESHCollection_families);
//  CPPUNIT_TEST( testMESHCollection_complete_sequence_with_polyhedra);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp();
  void tearDown();

  void testMESHCollection_constructor();
  void testMESHCollection_read_seq();
  void testMESHCollection_read_para();
  void testMESHCollection_square();
  void testMESHCollection_square_with_faces();
  void testMESHCollection_indivisible();
  void testMESHCollection_user_partition();
  void testMESHCollection_complete_sequence();
  void testMESHCollection_complete_sequence_with_polygon();
  void testMESHCollection_complete_sequence_with_polyhedra();
	void testMESHCollection_families();
  void testParallelTopology_graph_constructor();
    
  std::string getResourceFile( const std::string& );
  std::string getTmpDirectory();
  std::string makeTmpFile( const std::string&, const std::string& = "" );

  };

// to automatically remove temporary files from disk
class MEDSPLITTERTest_TmpFilesRemover
{
public:
  MEDSPLITTERTest_TmpFilesRemover() {}
  ~MEDSPLITTERTest_TmpFilesRemover();
  bool Register(const std::string theTmpFile);

private:
  std::set<std::string> myTmpFiles;
};

/*!
 *  Tool to print array to stream.
 */
template<class T>
void MEDSPLITTERTest_DumpArray (std::ostream & stream, const T* array, const int length, const std::string text)
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
