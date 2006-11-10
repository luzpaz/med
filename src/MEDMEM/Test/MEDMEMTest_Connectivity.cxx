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

//#include "MEDMEM_nArray.hxx"
//#include "MEDMEM_ArrayConvert.hxx"
//#include "MEDMEM_Array.hxx"
//#include "MEDMEM_ArrayInterface.hxx"
//#include "MEDMEM_AsciiFieldDriver.hxx"
//#include "MEDMEM_CellModel.hxx"
//#include "MEDMEM_Compatibility21_22.hxx"
#include "MEDMEM_Connectivity.hxx"
//#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_define.hxx"
//#include "MEDMEM_DriverFactory.hxx"
//#include "MEDMEM_DriversDef.hxx"
//#include "MEDMEM_DriverTools.hxx"
////#include "MEDMEM_Exception.hxx"
//#include "MEDMEM_Family.hxx"
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
//#include "MEDMEM_Support.hxx"
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

// #7: MEDMEM_Connectivity.hxx  }  MEDMEMTest_Connectivity.cxx

/*!
 *  Check methods (56), defined in MEDMEM_Connectivity.hxx:
 *  class CONNECTIVITY {
 *   (+)     friend ostream & operator<<(ostream &os, CONNECTIVITY &connectivity);
 *   (+)     CONNECTIVITY (MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (yetno) CONNECTIVITY (int numberOfTypes, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (+)     CONNECTIVITY (const CONNECTIVITY & m);
 *   (yetno) virtual ~CONNECTIVITY ();
 *   (yetno) void setConstituent (CONNECTIVITY * Constituent) throw (MEDEXCEPTION);
 *   (yetno) void setGeometricTypes (const MED_EN::medGeometryElement * Types,
 *                                   const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (yetno) void setCount (const int * Count, const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (yetno) void setNodal (const int * Connectivity,
 *                          const MED_EN::medEntityMesh Entity,
 *                          const MED_EN::medGeometryElement Type) throw (MEDEXCEPTION);
 *   (yetno) inline void setNumberOfNodes(int NumberOfNodes);
 *   (+)     inline int getEntityDimension() const;
 *   (yetno) inline void setEntityDimension(int EntityDimension);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     void setPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity,
 *                                        const int* PolygonsConnectivity,
 *                                        const int* PolygonsConnectivityIndex,
 *                                        int ConnectivitySize, int NumberOfPolygons);
 *   (+)     void setPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                          const int* PolyhedronConnectivity,
 *                                          const int* PolyhedronIndex, int ConnectivitySize,
 *                                          int NumberOfPolyhedron,
 *                                          const int* PolyhedronFacesIndex= (int*) NULL,
 *                                          int NumberOfFaces=0);
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) inline bool existConnectivity (MED_EN::medConnectivity connectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual bool existPolygonsConnectivity(MED_EN::medConnectivity connectivityType,
 *                                                  MED_EN::medEntityMesh Entity) const;
 *   (yetno) virtual bool existPolyhedronConnectivity(MED_EN::medConnectivity connectivityType,
 *                                                    MED_EN::medEntityMesh Entity) const;
 *
 *   (+)     virtual void calculateConnectivity (MED_EN::medConnectivity connectivityType,
 *                                               MED_EN::medEntityMesh Entity);
 *   (yetno) virtual void updateFamily (const vector<FAMILY*>& myFamilies);
 *
 *   (+)     inline MED_EN::medEntityMesh getEntity() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline int getNumberOfTypes (MED_EN::medEntityMesh Entity) const;
 *   (yetno) int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (yetno) const int * getConnectivityOfAnElementWithPoly
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity, int Number, int &lgth);
 *   (yetno) int getNumberOfPolyType() const;
 *   (yetno) int getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity,
 *                                           MED_EN::medGeometryElement Type) const;
 *   (yetno) int getNumberOfElementOfPolyType(MED_EN::medEntityMesh Entity) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) inline const MED_EN::medGeometryElement * getGeometricTypes
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) MED_EN::medGeometryElement * getGeometricTypesWithPoly
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) MED_EN::medGeometryElement getElementType (MED_EN::medEntityMesh Entity, int Number) const;
 *   (yetno) MED_EN::medGeometryElement getElementTypeWithPoly
 *             (MED_EN::medEntityMesh Entity, int Number) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) inline MED_EN::medGeometryElement getPolyTypeRelativeTo() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) virtual inline const int * getGlobalNumberingIndex
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) virtual const int * getConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                                                MED_EN::medEntityMesh Entity,
 *                                                MED_EN::medGeometryElement Type);
 *   (yetno) virtual int getConnectivityLength (MED_EN::medConnectivity ConnectivityType,
 *                                              MED_EN::medEntityMesh Entity,
 *                                              MED_EN::medGeometryElement Type);
 *   (yetno) virtual const int * getConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                                                     MED_EN::medEntityMesh Entity);
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) virtual const int* getPolygonsConnectivity(MED_EN::medConnectivity ConnectivityType,
 *                                                      MED_EN::medEntityMesh Entity);
 *   (yetno) virtual const int* getPolygonsConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
 *                                                           MED_EN::medEntityMesh Entity);
 *   (yetno) virtual int getNumberOfPolygons(MED_EN::medEntityMesh Entity=MED_EN::MED_ALL_ENTITIES) const;
 *   (yetno) virtual const int* getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual const int* getPolyhedronFacesIndex() const;
 *   (yetno) virtual const int* getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual int getNumberOfPolyhedronFaces() const;
 *   (yetno) virtual int getNumberOfPolyhedron() const;
 *   (yetno) int *getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const;
 *   (yetno) int **getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces,
 *                                             int* & nbOfNodesPerFaces) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) const CELLMODEL & getType (MED_EN::medGeometryElement Type) const;
 *   (yetno) const CELLMODEL * getCellsTypes (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) string * getCellTypeNames (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (yetno) int getNumberOfNodesInType (MED_EN::medGeometryElement Type) const;
 *   (yetno) int getNumberOfSubCellInType (MED_EN::medGeometryElement Type) const;
 *   (+)     virtual int getNumberOf (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   (yetno) virtual const int* getValue (MED_EN::medConnectivity TypeConnectivity,
 *                                        MED_EN::medGeometryElement Type);
 *   (yetno) virtual const int* getValueIndex (MED_EN::medConnectivity TypeConnectivity);
 *   (+)     virtual inline const int* getReverseConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (+)     virtual inline const int* getReverseConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (yetno) const int* getNeighbourhood() const;
 *   (yetno) void invertConnectivityForAFace(int faceId, const int *nodalConnForFace, bool polygonFace=false);
 *   (+)     bool deepCompare(const CONNECTIVITY& other) const;
 *
 *  Use code of MEDMEM/test_MEDMEM_PolyConnectivity.cxx
 *              MEDMEM/test_copie_connectivity.cxx
 *  }
 */
