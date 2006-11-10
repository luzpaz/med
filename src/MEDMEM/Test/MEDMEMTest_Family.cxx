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

////#include "MEDMEM_nArray.hxx"
////#include "MEDMEM_ArrayConvert.hxx"
////#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
//#include "MEDMEM_Connectivity.hxx"
////#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
//#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Family.hxx"
//#include "MEDMEM_FieldConvert.hxx"
//#include "MEDMEM_FieldForward.hxx"
//#include "MEDMEM_Field.hxx"
//#include "MEDMEM_Formulae.hxx"
//#include "MEDMEM_GaussLocalization.hxx"
//#include "MEDMEM_GenDriver.hxx"
//#include "MEDMEM_GibiMeshDriver.hxx"
//#include "MEDMEM_Grid.hxx"
//#include "MEDMEM_Group.hxx"
//#include "MEDMEM_IndexCheckingPolicy.hxx"
//#include "MEDMEM_InterlacingPolicy.hxx"
//#include "MEDMEM_InterlacingTraits.hxx"
//#include "MEDMEM_MedFieldDriver21.hxx"
//#include "MEDMEM_MedFieldDriver22.hxx"
//#include "MEDMEM_MedFieldDriver.hxx"
//#include "MEDMEM_Med.hxx"
//#include "MEDMEM_medimport_src.hxx"
//#include "MEDMEM_MedMedDriver21.hxx"
//#include "MEDMEM_MedMedDriver22.hxx"
//#include "MEDMEM_MedMedDriver.hxx"
//#include "MEDMEM_MEDMEMchampLire.hxx"
//#include "MEDMEM_MEDMEMgaussEcr.hxx"
//#include "MEDMEM_MEDMEMprofilEcr.hxx"
//#include "MEDMEM_MedMeshDriver21.hxx"
//#include "MEDMEM_MedMeshDriver22.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
//#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_Meshing.hxx"
//#include "MEDMEM_ModulusArray.hxx"
//#include "MEDMEM_PointerOf.hxx"
//#include "MEDMEM_PolyhedronArray.hxx"
//#include "MEDMEM_PorflowMeshDriver.hxx"
//#include "MEDMEM_RCBase.hxx"
//#include "MEDMEM_SetInterlacingType.hxx"
//#include "MEDMEM_SkyLineArray.hxx"
//#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Support.hxx"
//#include "MEDMEM_Tags.hxx"
//#include "MEDMEM_TopLevel.hxx"
//#include "MEDMEM_TypeMeshDriver.hxx"
//#include "MEDMEM_Unit.hxx"
//#include "MEDMEM_Utilities.hxx"
//#include "MEDMEM_VtkFieldDriver.hxx"
//#include "MEDMEM_VtkMedDriver.hxx"
//#include "MEDMEM_VtkMeshDriver.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;

// #13: MEDMEM_Family.hxx  }  MEDMEMTest.cxx

/*!
 *  Check methods (24), defined in MEDMEM_Family.hxx:
 *  class FAMILY : virtual public SUPPORT {
 *   (yetno) FAMILY();
 *   (yetno) FAMILY(MESH* Mesh, int Identifier, string Name,
 *                  int NumberOfAttribute, int *AttributeIdentifier,
 *                  int *AttributeValue, string AttributeDescription,
 *                  int NumberOfGroup,   string GroupName,
 *                  int * MEDArrayNodeFamily, int ** MEDArrayCellFamily,
 *                  int ** MEDArrayFaceFamily, int ** MEDArrayEdgeFamily);
 *   (yetno) FAMILY(const FAMILY & m);
 *   (yetno) FAMILY(const SUPPORT & s);
 *   (yetno) virtual ~FAMILY();
 *   (yetno) FAMILY & operator=(const FAMILY &fam);
 *   (yetno) friend ostream & operator<<(ostream &os, FAMILY &my);
 *   (yetno) friend ostream & operator<<(ostream &os, const FAMILY &my);
 *
 *   (yetno) bool build(MED_EN::medEntityMesh Entity,int **FamilyNumber);
 *
 *   (yetno) inline void setIdentifier             (int Identifier);
 *   (yetno) inline void setNumberOfAttributes     (int NumberOfAttribute);
 *   (yetno) inline void setAttributesIdentifiers  (int * AttributeIdentifier);
 *   (yetno) inline void setAttributesValues       (int * AttributeValue);
 *   (yetno) inline void setAttributesDescriptions (string * AttributeDescription);
 *   (yetno) inline void setNumberOfGroups         (int NumberOfGroups);
 *   (yetno) inline void setGroupsNames            (string * GroupName);
 *
 *   (yetno) inline int            getIdentifier()             const;
 *   (yetno) inline int            getNumberOfAttributes()     const;
 *   (yetno) inline const int *    getAttributesIdentifiers()  const;
 *   (yetno) inline const int *    getAttributesValues()       const;
 *   (yetno) inline const string * getAttributesDescriptions() const;
 *   (yetno) inline int            getNumberOfGroups()         const;
 *   (yetno) inline const string * getGroupsNames()            const;
 *
 *   (yetno) inline int    getAttributeIdentifier(int i)  const;
 *   (yetno) inline int    getAttributeValue(int i)       const;
 *   (yetno) inline string getAttributeDescription(int i) const;
 *   (yetno) inline string getGroupName(int i)            const;
 *  }
 *
 *  Use code of MEDMEM/test_copie_family.cxx
 */
