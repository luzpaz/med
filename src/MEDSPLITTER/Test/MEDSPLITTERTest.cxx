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
#include "MEDSPLITTERTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_STRING.hxx"

#include <sstream>
#include <cmath>

using namespace std;

// ============================================================================
/*!
 *  Set up the environment
 */
// ============================================================================
void MEDSPLITTERTest::setUp()
{
}

// ============================================================================
/*!
 *  - delete
 */
// ============================================================================
void MEDSPLITTERTest::tearDown()
{
}

// #1: MEDSPLITTER_MESHCollection.hxx                }  MEDSPLITTERTest_MESHCollection.cxx
// #2: MEDSPLITTER_MESHCollectionDriver.hxx          }  MEDSPLITTERTest_MESHCollectionDriver.cxx
// #3: MEDSPLITTER_Topology.hxx (-)                  }  MEDSPLITTERTest_Topology.cxx
// #4: MEDSPLITTER_ParallelTopology.hxx              }  MEDSPLITTERTest_ParallelTopology.cxx
// #5: MEDSPLITTER_SequentialTopology.hxx            }  MEDSPLITTERTest_SequentialTopology.cxx 
// #6: MEDSPLITTER_API.hxx                           }  MEDSPLITTERTest_API.cxx
// #7: MEDSPLITTER_Graph.hxx                         }  MEDSPLITTERTest_Graph.cxx
// #8: MEDSPLITTER_METISGraph.hxx                    }  MEDSPLITTERTest_METISGraph.cxx
// #9: MEDSPLITTER_SCOTCHGraph.hxx                   }  MEDSPLITTERTest_SCOTCHGraph.cxx
// #10: MEDSPLITTER_UserGraph.hxx                    }  MEDSPLITTERTest_UserGraph.cxx (-)
