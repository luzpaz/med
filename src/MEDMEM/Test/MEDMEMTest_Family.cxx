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
//

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Support.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #13: MEDMEM_Family.hxx  }  MEDMEMTest_Family.cxx

/*!
 *  Check methods (24), defined in MEDMEM_Family.hxx:
 *  class FAMILY : virtual public SUPPORT {
 *   (+)     FAMILY();
 *
 *           // Constructor to use with med driver.
 *   (!)     FAMILY(MESH* Mesh, int Identifier, string Name,
 *                  int NumberOfAttribute, int *AttributeIdentifier,
 *                  int *AttributeValue, string AttributeDescription,
 *                  int NumberOfGroup,   string GroupName,
 *                  int * MEDArrayNodeFamily, int ** MEDArrayCellFamily,
 *                  int ** MEDArrayFaceFamily, int ** MEDArrayEdgeFamily);
 *   (+)     FAMILY(const FAMILY & m);
 *   (+)     FAMILY(const SUPPORT & s);
 *   (+)     virtual ~FAMILY();
 *
 *           // Does not work (Segmentation Fault of Hang up)
 *   (!)     FAMILY & operator=(const FAMILY &fam);
 *
 *   (+)     friend ostream & operator<<(ostream &os, FAMILY &my);
 *   (+)     friend ostream & operator<<(ostream &os, const FAMILY &my);
 *
 *           // Is used only in constructor. Why it is public?
 *   (!)     bool build(MED_EN::medEntityMesh Entity,int **FamilyNumber);
 *
 *   (+)     inline void setIdentifier             (int Identifier);
 *   (+)     inline void setNumberOfAttributes     (int NumberOfAttribute);
 *   (+)     inline void setAttributesIdentifiers  (int * AttributeIdentifier);
 *   (+)     inline void setAttributesValues       (int * AttributeValue);
 *   (+)     inline void setAttributesDescriptions (string * AttributeDescription);
 *   (+)     inline void setNumberOfGroups         (int NumberOfGroups);
 *   (+)     inline void setGroupsNames            (string * GroupName);
 *
 *   (+)     inline int            getIdentifier()             const;
 *   (+)     inline int            getNumberOfAttributes()     const;
 *   (+)     inline const int *    getAttributesIdentifiers()  const;
 *   (+)     inline const int *    getAttributesValues()       const;
 *   (+)     inline const string * getAttributesDescriptions() const;
 *   (+)     inline int            getNumberOfGroups()         const;
 *   (+)     inline const string * getGroupsNames()            const;
 *
 *   (+)     inline int    getAttributeIdentifier(int i)  const;
 *   (+)     inline int    getAttributeValue(int i)       const;
 *   (+)     inline string getAttributeDescription(int i) const;
 *   (+)     inline string getGroupName(int i)            const;
 *  }
 *
 *  Use code of MEDMEM/test_copie_family.cxx
 */
void check_support(const SUPPORT * theSupport,
                   string theName, string theDescr, MED_EN::medEntityMesh theEntity, int theNbTypes)
{
  string aName  = theSupport->getName();
  string aDescr = theSupport->getDescription();
  MED_EN::medEntityMesh anEntity = theSupport->getEntity();
  int aNbGeomTypes = theSupport->getNumberOfTypes();

  CPPUNIT_ASSERT_EQUAL(theName, aName);
  CPPUNIT_ASSERT_EQUAL(theDescr, aDescr);
  CPPUNIT_ASSERT_EQUAL(theEntity, anEntity);
  CPPUNIT_ASSERT_EQUAL(theNbTypes, aNbGeomTypes);

  cout << "  - Entities list : " << endl;
  if (!(theSupport->isOnAllElements())) {
    cout << "  - NumberOfTypes : " << aNbGeomTypes << endl;
    const MED_EN::medGeometryElement * Types = theSupport->getTypes();
    for (int j = 0; j < aNbGeomTypes; j++) {
      cout << "    * Type " << Types[j] << " : ";
      int NumberOfElements = theSupport->getNumberOfElements(Types[j]);
      const int * Number = theSupport->getNumber(Types[j]);
      for (int k = 0; k < NumberOfElements; k++)
	cout << Number[k] << " ";
      cout << endl;
    }
  } else
    cout << "    Is on all entities !" << endl;
}