void affiche_support(const SUPPORT * mySupport) 
{
  cout << "  - Name          : " << mySupport->getName().c_str() << endl;
  cout << "  - Description   : " << mySupport->getDescription().c_str() << endl;
  cout << "  - Entity        : " << mySupport->getEntity() << endl;
  cout << "  - Entities list : " << endl;
  if (!(mySupport->isOnAllElements())) {
    int NumberOfTypes = mySupport->getNumberOfTypes();
    cout << "  - NumberOfTypes : " << NumberOfTypes << endl;
    const MED_EN::medGeometryElement * Types = mySupport->getTypes();
    for (int j = 0; j < NumberOfTypes; j++) {
      cout << "    * Type " << Types[j] << " : ";
      int NumberOfElements = mySupport->getNumberOfElements(Types[j]);
      const int * Number = mySupport->getNumber(Types[j]);
      for (int k = 0; k < NumberOfElements; k++)
	cout << Number[k] << " ";
      cout << endl;
    }
  } else
    cout << "    Is on all entities !" << endl;
}

void affiche_famille(const FAMILY * myFamily)
{
    affiche_support(myFamily);
    cout << "  - Identifier : " << myFamily->getIdentifier() << endl;
    int NumberOfAttributes = myFamily->getNumberOfAttributes();
    cout << "  - Attributes (" << NumberOfAttributes << ") :" << endl;
    for (int j = 1; j < NumberOfAttributes+1; j++)
      cout << "    * " << myFamily->getAttributeIdentifier(j) << " : "
           << myFamily->getAttributeValue(j) << ", "
           << myFamily->getAttributeDescription(j).c_str() << endl;
    int NumberOfGroups = myFamily->getNumberOfGroups();
    cout << "  - Groups (" << NumberOfGroups << ") :" << endl;
    for (int j = 1; j < NumberOfGroups+1; j++)
      cout << "    * " << myFamily->getGroupName(j).c_str() << endl;
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

  const FAMILY * aFamily = aMesh->getFamily(MED_EN::MED_NODE, 1);
  //On renseigne les attributs spécifiques à FAMILY (p/r à SUPPORT) et non renseignés lors de la lecture du maillage
//    int  NumberOfAttribute = 3;
//    int *AttributeIdentifier = new int[NumberOfAttribute];
//    int *AttributeValue = new int[NumberOfAttribute];
//    string *AttributeDescription = new string[NumberOfAttribute];
//    char *tmp;
//    for (int i=0;i<NumberOfAttribute;i++)
//      {
//        AttributeIdentifier[i]=i+1;
//        AttributeValue[i]=(i+1)*10;
//        sprintf(tmp,"Attribut N° %d",i+1);
//        AttributeDescription[i]=tmp;
//      }

//    myFamily->setNumberOfAttributes(NumberOfAttribute);
//    myFamily->setAttributesIdentifiers (AttributeIdentifier);
//    myFamily->setAttributesValues (AttributeValue);
//    myFamily->setAttributesDescriptions (AttributeDescription);

  cout << "Show Family :" << endl;
  affiche_famille(aFamily);
  FAMILY * aFamily2 = new FAMILY (* aFamily);
  //delete aFamily;
  cout << "Show Family2 :" << endl;
  affiche_famille(aFamily2);
  FAMILY * aFamily3 = new FAMILY (* aFamily2);
  delete aFamily2;
  cout << "Show Family3 :" << endl;
  affiche_famille(aFamily3);
  delete aFamily3;

  cout << "That's all" << endl;

  /*
  cout << "Show Group :"<<endl;
  affiche_groupe(myMesh,MED_NODE);
  affiche_groupe(myMesh,MED_CELL);
  affiche_groupe(myMesh,MED_FACE);
  affiche_groupe(myMesh,MED_EDGE);
  */

  delete aMesh;

  CPPUNIT_FAIL("Case Not Complete");
}
