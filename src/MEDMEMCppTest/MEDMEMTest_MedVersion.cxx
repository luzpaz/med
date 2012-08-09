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

#include <cstdlib>
#include <exception>

#include <MEDMEM_MedVersion.hxx>

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
  string filename = getResourceFile("pointe_nomorereadable21.med");
  string notExistFileName = "anyfile";
  medFileVersion myFileVersion;

  // Test - getMedFileVersion
  CPPUNIT_ASSERT_THROW(getMedFileVersion(notExistFileName),MEDEXCEPTION);

  CPPUNIT_ASSERT_NO_THROW(myFileVersion = getMedFileVersion(filename));
  CPPUNIT_ASSERT(myFileVersion == V21);

}
