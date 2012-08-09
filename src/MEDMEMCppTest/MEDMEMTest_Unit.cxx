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
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include <cstdlib>
#include <exception>

#include <MEDMEM_Unit.hxx>

using namespace std;
using namespace MEDMEM;


/*!
 *  Check methods (18), defined in MEDMEM_Unit.hxx:
 * class UNIT {
 *   (+)     UNIT();
 *   (+)     UNIT(string Name, string Description);
 *   (+)     ~UNIT();
 *   (+)     UNIT & operator=(const UNIT &unit);
 *   (+)     inline void setName(string Name);
 *   (+)     inline void setDescription(string Description);
 *   (+)     inline void setMasse(int Masse);
 *   (+)     inline void setLength(int Length);
 *   (+)     inline void setTime(int Time);
 *   (+)     inline void setTemperature(int Temperature);
 *   (+)     inline void setMatterQuantity(int MatterQuantity);
 *   (+)     void setCurrentStrength(int CurrentStrength);
 *   (+)     inline void setLightIntensity(int LightIntensity);
 *
 *   (+)     inline string getName() const;
 *   (+)     inline string getDescription() const;
 *   (+)     inline int    getMasse() const;
 *   (+)     inline int    getLength() const;
 *   (+)     inline int    getTime() const;
 *   (+)     inline int    getTemperature() const;
 *   (+)     inline int    getMatterQuantity() const;
 *   (+)     inline int    getCurrentStrength() const;
 *   (+)     inline int    getLightIntensity() const;
 * }
 *
 * Used code of MEDMEM/tests/testUUnit.cxx
 */

void MEDMEMTest::testUnit()
{
  string myName        = "NameForTesting";
  string myDescription = "all values are setted 10";
  int    myMasse           = 10;
  int    myLength          = 10;
  int    myTime            = 10;
  int    myTemperature     = 10;
  int    myMatterQuantity  = 10;
  int    myCurrentStrength = 10;
  int    myLightIntensity  = 10;

  //Default constructor
  UNIT   myUNIT;

  //Normal constructor
  UNIT *myUNIT1 = new UNIT(myName,myDescription);
  CPPUNIT_ASSERT(myUNIT1);
  delete myUNIT1;

  //--------------------------------------------------------------------//
  //                        Tests des methodes                          //
  //                                                                    //
  // - setName                                                          //
  // - setDescription                                                   //
  // - setMasse                                                         //
  // - setLength                                                        //
  // - setTime                                                          //
  // - setTemperature                                                   //
  // - setMatterQuantity                                                //
  // - setCurrentStrength                                               //
  // - setLightIntensity                                                //
  //--------------------------------------------------------------------//

  try
  {
    myUNIT.setName(myName);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setDescription(myDescription);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("MEDMEM_UNIT::setDescription(): Unknown exception");
  }

  try
  {
    myUNIT.setMasse(myMasse);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setLength(myLength);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setTime(myTime);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setTemperature(myTemperature);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setMatterQuantity(myMatterQuantity);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setCurrentStrength(myCurrentStrength);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    myUNIT.setLightIntensity(myLightIntensity);
  }
  catch(const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //--------------------------------------------------------------------//
  //                        Tests des methodes                          //
  //                                                                    //
  // - getName                                                          //
  // - getDescription                                                   //
  // - getMasse                                                         //
  // - getLength                                                        //
  // - getTime                                                          //
  // - getTemperature                                                   //
  // - getMatterQuantity                                                //
  // - getCurrentStrength                                               //
  // - getLightIntensity                                                //
  //--------------------------------------------------------------------//

  try
  {
    CPPUNIT_ASSERT( myUNIT.getName() == myName );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getDescription() == myDescription );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getMasse() == myMasse );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getLength() == myLength );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

    try
  {
    CPPUNIT_ASSERT( myUNIT.getTime() == myTime );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getTemperature() == myTemperature );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getMatterQuantity() == myMatterQuantity );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getCurrentStrength() == myCurrentStrength );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  try
  {
    CPPUNIT_ASSERT( myUNIT.getLightIntensity() == myLightIntensity );
  }
  catch( const std::exception &e )
  {
    CPPUNIT_FAIL(e.what());
  }
  catch ( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //--------------------------------------------------------------------//
  //                        Tests des method                            //
  // - operator=                                                        //
  //--------------------------------------------------------------------//

  UNIT myUNITCpy;
  myUNITCpy = myUNIT;
  CPPUNIT_ASSERT_EQUAL( myUNITCpy.getTemperature(), myUNITCpy.getTemperature());
}
