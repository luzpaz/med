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

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include <cstdlib>
#include <exception>

#include <MEDMEM_MedVersion.hxx>
#include "MEDMEM_Compatibility21_22.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (not in spec), defined in MEDMEM_MedVersion.hxx:
 *  (+) MED_EN::medFileVersion getMedFileVersion(const string & fileName) throw (MEDEXCEPTION);
 *  (+) int getMedAccessMode(MED_EN::med_mode_acces mode,
 *                               MED_EN::medFileVersion medVersion) throw (MEDEXCEPTION);
 */

void MEDMEMTest::testMedVersion()
{
  string filename = getenv("DATA_DIR");
  filename += "/MedFiles/pointe.med";
  string notExistFileName = "anyfile";
  medFileVersion myFileVersion;
  medFileVersion myEmptyFileVersion;

  // Test - getMedFileVersion
  CPPUNIT_ASSERT_THROW(getMedFileVersion(notExistFileName),MEDEXCEPTION);

  try
  {
    myFileVersion = getMedFileVersion(filename);
    CPPUNIT_ASSERT(myFileVersion == V21);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  //Test - getMedAccessMode
  CPPUNIT_ASSERT_THROW(getMedAccessMode(MED_REMP,myEmptyFileVersion),MEDEXCEPTION);

   try
  {
    CPPUNIT_ASSERT_EQUAL(med_2_1::MED_ECRI,
                         (med_2_1::med_mode_acces)getMedAccessMode(MED_REMP,myFileVersion));
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }
}
