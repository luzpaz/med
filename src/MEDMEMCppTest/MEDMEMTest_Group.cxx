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

#include "MEDMEM_Group.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (10), defined in MEDMEM_Group.hxx:
 *  class GROUP : virtual public SUPPORT {
 *   (+) GROUP();
 *   (+) GROUP(const string & name, const list<FAMILY*> & family) throw (MEDEXCEPTION);
 *   (+) GROUP(const GROUP & m);
 *   (+) virtual ~GROUP();
 *   (+) GROUP & operator=(const GROUP &group);
 *   (+) friend ostream & operator<<(ostream &os, GROUP &my);
 *   (+) inline void setNumberOfFamilies(int numberOfFamilies);
 *   (+) inline void setFamilies(vector<FAMILY*> Family);
 *   (+) inline int                  getNumberOfFamilies() const;
 *   (+) inline vector<FAMILY*> getFamilies() const;
 *   (+) inline FAMILY *          getFamily(int i) const;
 *  }
 */
void MEDMEMTest::testGroup()
{
  string filename = getResourceFile("pointe.med") ;
  string meshname = "maa1";

  MESH * myMesh=new MESH;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh);
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open();
  myMeshDriver.read();
  myMeshDriver.close();

  const GROUP * myGroup = myMesh->getGroup(MED_NODE,1);
  CPPUNIT_ASSERT(myGroup != NULL);

  int NumberOfFamillies = myGroup->getNumberOfFamilies();
  CPPUNIT_ASSERT(NumberOfFamillies != 0);

  vector<FAMILY*> aFamilies = myGroup->getFamilies();
  CPPUNIT_ASSERT(NumberOfFamillies == (int)aFamilies.size());
  list<FAMILY*> aList;

  for (int j=1;j<=NumberOfFamillies;j++)
  {
    try{
      aList.push_back(myGroup->getFamily(j));
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
    CPPUNIT_ASSERT_EQUAL(myGroup->getFamily(j)->getName(), aFamilies[j-1]->getName());
  }

  GROUP* myGroup2 = new GROUP(*myGroup);

  ostringstream os;
  os << *myGroup2;
  CPPUNIT_ASSERT(os.str() != "");

  GROUP *myGroup3=new GROUP;
  try{
    *myGroup3 = *myGroup2;
  }
  catch (const std::exception &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch (...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  CPPUNIT_ASSERT_EQUAL(*myGroup3, *myGroup2);
  myGroup3->removeReference();

  GROUP *myGroup4=new GROUP;
  const GROUP * Group = myMesh->getGroup(MED_NODE,2);
  CPPUNIT_ASSERT(Group != NULL);

  int NumberOfFamillies1 = Group->getNumberOfFamilies();
  CPPUNIT_ASSERT(NumberOfFamillies1 != 0);
  if(NumberOfFamillies1)
  {
    myGroup4->setNumberOfFamilies(NumberOfFamillies1);
    myGroup4->setFamilies(Group->getFamilies());
    for(int i = 1; i <= myGroup4->getNumberOfFamilies(); i++ )
    {
      CPPUNIT_ASSERT_EQUAL(myGroup4->getFamily(i), Group->getFamily(i));
    }
  }
  myGroup4->removeReference();
  if(aList.size())
  {
    try{
      GROUP *myGroup5=new GROUP("newFamily", aList);
      myGroup5->removeReference();
    }
    catch (const std::exception &e)
    {
      CPPUNIT_FAIL(e.what());
    }
    catch (...)
    {
      CPPUNIT_FAIL("Unknown exception");
    }
  }
  myGroup2->removeReference();
  myMesh->removeReference() ;
}
