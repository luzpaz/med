// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

#ifndef _MEDSPLITTERTEST_HXX_
#define _MEDSPLITTERTEST_HXX_

#include "MEDSPLITTERTest_defs.hxx"

#include <cppunit/extensions/HelperMacros.h>

#include <set>
#include <string>
#include <iostream>

namespace MEDSPLITTER {
  class MESH;
};

class MEDSPLITTERTEST_EXPORT MEDSPLITTERTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( MEDSPLITTERTest );
  CPPUNIT_TEST( testMESHCollection_constructor );
  CPPUNIT_TEST( testMESHCollection_read_seq);
  CPPUNIT_TEST( testMESHCollection_read_para);
  CPPUNIT_TEST( testMESHCollection_square);
  CPPUNIT_TEST( testMESHCollection_square_with_faces);
#ifdef MED_ENABLE_SCOTCH
  CPPUNIT_TEST( testMESHCollection_indivisible);
#endif
  CPPUNIT_TEST( testMESHCollection_user_partition);
  CPPUNIT_TEST( testParallelTopology_graph_constructor);
#ifdef MED_ENABLE_SCOTCH
  CPPUNIT_TEST( testMESHCollection_complete_sequence);
  CPPUNIT_TEST( testMESHCollection_complete_sequence_with_polygon);
#endif
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
    
  };

#endif