void check_famille(const FAMILY * theFamily,
                   string theName, string theDescr, MED_EN::medEntityMesh theEntity, int theNbTypes,
                   int theID, int theNbAttributes, int theNbGroups)
{
    check_support(theFamily, theName, theDescr, theEntity, theNbTypes);

    // identifier
    int id           = theFamily->getIdentifier();
    int nbAttributes = theFamily->getNumberOfAttributes();
    int nbGroups     = theFamily->getNumberOfGroups();

    CPPUNIT_ASSERT_EQUAL(theID, id);
    CPPUNIT_ASSERT_EQUAL(theNbAttributes, nbAttributes);
    CPPUNIT_ASSERT_EQUAL(theNbGroups, nbGroups);

    // attributes
    cout << "  - Attributes (" << nbAttributes << ") :" << endl;
    for (int j = 1; j < nbAttributes + 1; j++)
      cout << "    * "
           << theFamily->getAttributeIdentifier(j) << " : "
           << theFamily->getAttributeValue(j) << ", "
           << theFamily->getAttributeDescription(j).c_str() << endl;

    // groups
    cout << "  - Groups (" << nbGroups << ") :" << endl;
    for (int j = 1; j < nbGroups + 1; j++)
      cout << "    * " << theFamily->getGroupName(j).c_str() << endl;
}

void MEDMEMTest::testFamily()
{
  ///////////////////////////////////
  // TEST 1: test_copie_family.cxx //
  ///////////////////////////////////
  string datadir  = getenv("DATA_DIR");
  string filename = datadir + "/MedFiles/pointe.med";
  string meshname = "maa1";

  MESH * aMesh = new MESH();
  aMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER aMeshDriver (filename, aMesh);
  aMeshDriver.setMeshName(meshname);
  aMeshDriver.open();
  aMeshDriver.read();
  aMeshDriver.close();

  const FAMILY * aFamily1 = aMesh->getFamily(MED_EN::MED_NODE, 1);

  // SUPPORT attributes
  string aName     = aFamily1->getName();
  string aDescr    = aFamily1->getDescription();
  MED_EN::medEntityMesh anEntity = aFamily1->getEntity();
  int aNbGeomTypes = aFamily1->getNumberOfTypes();

  // FAMILY attributes
  int id           = aFamily1->getIdentifier();
  int nbAttributes = aFamily1->getNumberOfAttributes();
  int nbGroups     = aFamily1->getNumberOfGroups();

  // Dump
  ostringstream ostr1;
  ostr1 << *aFamily1;
  CPPUNIT_ASSERT(ostr1.str() != "");

  cout << "Show Family1 :" << endl;
  check_famille(aFamily1,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);

  FAMILY * aFamily2 = new FAMILY (* aFamily1);

  // Dump
  ostringstream ostr2;
  ostr2 << *aFamily2;
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());

  // Compare supports
  CPPUNIT_ASSERT(aFamily2->deepCompare(*aFamily1));

  cout << "Show Family2 :" << endl;
  check_famille(aFamily2,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);

  FAMILY * aFamily3 = new FAMILY (* aFamily2);
  delete aFamily2;

  // Dump
  ostringstream ostr3;
  ostr3 << *aFamily3;
  CPPUNIT_ASSERT(ostr1.str() == ostr3.str());

  cout << "Show Family3 :" << endl;
  check_famille(aFamily3,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);

  cout << "That's all" << endl;

  ////////////
  // TEST 2 //
  ////////////

  // Change aFamily3

  // attributes
  int   newNumberOfAttribute = 3;
  int * newAttributeIdentifier = new int[newNumberOfAttribute];
  int * newAttributeValue = new int[newNumberOfAttribute];
  string * newAttributeDescription = new string[newNumberOfAttribute];

  char tmp [32];
  for (int i = 0; i < newNumberOfAttribute; i++) {
    newAttributeIdentifier[i] = i+1;
    newAttributeValue[i] = (i+1)*10;
    sprintf(tmp, "Attribut N° %d", i+1);
    newAttributeDescription[i] = tmp;
  }

  aFamily3->setNumberOfAttributes(newNumberOfAttribute);
  aFamily3->setAttributesIdentifiers(newAttributeIdentifier);
  aFamily3->setAttributesValues(newAttributeValue);
  aFamily3->setAttributesDescriptions(newAttributeDescription);

  // groups
  int newNumberOfGroups = 4;
  string * newGroups = new string[newNumberOfGroups];
  for (int i = 0; i < newNumberOfGroups; i++) {
    sprintf(tmp, "Group N° %d", i+1);
    newGroups[i] = tmp;
  }

  aFamily3->setNumberOfGroups(newNumberOfGroups);
  aFamily3->setGroupsNames(newGroups, /*giveOwnership = */true);

  // identifier
  aFamily3->setIdentifier(77);

  // check new parameters of aFamily3

  // attributes
  CPPUNIT_ASSERT_EQUAL(newNumberOfAttribute, aFamily3->getNumberOfAttributes());
  const int *    newAttributeIdentifierBack  = aFamily3->getAttributesIdentifiers();
  const int *    newAttributeValueBack       = aFamily3->getAttributesValues();
  const string * newAttributeDescriptionBack = aFamily3->getAttributesDescriptions();
  for (int i = 1; i <= newNumberOfAttribute; i++) {
    sprintf(tmp, "Attribut N° %d", i);
    string descr (tmp);

    CPPUNIT_ASSERT_EQUAL(i    , aFamily3->getAttributeIdentifier(i));
    CPPUNIT_ASSERT_EQUAL(i*10 , aFamily3->getAttributeValue(i));
    CPPUNIT_ASSERT_EQUAL(descr, aFamily3->getAttributeDescription(i));

    CPPUNIT_ASSERT_EQUAL(i    , newAttributeIdentifierBack[i - 1]);
    CPPUNIT_ASSERT_EQUAL(i*10 , newAttributeValueBack[i - 1]);
    CPPUNIT_ASSERT_EQUAL(descr, newAttributeDescriptionBack[i - 1]);
  }

  // checked invalid indices
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Index is not checked to be in valid range
  CPPUNIT_ASSERT_THROW(aFamily3->getAttributeIdentifier(newNumberOfAttribute + 1), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aFamily3->getAttributeDescription(0), MEDEXCEPTION);