void affiche_connectivity(CONNECTIVITY * myConnectivity,
                          int MeshDimension, int NumberOfNodes, MED_EN::medEntityMesh Entity,
                          int NumberOfTypes)
{
  int entityDim = myConnectivity->getEntityDimension();
  CPPUNIT_ASSERT_EQUAL(MeshDimension, entityDim); //?

  //int nodesNb = myConnectivity->getNumberOfElements(MED_EN::MED_NODE, MED_EN::MED_NONE);
  int nodesNb = myConnectivity->getNumberOfElementsWithPoly(MED_EN::MED_NODE, MED_EN::MED_NONE);
  CPPUNIT_ASSERT_EQUAL(NumberOfNodes, nodesNb); //?

  // MED_EN::MED_CELL
  MED_EN::medEntityMesh entity = myConnectivity->getEntity();
  CPPUNIT_ASSERT_EQUAL(Entity, entity);

  int typesNb = myConnectivity->getNumberOfTypes(Entity);
  CPPUNIT_ASSERT_EQUAL(NumberOfTypes, typesNb);

  const MED_EN::medGeometryElement * Types = myConnectivity->getGeometricTypes(Entity);

  // Show Nodal Connectivity
  cout << "Show Nodal Connectivity :" << endl;
  for (int i = 0; i < NumberOfTypes; i++) {
    cout << "For type " << Types[i] << " : " << endl;
    int NumberOfElements = myConnectivity->getNumberOf(Entity, Types[i]);
    const int * connectivity =
      myConnectivity->getConnectivity(MED_EN::MED_NODAL, Entity, Types[i]);
    int NomberOfNodesPerCell = Types[i]%100;
    for (int j = 0; j < NumberOfElements; j++) {
      cout << "Element " << j+1 << " : ";
      for (int k = 0; k < NomberOfNodesPerCell; k++) {
        cout << connectivity[j*NomberOfNodesPerCell + k] << " ";
      }
      cout << endl;
    }
  }

  // Show Reverse Nodal Connectivity
  cout << "Show Reverse Nodal Connectivity :" << endl;
  const int * ReverseNodalConnectivity =
    myConnectivity->getReverseConnectivity(MED_EN::MED_NODAL, Entity);
  const int * ReverseNodalConnectivityIndex =
    myConnectivity->getReverseConnectivityIndex(MED_EN::MED_NODAL, Entity);
  for (int i = 0; i < NumberOfNodes; i++) {
    cout << "Node "<< i+1 << " : ";
    for (int j = ReverseNodalConnectivityIndex[i]; j < ReverseNodalConnectivityIndex[i+1]; j++)
      cout << ReverseNodalConnectivity[j-1] << " ";
    cout << endl;
  }

  // Show Descending Connectivity
  cout << "Show Connectivity (Descending) :" << endl;
  int NumberOfElements;
  const int * connectivity;
  const int * connectivity_index;
  myConnectivity->calculateConnectivity(MED_EN::MED_DESCENDING, Entity);
  try {
    NumberOfElements = myConnectivity->getNumberOf(Entity, MED_EN::MED_ALL_ELEMENTS);
    connectivity = myConnectivity->getConnectivity(MED_EN::MED_DESCENDING, Entity,
                                                   MED_EN::MED_ALL_ELEMENTS);
    connectivity_index = myConnectivity->getConnectivityIndex(MED_EN::MED_DESCENDING, Entity);
  }
  catch (MEDEXCEPTION m) {
    CPPUNIT_FAIL(m.what());
  }

  for (int j = 0; j < NumberOfElements; j++) {
    cout << "Element " << j+1 << " : ";
    for (int k = connectivity_index[j]; k < connectivity_index[j+1]; k++)
      cout << connectivity[k-1] << " ";
    cout << endl;
  }

  // Get constituent entity type and quantity
  int NumberOfConstituents  = 0;
  string constituent;
  MED_EN::medEntityMesh constituentEntity;

  if (MeshDimension == 3) {
    constituent = "Face";
    constituentEntity = MED_EN::MED_FACE;
  }

  if (MeshDimension == 2) {
    constituent = "Edge";
    constituentEntity = MED_EN::MED_EDGE;
  }

  if (MeshDimension == 1) {
    MESSAGE("ERROR : MeshDimension = 1 !");
    MESSAGE("We could not see Reverse Descending Connectivity.");
    return;
  }

  // Show Reverse Descending Connectivity
  cout << "Show Reverse Descending Connectivity :" << endl;
  const int * ReverseDescendingConnectivity =
    myConnectivity->getReverseConnectivity(MED_EN::MED_DESCENDING, Entity);
  const int * ReverseDescendingConnectivityIndex =
    myConnectivity->getReverseConnectivityIndex(MED_EN::MED_DESCENDING, Entity);

  NumberOfConstituents = myConnectivity->getNumberOf(constituentEntity, MED_EN::MED_ALL_ELEMENTS);
  for (int i = 0; i < NumberOfConstituents; i++) {
    cout << constituent << i+1 << " : ";
    for (int j = ReverseDescendingConnectivityIndex[i];
         j < ReverseDescendingConnectivityIndex[i+1]; j++) {
      cout << ReverseDescendingConnectivity[j-1] << " ";
    }
    cout << endl;
  }

  // Show <constituent> Nodal Connectivity
  cout << "Show " << constituent << " Connectivity (Nodal) :" << endl;
  const int * face_connectivity =
    myConnectivity->getConnectivity(MED_EN::MED_NODAL, constituentEntity, MED_EN::MED_ALL_ELEMENTS);
  const int * face_connectivity_index =
    myConnectivity->getConnectivityIndex(MED_EN::MED_NODAL, constituentEntity);
  for (int i = 0; i < NumberOfConstituents; i++) {
    cout << constituent << i+1 << " : ";
    for (int j = face_connectivity_index[i]; j < face_connectivity_index[i+1]; j++)
      cout << face_connectivity[j-1] << " ";
    cout << endl;
  }
}

