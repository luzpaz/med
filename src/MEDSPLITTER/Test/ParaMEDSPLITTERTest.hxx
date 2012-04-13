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
#ifndef _ParaMEDSPLITTERTest_HXX_
#define _ParaMEDSPLITTERTest_HXX_

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestRunner.h>
#include <stdexcept>

#include <mpi.h>

#include <iostream>
#include <fstream>

class ParaMEDSPLITTERTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ParaMEDSPLITTERTest );

  CPPUNIT_TEST( testMeshSendReceive );
  CPPUNIT_TEST( testMeshFuse );

  CPPUNIT_TEST( testParaDomainSelector_gatherGraph );
  CPPUNIT_TEST( testParaDomainSelector_gatherNbOf );
  CPPUNIT_TEST( testParaDomainSelector_gatherEntityTypesInfo );
  CPPUNIT_TEST( testParaDomainSelector_gatherNbCellPairs );
  CPPUNIT_TEST( testParaDomainSelector_exchangeJoint );
  CPPUNIT_TEST( testParaDomainSelector_exchangeSubentityIds );

  // CPPUNIT_TEST( functional_validation ); -> For the moment bug into ConnectZone

  CPPUNIT_TEST_SUITE_END();

public:

  void testMeshSendReceive();
  void testMeshFuse();

  void testParaDomainSelector_gatherGraph();
  void testParaDomainSelector_gatherNbOf ();
  void testParaDomainSelector_gatherEntityTypesInfo ();
  void testParaDomainSelector_gatherNbCellPairs ();
  void testParaDomainSelector_exchangeJoint ();
  void testParaDomainSelector_exchangeSubentityIds ();

  void functional_validation();
  void test_medsplitter_para( const std::string& med_file, const std::string& mesh );
};

#endif