#endif

  // groups
  CPPUNIT_ASSERT_EQUAL(newNumberOfGroups, aFamily3->getNumberOfGroups());
  const string * newGroupsBack = aFamily3->getGroupsNames();
  for (int i = 1; i <= newNumberOfGroups; i++) {
    sprintf(tmp, "Group N° %d", i);
    string grp (tmp);

    CPPUNIT_ASSERT_EQUAL(grp, aFamily3->getGroupName(i));
    CPPUNIT_ASSERT_EQUAL(grp, newGroupsBack[i - 1]);
  }

  // identifier
  CPPUNIT_ASSERT_EQUAL(77, aFamily3->getIdentifier());

  // check default constructor and operator=
  FAMILY aFamily4;
#ifdef ENABLE_FAULTS
  aFamily4 = (const FAMILY &)*aFamily3;
#endif
#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Wrong implementation or usage of PointerOf<string>.
  //       Do not use memcpy() with array of std::string!
  CPPUNIT_FAIL("Impossible to use FAMILY::operator= because of"
               " wrong implementation or usage of PointerOf<string>");
  /*{
    int nb = 3;
    string * str = new string[nb];

    char tmp_str [32];
    for (int i = 0; i < nb; i++) {
      sprintf(tmp_str, "My String N° %d", i+1);
      str[i] = tmp;
    }

    // bad
    string* _pointer;
    _pointer = new string[3];
    // This is wrong, because class string is not simple type
    // and can have pointers to some data, deallocated in it's destructor,
    // which (data) will not be copied by this operation.
    memcpy(_pointer, str, 3*sizeof(string));
    delete [] _pointer;

    // good
    //PointerOf<int> p1 (1);
    //PointerOf<int> p2 (20);
    //p2 = newAttributeValue;
    //p1.set(3, p2);

    // bad
    //PointerOf<string> p1 (1);
    //PointerOf<string> p2 (20);
    //p2 = str;
    //p1.set(3, p2);

    delete [] str;
  }
  */
#endif

  ////////////
  // TEST 3 //
  ////////////

  // check construction from given support
  SUPPORT s1 (*aFamily3);
  FAMILY  f1 (s1);
  CPPUNIT_ASSERT_EQUAL(0, f1.getIdentifier());
  CPPUNIT_ASSERT_EQUAL(0, f1.getNumberOfAttributes());
  CPPUNIT_ASSERT_EQUAL(0, f1.getNumberOfGroups());
  CPPUNIT_ASSERT(s1.deepCompare(f1));

  delete aFamily3;

  delete [] newAttributeIdentifier;
  delete [] newAttributeValue;
  delete [] newAttributeDescription;

  delete aMesh;

  //CPPUNIT_FAIL("Constructor FAMILY::FAMILY(MESH*, int, string, etc.) and method build()"
  //             " is not tested because it is designed to be used only with med driver");
}
