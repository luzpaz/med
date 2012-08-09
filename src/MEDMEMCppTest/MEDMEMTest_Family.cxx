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
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;

// #13: MEDMEM_Family.hxx  }  MEDMEMTest_Family.cxx

/*!
 *  Check methods (24), defined in MEDMEM_Family.hxx:
 *  class FAMILY : virtual public SUPPORT {
 *   (+)     FAMILY();
 *   (+)     FAMILY(MESH* Mesh, int Identifier, string Name,
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
 *   (+)     bool build(MED_EN::medEntityMesh Entity,int **FamilyNumber);
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
static void check_support(const SUPPORT * theSupport,
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
}

static void check_famille(const FAMILY * theFamily,
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
}

void MEDMEMTest::testFamily()
{
  ///////////////////////////////////
  // TEST 1: test_copie_family.cxx //
  ///////////////////////////////////
  string filename = getResourceFile("pointe.med");
  string meshname = "maa1";

  MESH * aMesh = new MESH;
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

  check_famille(aFamily1,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);

  // Copy constructor
  FAMILY * aFamily2 = new FAMILY (* aFamily1);

  // Dump
  ostringstream ostr2;
  ostr2 << *aFamily2;
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());

  // Compare supports
  CPPUNIT_ASSERT(aFamily2->deepCompare(*aFamily1));

  check_famille(aFamily2,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);

  // One more copy
  FAMILY * aFamily3 = new FAMILY (* aFamily2);
  aFamily2->removeReference();

  // Dump
  ostringstream ostr3;
  ostr3 << *aFamily3;
  CPPUNIT_ASSERT(ostr1.str() == ostr3.str());

  check_famille(aFamily3,
                aName, aDescr, anEntity, aNbGeomTypes,
                id, nbAttributes, nbGroups);


  ////////////
  // TEST 2 //
  ////////////
  {
    // Prepare data

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

    // groups
    int newNumberOfGroups = 4;
    string * newGroups = new string[newNumberOfGroups];
    for (int i = 0; i < newNumberOfGroups; i++) {
      sprintf(tmp, "Group N° %d", i+1);
      newGroups[i] = tmp;
    }

    // Change aFamily3

    // attributes
    aFamily3->setNumberOfAttributes(newNumberOfAttribute);
    aFamily3->setAttributesIdentifiers(newAttributeIdentifier);
    aFamily3->setAttributesValues(newAttributeValue);
    aFamily3->setAttributesDescriptions(newAttributeDescription);

    // groups
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

    // next string is commented since this pointer is shared
    // by Family3 and in future during deleting of Family3
    // we recieve error
    delete [] newAttributeDescription;

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
  }

  /////////////////////////////////////////////////////
  // TEST 3: check default constructor and operator= //
  /////////////////////////////////////////////////////
  {
    FAMILY *aFamily4=new FAMILY;
    *aFamily4 = (const FAMILY &)*aFamily3;
    aFamily4->removeReference();
  }

  ///////////////////////////////////////////////////
  // TEST 4: check construction from given support //
  ///////////////////////////////////////////////////
  {
    SUPPORT *s1=new SUPPORT(*aFamily3);
    FAMILY  *f1=new FAMILY(*s1);
    CPPUNIT_ASSERT_EQUAL(0, f1->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(0, f1->getNumberOfAttributes());
    CPPUNIT_ASSERT_EQUAL(0, f1->getNumberOfGroups());
    CPPUNIT_ASSERT(s1->deepCompare(*f1));
    s1->removeReference();
    f1->removeReference();
  }

  aFamily3->removeReference();
  aMesh->removeReference();

  /////////////////////////////////////////////////////////////////
  // TEST 5: check constructor, designed to use with med driver. //
  /////////////////////////////////////////////////////////////////
  {
    MESH * aTestMesh = MEDMEMTest_createTestMesh();

    // TestMesh families. Let's imagine that we would like to have
    // the following nodes and elements distribution by families:

    // NumberOfNodes = 19;
    int aNodeFamily[19] = {1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
                           2, 2, 2, 2, 2, 2, 3, 3, 3};

    // NumberOfCellTypes = 3;
    // NumberOfCells[NumberOfCellTypes] = {12, 2, 2};
    int aCellType1Family[12] = {4,4,4,4,4,5,5,5,5,5,5,5};
    int aCellType2Family[ 2] = {5,6};
    int aCellType3Family[ 2] = {6,6};
    int * aCellFamily[3] = {aCellType1Family, aCellType2Family, aCellType3Family};

    // NumberOfFaceTypes = 2;
    // NumberOfFaces[NumberOfFaceTypes] = {4, 4};
    int aFaceType1Family[4] = {7,7,7,7};
    int aFaceType2Family[4] = {7,7,7,7};
    int * aFaceFamily[3] = {aFaceType1Family, aFaceType2Family};

    // No edges in aTestMesh
    int ** anEdgeFamily = NULL;

    // new families data
    int attrId[2] = {1,2};
    int attrVa[2] = {7,8};

    string attrDescr1 ("Attribute 1 description");
    string attrDescr2 ("Attribute 2 description");
    string attrDescrEmpty (MED_COMMENT_SIZE - 23, ' ');
    attrDescr1 += attrDescrEmpty;
    attrDescr2 += attrDescrEmpty;
    CPPUNIT_ASSERT(MED_COMMENT_SIZE == attrDescr1.length());
    CPPUNIT_ASSERT(MED_COMMENT_SIZE == attrDescr2.length());
    string attrDescr = attrDescr1 + attrDescr2;

    string groupName1 ("Group_1");
    string groupName2 ("Group_2");
    string groupName3 ("Group_3");
    string groupNameEmpty (MED_LNAME_SIZE - 7, ' ');
    groupName1 += groupNameEmpty;
    groupName2 += groupNameEmpty;
    groupName3 += groupNameEmpty;
    CPPUNIT_ASSERT(MED_LNAME_SIZE == groupName1.length());
    CPPUNIT_ASSERT(MED_LNAME_SIZE == groupName2.length());
    CPPUNIT_ASSERT(MED_LNAME_SIZE == groupName3.length());
    string groupNames = groupName1 + groupName2 + groupName3;

    // nodes family 1
    FAMILY *aNodesF1=new FAMILY(aTestMesh, /*Identifier*/1, "Nodes 1", 
                                /*NumberOfAttribute*/2, attrId, attrVa, attrDescr,
                                /*NumberOfGroup*/3, groupNames,
                                aNodeFamily, aCellFamily, aFaceFamily, anEdgeFamily);


    CPPUNIT_ASSERT_EQUAL(1, aNodesF1->getIdentifier());
    CPPUNIT_ASSERT(strcmp("Nodes 1", aNodesF1->getName().c_str()) == 0);
    CPPUNIT_ASSERT(MED_EN::MED_NODE == aNodesF1->getEntity());
    CPPUNIT_ASSERT(!aNodesF1->isOnAllElements());
    CPPUNIT_ASSERT_EQUAL(7, aNodesF1->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT_EQUAL(2, aNodesF1->getNumberOfAttributes());
    CPPUNIT_ASSERT_EQUAL(1, aNodesF1->getAttributeIdentifier(1));
    CPPUNIT_ASSERT_EQUAL(2, aNodesF1->getAttributeIdentifier(2));
    CPPUNIT_ASSERT_EQUAL(7, aNodesF1->getAttributeValue(1));
    CPPUNIT_ASSERT_EQUAL(8, aNodesF1->getAttributeValue(2));
    CPPUNIT_ASSERT_EQUAL(attrDescr1, aNodesF1->getAttributeDescription(1));
    CPPUNIT_ASSERT_EQUAL(attrDescr2, aNodesF1->getAttributeDescription(2));

    CPPUNIT_ASSERT_EQUAL(3, aNodesF1->getNumberOfGroups());
    CPPUNIT_ASSERT_EQUAL(groupName1, aNodesF1->getGroupName(1));
    CPPUNIT_ASSERT_EQUAL(groupName2, aNodesF1->getGroupName(2));
    CPPUNIT_ASSERT_EQUAL(groupName3, aNodesF1->getGroupName(3));
    aNodesF1->removeReference();

    // faces family 7
    FAMILY *aFacesF7=new FAMILY(aTestMesh, /*Identifier*/7, "Faces All", 
                                /*NumberOfAttribute*/2, attrId, attrVa, attrDescr,
                                /*NumberOfGroup*/3, groupNames,
                                aNodeFamily, aCellFamily, aFaceFamily, anEdgeFamily);


    CPPUNIT_ASSERT_EQUAL(7, aFacesF7->getIdentifier());
    CPPUNIT_ASSERT(strcmp("Faces All", aFacesF7->getName().c_str()) == 0);
    CPPUNIT_ASSERT(MED_EN::MED_FACE == aFacesF7->getEntity());

    CPPUNIT_ASSERT_EQUAL(8, aTestMesh->getNumberOfElements(MED_EN::MED_FACE,
                                                           MED_EN::MED_ALL_ELEMENTS));
    CPPUNIT_ASSERT_EQUAL(8, aFacesF7->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT(!aFacesF7->isOnAllElements());

    CPPUNIT_ASSERT_EQUAL(2, aFacesF7->getNumberOfAttributes());
    CPPUNIT_ASSERT_EQUAL(1, aFacesF7->getAttributeIdentifier(1));
    CPPUNIT_ASSERT_EQUAL(2, aFacesF7->getAttributeIdentifier(2));
    CPPUNIT_ASSERT_EQUAL(7, aFacesF7->getAttributeValue(1));
    CPPUNIT_ASSERT_EQUAL(8, aFacesF7->getAttributeValue(2));
    CPPUNIT_ASSERT_EQUAL(attrDescr1, aFacesF7->getAttributeDescription(1));
    CPPUNIT_ASSERT_EQUAL(attrDescr2, aFacesF7->getAttributeDescription(2));

    CPPUNIT_ASSERT_EQUAL(3, aFacesF7->getNumberOfGroups());
    CPPUNIT_ASSERT_EQUAL(groupName1, aFacesF7->getGroupName(1));
    CPPUNIT_ASSERT_EQUAL(groupName2, aFacesF7->getGroupName(2));
    CPPUNIT_ASSERT_EQUAL(groupName3, aFacesF7->getGroupName(3));
    aFacesF7->removeReference();
    aTestMesh->removeReference();

    // Method build() is not tested directly, but it is called from constructor, tested here
  }
}