void MEDMEMTest::testConnectivity()
{
  //////////////////////////////////////////////
  // TEST 1: test_MEDMEM_PolyConnectivity.cxx //
  //////////////////////////////////////////////
  CONNECTIVITY myNodalConnectivity;
  CONNECTIVITY myDescendingConnectivity;

  //POLYGONS
  const int NumberOfPolygons = 2;
  const int ConnectivitySize = 12;
  int PolygonsConnectivityIndex[NumberOfPolygons+1] = {1,7,13};

  //Nodal
  int PolygonsNodalConnectivity[ConnectivitySize] = {1,2,3,4,5,10,10,5,6,7,8,9};

  myNodalConnectivity.setPolygonsConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                              PolygonsNodalConnectivity,
                                              PolygonsConnectivityIndex, ConnectivitySize,
                                              NumberOfPolygons);

  //Descending
  int PolygonsDescendingConnectivity[ConnectivitySize] = {1,2,3,4,11,10,11,5,6,7,8,9};

  myDescendingConnectivity.setPolygonsConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                   PolygonsDescendingConnectivity,
                                                   PolygonsConnectivityIndex, ConnectivitySize,
                                                   NumberOfPolygons);

  //POLYHEDRON
  const int NumberOfPolyhedron = 2;
  int PolyhedronIndex[NumberOfPolyhedron+1] = {1,10,20};

  //Nodal
  const int NumberOfFaces = 19;
  const int NumberOfNodes = 74;
  int PolyhedronFacesIndex[NumberOfFaces+1] =
    {1,7,11,15,19,23,27,31,34,39,44,48,52,55,58,61,64,68,72,75};
  int PolyhedronNodalConnectivity[NumberOfNodes] =
    {1,2,3,4,5,6,1,7,8,2,2,8,9,3,4,3,9,10,5,4,
     10,11,6,5,11,12,1,6,12,7,7,12,8,10,9,8,12,11,13,14,
     15,3,2,13,2,8,16,14,13,16,17,15,14,17,15,17,18,15,18,9,
     3,15,9,2,3,9,8,8,9,17,16,9,18,17};

  myNodalConnectivity.setPolyhedronConnectivity(MED_EN::MED_NODAL, PolyhedronNodalConnectivity,
                                                PolyhedronIndex, NumberOfNodes, NumberOfPolyhedron,
                                                PolyhedronFacesIndex, NumberOfFaces);

  //Descending
  const int DescendingConnectivitySize = 19;
  int PolyhedronDescendingConnectivity[DescendingConnectivitySize] =
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,3,17,18};

  myDescendingConnectivity.setPolyhedronConnectivity(MED_EN::MED_DESCENDING,
                                                     PolyhedronDescendingConnectivity,
                                                     PolyhedronIndex, DescendingConnectivitySize,
                                                     NumberOfPolyhedron);

  cout << "Poly NodalConnectivity:" << endl;
  cout << myNodalConnectivity;
  cout << "Poly NodalConnectivity END" << endl;
  cout << "Poly DescendingConnectivity:" << endl;
  cout << myDescendingConnectivity;
  cout << "Poly DescendingConnectivity END" << endl;

  myNodalConnectivity.setNumberOfNodes(777);
  CPPUNIT_ASSERT_EQUAL(777, myNodalConnectivity.getNumberOfElementsWithPoly
                              (MED_EN::MED_NODE, MED_EN::MED_NONE));

  // Throws because _constituent is not calculated
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_FACE, MED_EN::MED_NONE), MEDEXCEPTION);

  // Throws because entity must differ from MED_NONE and MED_ALL_ELEMENTS
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_NONE), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

  // Throws because types are not defined
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_POLYGON), MEDEXCEPTION);

  // Throws because _nodal is not defined
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.calculateConnectivity
                       (MED_EN::MED_DESCENDING, MED_EN::MED_CELL), MEDEXCEPTION);

  /////////////////////////////////////////
  // TEST 2: test_copie_connectivity.cxx //
  /////////////////////////////////////////
  string datadir  = getenv("DATA_DIR");
  string filename = datadir + "/MedFiles/pointe.med";
  string meshname = "maa1";

  //Construction d'un maillage
  MESH * myMesh = new MESH();
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver (filename, myMesh);
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open();
  myMeshDriver.read(); //A partir d'ici la connectivité est construite
  myMeshDriver.close();

  int aMeshDimension = myMesh->getMeshDimension();
  int aNumberOfNodes = myMesh->getNumberOfNodes();

  const CONNECTIVITY * myConnectivity0 = myMesh->getConnectivityptr();
  CONNECTIVITY * myConnectivity1 = const_cast<CONNECTIVITY *>(myConnectivity0);

  // DATA:
  MED_EN::medEntityMesh anEntity0 = myConnectivity0->getEntity();

  int nbOfTypes = myConnectivity0->getNumberOfTypes(anEntity0);
  //const MED_EN::medGeometryElement * namesOfTypes = myConnectivity0->getGeometricTypes(anEntity0);
  //int * nbEltsPerType = new int [nbOfTypes];
  //int * nbNodsPerType = new int [nbOfTypes];
  //for (int i = 0; i < nbOfTypes; i++) {
  //  nbEltsPerType[i] = myConnectivity0->getNumberOf(anEntity0, namesOfTypes[i]);
  //  nbNodsPerType[i] = myConnectivity0->getNumberOfNodesInType(namesOfTypes[i]);
  //}

  // Show
  cout << "myConnectivity1 BEGIN" << endl;
  cout << *myConnectivity1 << endl;
  cout << "myConnectivity1 END" << endl;
  affiche_connectivity(myConnectivity1, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
  cout << "myConnectivity0 END 2" << endl;

  ostringstream ostr1;
  ostr1 << *myConnectivity1;
  CPPUNIT_ASSERT(ostr1.str() != "");

  // COPY
  CONNECTIVITY * myConnectivity2 = new CONNECTIVITY(* myConnectivity0);

  // Compare
  CPPUNIT_ASSERT(myConnectivity2->deepCompare(*myConnectivity0));

  // Compare after deleting the initial connectivity
  delete myMesh;
  myMesh = NULL;
  myConnectivity0 = NULL;

  MED_EN::medEntityMesh anEntity2 = myConnectivity2->getEntity();
  CPPUNIT_ASSERT_EQUAL(anEntity0, anEntity2);

  cout << "myConnectivity2 BEGIN" << endl;
  cout << *myConnectivity2 << endl;
  cout << "myConnectivity2 END" << endl;
  affiche_connectivity(myConnectivity2, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
  cout << "myConnectivity2 END 2" << endl;

  ostringstream ostr2;
  ostr2 << *myConnectivity2;
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());

  // ONE MORE COPY
  CONNECTIVITY * myConnectivity3 = new CONNECTIVITY(* myConnectivity2);
  delete myConnectivity2;

  MED_EN::medEntityMesh anEntity3 = myConnectivity3->getEntity();
  CPPUNIT_ASSERT_EQUAL(anEntity0, anEntity3);

  cout << "myConnectivity3 BEGIN" << endl;
  cout << *myConnectivity3 << endl;
  cout << "myConnectivity3 END" << endl;
  affiche_connectivity(myConnectivity3, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
  cout << "myConnectivity3 END 2" << endl;

  ostringstream ostr3;
  ostr3 << *myConnectivity3;
  CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr3.str());

  delete myConnectivity3;

  //delete [] nbEltsPerType;
  //delete [] nbNodsPerType;

  ////////////
  // TEST 3 //
  ////////////

  CPPUNIT_FAIL("Case Not Complete");
}
