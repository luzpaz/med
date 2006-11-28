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

#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

#ifdef ENABLE_FAULTS
  // (BUG)
#endif
#ifdef ENABLE_FORCED_FAILURES
  //CPPUNIT_FAIL("");
#endif

using namespace std;
using namespace MEDMEM;

// #7: MEDMEM_Connectivity.hxx  }  MEDMEMTest_Connectivity.cxx

/*!
 *  Check methods (56), defined in MEDMEM_Connectivity.hxx:
 *  class CONNECTIVITY {
 *   (+)     friend ostream & operator<<(ostream &os, CONNECTIVITY &connectivity);
 *   (+)     CONNECTIVITY (MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (+)     CONNECTIVITY (int numberOfTypes, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
 *   (+)     CONNECTIVITY (const CONNECTIVITY & m);
 *   (+)     virtual ~CONNECTIVITY ();
 *   (+)     void setConstituent (CONNECTIVITY * Constituent) throw (MEDEXCEPTION);
 *   (+)     void setGeometricTypes (const MED_EN::medGeometryElement * Types,
 *                                   const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+)     void setCount (const int * Count, const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION);
 *   (+)     void setNodal (const int * Connectivity,
 *                          const MED_EN::medEntityMesh Entity,
 *                          const MED_EN::medGeometryElement Type) throw (MEDEXCEPTION);
 *   (+)     inline void setNumberOfNodes(int NumberOfNodes);
 *   (+)     inline int getEntityDimension() const;
 *   (+)     inline void setEntityDimension(int EntityDimension);
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
 *   (+)     inline bool existConnectivity (MED_EN::medConnectivity connectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (+)     virtual bool existPolygonsConnectivity(MED_EN::medConnectivity connectivityType,
 *                                                  MED_EN::medEntityMesh Entity) const;
 *   (+)     virtual bool existPolyhedronConnectivity(MED_EN::medConnectivity connectivityType,
 *                                                    MED_EN::medEntityMesh Entity) const;
 *
 *   (+)     virtual void calculateConnectivity (MED_EN::medConnectivity connectivityType,
 *                                               MED_EN::medEntityMesh Entity);
 *   (yetno) virtual void updateFamily (const vector<FAMILY*>& myFamilies);
 *
 *   (+)     inline MED_EN::medEntityMesh getEntity() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline int getNumberOfTypes (MED_EN::medEntityMesh Entity) const;
 *   (+)     int getNumberOfTypesWithPoly(MED_EN::medEntityMesh Entity) const;
 *   (+)     const int * getConnectivityOfAnElementWithPoly
 *           (MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity, int Number, int &lgth);
 *   (+)     int getNumberOfPolyType() const;
 *   (+)     int getNumberOfElementsWithPoly(MED_EN::medEntityMesh Entity,
 *                                           MED_EN::medGeometryElement Type) const;
 *   (+)     int getNumberOfElementOfPolyType(MED_EN::medEntityMesh Entity) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline const MED_EN::medGeometryElement * getGeometricTypes
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (+)     MED_EN::medGeometryElement * getGeometricTypesWithPoly
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     MED_EN::medGeometryElement getElementType (MED_EN::medEntityMesh Entity, int Number) const;
 *   (+)     MED_EN::medGeometryElement getElementTypeWithPoly
 *                        (MED_EN::medEntityMesh Entity, int Number) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline MED_EN::medGeometryElement getPolyTypeRelativeTo() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) virtual inline const int * getGlobalNumberingIndex
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   -----------------------------------------------------------------------------------------------------
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
 *   -----------------------------------------------------------------------------------------------------
 *   (yetno) virtual const int* getPolyhedronConnectivity(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual const int* getPolyhedronFacesIndex() const;
 *   (yetno) virtual const int* getPolyhedronIndex(MED_EN::medConnectivity ConnectivityType) const;
 *   (yetno) virtual int getNumberOfPolyhedronFaces() const;
 *   (yetno) virtual int getNumberOfPolyhedron() const;
 *   (yetno) int *getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const;
 *   (yetno) int **getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces,
 *                                             int* & nbOfNodesPerFaces) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     const CELLMODEL & getType (MED_EN::medGeometryElement Type) const;
 *   (+)     const CELLMODEL * getCellsTypes (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (+)     string * getCellTypeNames (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (+)     int getNumberOfNodesInType (MED_EN::medGeometryElement Type) const;
 *   (+)     int getNumberOfSubCellInType (MED_EN::medGeometryElement Type) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     virtual int getNumberOf (MED_EN::medEntityMesh Entity, MED_EN::medGeometryElement Type) const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     virtual const int* getValue (MED_EN::medConnectivity TypeConnectivity,
 *                                        MED_EN::medGeometryElement Type);
 *   (+)     virtual const int* getValueIndex (MED_EN::medConnectivity TypeConnectivity);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     virtual inline const int* getReverseConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (+)     virtual inline const int* getReverseConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                                        MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) throw (MEDEXCEPTION);
 *   (NOT YET IMPLEMENTED!!!) const int* getNeighbourhood() const;
 *   (yetno) void invertConnectivityForAFace(int faceId, const int *nodalConnForFace, bool polygonFace=false);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     bool deepCompare(const CONNECTIVITY& other) const;
 *
 *  Use code of MEDMEM/test_MEDMEM_PolyConnectivity.cxx
 *              MEDMEM/test_copie_connectivity.cxx
 *  }
 */
void showConnectivity(CONNECTIVITY * myConnectivity,
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

/////////////////////////////////////////
// TEST 2: test_copie_connectivity.cxx //
/////////////////////////////////////////
void checkCopyConnectivity()
{
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

  // Show
  cout << "myConnectivity1 BEGIN" << endl;
  cout << *myConnectivity1 << endl;
  cout << "myConnectivity1 END" << endl;
  showConnectivity(myConnectivity1, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
  cout << "myConnectivity1 END 2" << endl;

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
  showConnectivity(myConnectivity2, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
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
  showConnectivity(myConnectivity3, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);
  cout << "myConnectivity3 END 2" << endl;

  ostringstream ostr3;
  ostr3 << *myConnectivity3;
  CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr3.str());

  delete myConnectivity3;
}

void createOrCheck (CONNECTIVITY * theC, string msg, bool create = false)
{
  // Preconditions: Entity and NumberOfTypes
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_CELL, theC->getEntity());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_CELL));

  // EntityDimension
  if (create)
    // It would be good to set EntityDimension automatically for EDGEs and FACEs,
    // and warn about not set EntityDimension for CELLs
    // (or calculate it from given geometric types)
    theC->setEntityDimension(3);
  else
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getEntityDimension());

  // NumberOfNodes
  int nbNodes = 20;

  if (create) {
    theC->setNumberOfNodes(nbNodes);
  }
  else {
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbNodes, theC->getNumberOfElementsWithPoly
                                 (MED_EN::MED_NODE, MED_EN::MED_NONE));
  }

  // GeometricTypes
  MED_EN::medGeometryElement aCellTypes[2] = {MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};

  if (create) {
    theC->setGeometricTypes(aCellTypes, MED_EN::MED_CELL);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_FACE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_EDGE), MEDEXCEPTION);
  }
  else {
    // CELLS: theC
    const MED_EN::medGeometryElement * aCellTypesBack = theC->getGeometricTypes(MED_EN::MED_CELL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCellTypes[0], aCellTypesBack[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCellTypes[1], aCellTypesBack[1]);

    const CELLMODEL * aCellModels = theC->getCellsTypes(MED_EN::MED_CELL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCellTypes[0], aCellModels[0].getType());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCellTypes[1], aCellModels[1].getType());

    string * aCellTypesNames = theC->getCellTypeNames(MED_EN::MED_CELL);
    CPPUNIT_ASSERT_MESSAGE(msg, aCellTypesNames[0] == "MED_PYRA5");
    CPPUNIT_ASSERT_MESSAGE(msg, aCellTypesNames[1] == "MED_HEXA8");

    // FACES: theC->_constituent
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_FACE));
    const MED_EN::medGeometryElement * aFaceTypesBack = theC->getGeometricTypes(MED_EN::MED_FACE);
    CPPUNIT_ASSERT_MESSAGE(msg,
            (aFaceTypesBack[0] == MED_EN::MED_TRIA3 && aFaceTypesBack[1] == MED_EN::MED_QUAD4) ||
            (aFaceTypesBack[1] == MED_EN::MED_TRIA3 && aFaceTypesBack[0] == MED_EN::MED_QUAD4));

    const CELLMODEL * aFaceModels = theC->getCellsTypes(MED_EN::MED_FACE);
    bool case1 = (aFaceModels[0].getType() == MED_EN::MED_TRIA3 &&
                  aFaceModels[1].getType() == MED_EN::MED_QUAD4);
    bool case2 = (aFaceModels[1].getType() == MED_EN::MED_TRIA3 &&
                  aFaceModels[0].getType() == MED_EN::MED_QUAD4);
    CPPUNIT_ASSERT_MESSAGE(msg, case1 || case2);

    string * aFaceTypesNames = theC->getCellTypeNames(MED_EN::MED_FACE);
    CPPUNIT_ASSERT_MESSAGE(msg,
                           (aFaceTypesNames[0] == "MED_TRIA3" && aFaceTypesNames[1] == "MED_QUAD4") ||
                           (aFaceTypesNames[1] == "MED_TRIA3" && aFaceTypesNames[0] == "MED_QUAD4"));

    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfTypes(MED_EN::MED_EDGE));
    //const MED_EN::medGeometryElement * anEdgeTypesBack = theC->getGeometricTypes(MED_EN::MED_EDGE);
    //const CELLMODEL * anEdgeModels = theC->getCellsTypes(MED_EN::MED_EDGE);

    // invalid cases
    CPPUNIT_ASSERT_THROW(theC->getGeometricTypes(MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getGeometricTypes(MED_EN::MED_ALL_ENTITIES), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getCellsTypes(MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getCellTypeNames(MED_EN::MED_ALL_ENTITIES), MEDEXCEPTION);
  }

  // Nodal Connectivity for standard types
  int countCells[3] = {1, 3, 4}; // 2 PYRA5 and 1 HEXA8
  int nodesCells_PYRA5[10] = {2,3,4,5,1, 6,7,8,9,10};
  int nodesCells_HEXA8[8] = {2,3,4,5, 6,7,8,9};

  //          .1
  //
  //      3.---------.4
  //      /|        /|
  //     / |       / |
  //    /  |      /  |
  //  2.---------.5  |
  //   |   |     |   |
  //   |  7.-----|---.8
  //   |  /      |  /
  //   | /       | /
  //   |/        |/
  //  6.---------.9
  //
  //          .10

  // cells index will be: {1, 6, 11, 19}

  if (create) {
    theC->setCount(countCells, MED_EN::MED_CELL);
    theC->setNodal(nodesCells_PYRA5, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
    theC->setNodal(nodesCells_HEXA8, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

    // invalid cases
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_EDGE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_FACE), MEDEXCEPTION);

    CPPUNIT_ASSERT_THROW(theC->setNodal(nodesCells_PYRA5, MED_EN::MED_FACE, MED_EN::MED_PYRA5), MEDEXCEPTION);
  }
  else {
    // CELLS(3D): theC
    CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
    CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_PYRA5));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_HEXA8));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                           MED_EN::MED_PYRA5));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                           MED_EN::MED_HEXA8));

    // FACES: theC->_constituent
    CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
    //CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  8, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_TRIA3));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_QUAD4));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 14, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS));

    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));

    // invalid cases
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_TETRA4));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                           MED_EN::MED_TETRA4));
  }

  // 2 POLYHEDRA
  const int nbPolyhedron = 2;
  const int nbPolyFaces = 14;
  const int nbPolyNodes = 52;

  int aPolyhedronIndex[nbPolyhedron + 1] = {1,8,15};

  int aPolyhedronFacesIndex[nbPolyFaces + 1] = {1,7,10,14,17,20,24, 27,33,36,40,43,46,50,53};

  int aPolyhedronNodalConnectivity[nbPolyNodes] = {
    11,15,19,20,17,13, 11,13,14, 14,13,17,18, 18,17,20, 11,14,15, 15,14,18,19, 19,18,20,
    11,13,17,20,19,15, 11,12,13, 13,12,16,17, 17,16,20, 11,15,12, 12,15,19,16, 16,19,20};

  //          .11
  //
  //     13.---------.14
  //      /|\       /|
  //     / |  \    / |
  //    /  |    \ /  |
  // 12.---------.15 |
  //   |   |     |   |
  //   | 17.-----|---.18
  //   |  / \    |  /
  //   | /    \  | /
  //   |/       \|/
  // 16.---------.19
  //
  //          .20

  if (create) {
    theC->setPolyhedronConnectivity(MED_EN::MED_NODAL, aPolyhedronNodalConnectivity,
                                    aPolyhedronIndex, nbPolyNodes, nbPolyhedron,
                                    aPolyhedronFacesIndex, nbPolyFaces);
  }
  else {
    int len, i;

    // CELLS(3D): theC
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfPolyType());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypesWithPoly(MED_EN::MED_CELL));

    const MED_EN::medGeometryElement * aCallTypesBack = theC->getGeometricTypesWithPoly(MED_EN::MED_CELL);
    CPPUNIT_ASSERT_MESSAGE(msg,
            (aCallTypesBack[0] == MED_EN::MED_PYRA5 && aCallTypesBack[1] == MED_EN::MED_HEXA8) ||
            (aCallTypesBack[0] == MED_EN::MED_HEXA8 && aCallTypesBack[1] == MED_EN::MED_PYRA5));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYHEDRA, aCallTypesBack[2]);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfElementOfPolyType(MED_EN::MED_CELL));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                           MED_EN::MED_POLYHEDRA));
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Actually there is no CELLs of type POLYGON (only FACEs)!!!
    //       But this call returns 2 instead of 0
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                           MED_EN::MED_POLYGON));
#endif

    int nbCellAll = 5; // 2 (PYRA5) + 1 (HEXA8) + 2 (POLYGON)
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) No (or bad) implementation for MED_ALL_ELEMENTS, really returns 3 (without poly)
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbCellAll, theC->getNumberOfElementsWithPoly(MED_EN::MED_CELL,
                                                                                   MED_EN::MED_ALL_ELEMENTS));
#endif

    // first PYRA5 {1,2,3,4,5}
    const int * c1 = theC->getConnectivityOfAnElementWithPoly
                           (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, len);
    for (i = 0; i < len; i++) {
      if (c1[i] < 1 || 5 < c1[i]) CPPUNIT_FAIL(msg);
    }

    // first POLYHEDRA
    // This throws "NODAL Connectivity required for a polyhedron"
    // Why? Because this has no sence without index?
    //const int * nc_e2 = aCells2.getConnectivityOfAnElementWithPoly
    //  (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/4, len);

    // cells descending connectivity
    for (i = 1; i <= nbCellAll; i++) {
      const int * ci = theC->getConnectivityOfAnElementWithPoly
        (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/i, len);

      MED_EN::medGeometryElement aCurElemTypeWithPoly = theC->getElementTypeWithPoly(MED_EN::MED_CELL, i);

      if (i <= 3) { // nb.standard cells = 3
        MED_EN::medGeometryElement aCurElemType = theC->getElementType(MED_EN::MED_CELL, i);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCurElemType, aCurElemTypeWithPoly);
        // sign of connectivity array value means element direction
        for (int j = 0; j < len; j++) {
          CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(ci[j]) && labs(ci[j]) <= 14); // nb.standard faces = 14
        }
      }
      else {
        CPPUNIT_ASSERT_THROW(theC->getElementType(MED_EN::MED_CELL, i), MEDEXCEPTION);
        for (int j = 0; j < len; j++) {
          CPPUNIT_ASSERT_MESSAGE(msg, 14 < labs(ci[j]) && labs(ci[j]) <= 27); // nb.polygons = 13
        }
      }

      switch (aCurElemTypeWithPoly) {
      case MED_EN::MED_PYRA5:     CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, len); break;
      case MED_EN::MED_HEXA8:     CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, len); break;
      case MED_EN::MED_POLYHEDRA: CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 7, len); break;
      default:
        CPPUNIT_FAIL(msg); // wrong element type
      }
    }

    // FACES: theC->_constituent
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypesWithPoly(MED_EN::MED_FACE));

    const MED_EN::medGeometryElement * aFaceTypesBack = theC->getGeometricTypesWithPoly(MED_EN::MED_FACE);
    CPPUNIT_ASSERT_MESSAGE(msg,
            (aFaceTypesBack[0] == MED_EN::MED_TRIA3 && aFaceTypesBack[1] == MED_EN::MED_QUAD4) ||
            (aFaceTypesBack[0] == MED_EN::MED_QUAD4 && aFaceTypesBack[1] == MED_EN::MED_TRIA3));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYGON, aFaceTypesBack[2]);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 13, theC->getNumberOfElementOfPolyType(MED_EN::MED_FACE));
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Actually there is no FACEs of type POLYHEDRA (only CELLs)!!!
    //       But this call returns 13 instead of 0
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  0, theC->getNumberOfElementsWithPoly(MED_EN::MED_FACE,
                                                                            MED_EN::MED_POLYHEDRA));
#endif
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 13, theC->getNumberOfElementsWithPoly(MED_EN::MED_FACE,
                                                                            MED_EN::MED_POLYGON));

    int nbFaAll = 27; // 6 (QUAD4) + 8 (TRIA3) + 13 (POLYGON)
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) No (or bad) implementation for MED_ALL_ELEMENTS, really returns 14 (without poly)
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbFaAll, theC->getNumberOfElementsWithPoly(MED_EN::MED_FACE,
                                                                                 MED_EN::MED_ALL_ELEMENTS));
#endif

    bool isHexagon = false;
    for (i = 1; i <= nbFaAll; i++) {
      const int * ci = theC->getConnectivityOfAnElementWithPoly(MED_EN::MED_NODAL,
                                                                MED_EN::MED_FACE, /*Number*/i, len);
      MED_EN::medGeometryElement aCurElemTypeWithPoly = theC->getElementTypeWithPoly(MED_EN::MED_FACE, i);

      if (len == 6) {
        CPPUNIT_ASSERT_MESSAGE(msg, !isHexagon); // because only one hexagon must exist

        // check nodes {11,15,19,20,17,13}
        int nij;
        for (int j = 0; j < len; j++) {
          nij = ci[j];
          CPPUNIT_ASSERT_MESSAGE(msg, nij==11 || nij==15 || nij==19 || nij==20 || nij==17 || nij==13);
        }

        isHexagon = true;
      }

      if (i > 14) { // nb.standard faces = 14
        CPPUNIT_ASSERT_THROW(theC->getElementType(MED_EN::MED_FACE, i), MEDEXCEPTION);
      }
      else {
        MED_EN::medGeometryElement aCurElemType = theC->getElementType(MED_EN::MED_FACE, i);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aCurElemType, aCurElemTypeWithPoly);
      }

      switch (aCurElemTypeWithPoly) {
      case MED_EN::MED_TRIA3:   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, len); break;
      case MED_EN::MED_QUAD4:   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 4, len); break;
      case MED_EN::MED_POLYGON: CPPUNIT_ASSERT_MESSAGE(msg, len == 3 || len == 4 || len == 6); break;
      default:
        CPPUNIT_FAIL(msg); // wrong element type
      }
    }
    CPPUNIT_ASSERT_MESSAGE(msg, isHexagon); // hexagon must exist

    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypesWithPoly(MED_EN::MED_EDGE));
  }

  if (create) {
    // force _constituent computation
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_FACE));

#ifdef ENABLE_FORCED_FAILURES
    // (BUG) Why it returns 0?
    // And if we force calculating of descending connectivity for faces:
    // theC->getNumberOfTypes(MED_EN::MED_EDGE);
    // then dumping of theC to stream throws a MEDEXCEPTION
    // "CONNECTIVITY::calculateNodalConnectivity : No connectivity found !"
    // while dumping edges connectivity
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfTypes(MED_EN::MED_EDGE));
#endif

    cout << "^^^^^" << endl;
    cout <<  *theC  << endl;
    cout << "^^^^^" << endl;
  }
  else {
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYHEDRA, theC->getPolyTypeRelativeTo());

    // The following methods are not recursive, i.e. they return types
    // of this connectivity, but do not return types of _constituent.
    // And these methods do not work with poly-types.

    // getType
    const CELLMODEL & aPYRA5_type = theC->getType(MED_EN::MED_PYRA5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_PYRA5, aPYRA5_type.getType());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, aPYRA5_type.getNumberOfVertexes());

    const CELLMODEL & aHEXA8_type = theC->getType(MED_EN::MED_HEXA8);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 8, aHEXA8_type.getNumberOfNodes());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, aHEXA8_type.getDimension());
    // nb. of sub-faces (nb. of constituents with dimension = 3 - 1)
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, aHEXA8_type.getNumberOfConstituents(1));

    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_TRIA3), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_POLYGON), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_POLYHEDRA), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_NONE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

    // getNumberOfNodesInType
    int nbNodesInPYRA5 = theC->getNumberOfNodesInType(MED_EN::MED_PYRA5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, nbNodesInPYRA5);

    CPPUNIT_ASSERT_THROW(theC->getNumberOfNodesInType(MED_EN::MED_TRIA3), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getNumberOfNodesInType(MED_EN::MED_POLYGON), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getNumberOfNodesInType(MED_EN::MED_NONE), MEDEXCEPTION);

    // getNumberOfSubCellInType
    int nbFacesInHEXA8 = theC->getNumberOfSubCellInType(MED_EN::MED_HEXA8);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, nbFacesInHEXA8);

    CPPUNIT_ASSERT_THROW(theC->getNumberOfSubCellInType(MED_EN::MED_QUAD4), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getNumberOfSubCellInType(MED_EN::MED_POLYHEDRA), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getNumberOfSubCellInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

    // getValueIndex
    const int* nodalIndex = theC->getValueIndex(MED_EN::MED_NODAL);
    const int* desceIndex = theC->getValueIndex(MED_EN::MED_DESCENDING);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, nodalIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, nodalIndex[1]);  // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 11, nodalIndex[2]); // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 19, nodalIndex[3]); // +8 nodes of HEXA8

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, desceIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, desceIndex[1]);  // +5 faces of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 11, desceIndex[2]); // +5 faces of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 17, desceIndex[3]); // +6 faces of HEXA8

    // getValue
    const int* nodalValue = theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_ALL_ELEMENTS);
    const int* nodalPYRA5 = theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_PYRA5);
    const int* nodalHEXA8 = theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_HEXA8);

    for (int i = 0; i < 10; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_PYRA5[i], nodalPYRA5[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_PYRA5[i], nodalValue[i]);
    }

    for (int i = 0; i < 8; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_HEXA8[i], nodalHEXA8[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_HEXA8[i], nodalValue[10 + i]);
    }

    const int* desceValue = theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_ALL_ELEMENTS);
#ifdef ENABLE_FORCED_FAILURES
    // (BUG) CONNECTIVITY::getValue(MED_EN::MED_DESCENDING, type) wrong implementation:
    // in line 1027: _descending->getI(Type)
    const int* descePYRA5 = theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_PYRA5);
    const int* desceHEXA8 = theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_HEXA8);
#endif

    for (int i = 0; i < 10; i++) {
#ifdef ENABLE_FORCED_FAILURES
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(descePYRA5[i]) && labs(descePYRA5[i]) < 16);
#endif
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceValue[i]) && labs(desceValue[i]) < 16);
    }

    for (int i = 0; i < 6; i++) {
#ifdef ENABLE_FORCED_FAILURES
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceHEXA8[i]) && labs(desceHEXA8[i]) < 16);
#endif
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceValue[10 + i]) && labs(desceValue[10 + i]) < 16);
    }

    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_QUAD4), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_POLYHEDRA), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_NONE), MEDEXCEPTION);
  }
}

void MEDMEMTest::testConnectivity()
{
  //////////////////////////////////////////////
  // TEST 1: test_MEDMEM_PolyConnectivity.cxx //
  //////////////////////////////////////////////
  CONNECTIVITY myNodalConnectivity;
  CONNECTIVITY myDesceConnectivity;

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

  myDesceConnectivity.setPolygonsConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
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

  myDesceConnectivity.setPolyhedronConnectivity(MED_EN::MED_DESCENDING,
                                                PolyhedronDescendingConnectivity,
                                                PolyhedronIndex, DescendingConnectivitySize,
                                                NumberOfPolyhedron);

  cout << "Poly NodalConnectivity:" << endl;
  cout << myNodalConnectivity;
  cout << "Poly NodalConnectivity END" << endl;
  cout << "Poly DescendingConnectivity:" << endl;
  cout << myDesceConnectivity;
  cout << "Poly DescendingConnectivity END" << endl;

  myNodalConnectivity.setNumberOfNodes(777);
  CPPUNIT_ASSERT_EQUAL(777, myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_NODE, MED_EN::MED_NONE));

  // Throws because _constituent is not calculated
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_FACE, MED_EN::MED_NONE), MEDEXCEPTION);

  // Returns zero, because EntityDimension is not set
  CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYGON));
  CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));

  // Throws because entity must differ from MED_NONE and MED_ALL_ELEMENTS
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_NONE), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

  // Throws because types are not defined
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_POLYGON), MEDEXCEPTION);

  // Throws because _nodal is not defined
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.calculateConnectivity
                       (MED_EN::MED_DESCENDING, MED_EN::MED_CELL), MEDEXCEPTION);

  // existPolygonsConnectivity
  CPPUNIT_ASSERT(myNodalConnectivity.existPolygonsConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!myNodalConnectivity.existPolygonsConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!myNodalConnectivity.existPolygonsConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));

  CPPUNIT_ASSERT(!myDesceConnectivity.existPolygonsConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!myDesceConnectivity.existPolygonsConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(myDesceConnectivity.existPolygonsConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));

  // existPolyhedronConnectivity
  CPPUNIT_ASSERT(myNodalConnectivity.existPolyhedronConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!myNodalConnectivity.existPolyhedronConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!myNodalConnectivity.existPolyhedronConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));

  CPPUNIT_ASSERT(!myDesceConnectivity.existPolyhedronConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!myDesceConnectivity.existPolyhedronConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(myDesceConnectivity.existPolyhedronConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));

  // fails because EntityDimension is not set (== 0)
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getPolyTypeRelativeTo(), MEDEXCEPTION);

  // setEntityDimension
  // myNodalConnectivity and myDesceConnectivity is not good, because
  // it contains cells of different dimension (2D and 3D)
  // We set here EntityDimension for good work of below methods
  myNodalConnectivity.setEntityDimension(3);
  myDesceConnectivity.setEntityDimension(2);

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYHEDRA, myNodalConnectivity.getPolyTypeRelativeTo());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYGON, myDesceConnectivity.getPolyTypeRelativeTo());

  // Poly types
  CPPUNIT_ASSERT_EQUAL(1, myNodalConnectivity.getNumberOfTypesWithPoly(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_EQUAL(1, myDesceConnectivity.getNumberOfTypesWithPoly(MED_EN::MED_CELL));

  CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOfTypesWithPoly(MED_EN::MED_FACE));
  CPPUNIT_ASSERT_EQUAL(0, myDesceConnectivity.getNumberOfTypesWithPoly(MED_EN::MED_EDGE));

  CPPUNIT_ASSERT_EQUAL(1, myNodalConnectivity.getNumberOfPolyType());
  CPPUNIT_ASSERT_EQUAL(1, myDesceConnectivity.getNumberOfPolyType());

  // getNumberOfElementsWithPoly
  CPPUNIT_ASSERT_EQUAL(NumberOfPolyhedron, myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));
  CPPUNIT_ASSERT_EQUAL(NumberOfPolygons, myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYGON));
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_FACE, MED_EN::MED_POLYGON), MEDEXCEPTION);

  CPPUNIT_ASSERT_EQUAL(NumberOfPolyhedron, myDesceConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));
  CPPUNIT_ASSERT_EQUAL(NumberOfPolygons, myDesceConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_CELL, MED_EN::MED_POLYGON));
  CPPUNIT_ASSERT_THROW(myDesceConnectivity.getNumberOfElementsWithPoly
                       (MED_EN::MED_FACE, MED_EN::MED_POLYGON), MEDEXCEPTION);

  // getNumberOfElementOfPolyType
  CPPUNIT_ASSERT_EQUAL(NumberOfPolyhedron,
                       myNodalConnectivity.getNumberOfElementOfPolyType(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfElementOfPolyType(MED_EN::MED_FACE), MEDEXCEPTION);

  CPPUNIT_ASSERT_EQUAL(NumberOfPolygons,
                       myDesceConnectivity.getNumberOfElementOfPolyType(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_THROW(myDesceConnectivity.getNumberOfElementOfPolyType(MED_EN::MED_FACE), MEDEXCEPTION);

  // getConnectivityOfAnElementWithPoly
  {
    //#ifdef ENABLE_FORCED_FAILURES
    // This throws "NODAL Connectivity required for a polyhedron"
    // Why? Because this has no sence without index?
    //int len_ph1, len_ph2;
    //const int * nc_ph1 = myNodalConnectivity.getConnectivityOfAnElementWithPoly
    //  (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len_ph1);
    //CPPUNIT_ASSERT_EQUAL(38, len_ph1);
    //const int * nc_ph2 = myNodalConnectivity.getConnectivityOfAnElementWithPoly
    //  (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/2, len_ph2);
    //CPPUNIT_ASSERT_EQUAL(34, len_ph2);
    //#endif

    int len_pg1, len_pg2, i;
    const int * dc_pg1 = myDesceConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/1, len_pg1); // {1,2,3,4,11,10}
    CPPUNIT_ASSERT_EQUAL(6, len_pg1);
    const int * dc_pg2 = myDesceConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/2, len_pg2); // {11,5,6,7,8,9}
    CPPUNIT_ASSERT_EQUAL(6, len_pg2);

    for (i = 0; i < 6; i++) {
      if (dc_pg1[i] < 1 || (4 < dc_pg1[i] && dc_pg1[i] < 10) || 11 < dc_pg1[i] ||
          dc_pg2[i] < 5 || dc_pg2[i] == 10 || 11 < dc_pg2[i])
        CPPUNIT_FAIL("Wrong connectivity returned");
    }
  }

  // We reset here EntityDimension to check getConnectivityOfAnElementWithPoly()
  myNodalConnectivity.setEntityDimension(2);
  myDesceConnectivity.setEntityDimension(3);

  {
    int len_pg1, len_pg2, i;
    const int * nc_pg1 = myNodalConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len_pg1); // {1,2,3,4,5,10}
    CPPUNIT_ASSERT_EQUAL(6, len_pg1);
    const int * nc_pg2 = myNodalConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/2, len_pg2); // {10,5,6,7,8,9}
    CPPUNIT_ASSERT_EQUAL(6, len_pg2);

    for (i = 0; i < 6; i++) {
      if (nc_pg1[i] < 1 || (5 < nc_pg1[i] && nc_pg1[i] != 10) ||
          nc_pg2[i] < 5 || 10 < nc_pg2[i])
        CPPUNIT_FAIL("Wrong connectivity returned");
    }

    int len_ph1, len_ph2;
    const int * dc_ph1 = myDesceConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/1, len_ph1); // {1,2,3,4,5,6,7,8,9}
    CPPUNIT_ASSERT_EQUAL(9, len_ph1);
    const int * dc_ph2 = myDesceConnectivity.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/2, len_ph2); // {10,11,12,13,14,15,16,3,17,18}
    CPPUNIT_ASSERT_EQUAL(10, len_ph2);

    for (i = 0; i < 9; i++) {
      if (dc_ph1[i] < 1 || 9 < dc_ph1[i])
        CPPUNIT_FAIL("Wrong connectivity returned");
    }

    for (i = 0; i < 10; i++) {
      if ((dc_ph2[i] != 3 && dc_ph2[i] < 10) || 18 < dc_ph2[i])
        CPPUNIT_FAIL("Wrong connectivity returned");
    }
  }

  ////////////
  // TEST 2 //
  ////////////
  checkCopyConnectivity();

  ////////////
  // TEST 3 //
  ////////////

  CONNECTIVITY aCells1 (/*numberOfTypes*/2, /*Entity*/MED_EN::MED_CELL);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aCells1.getEntity());
  CPPUNIT_ASSERT_EQUAL(2, aCells1.getNumberOfTypes(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_EQUAL(2, aCells1.getNumberOfTypesWithPoly(MED_EN::MED_CELL));

  CONNECTIVITY aCells2 (/*numberOfTypes*/3/*, Entity=MED_EN::MED_CELL*/);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aCells2.getEntity());
  CPPUNIT_ASSERT_EQUAL(3, aCells2.getNumberOfTypes(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_EQUAL(3, aCells2.getNumberOfTypesWithPoly(MED_EN::MED_CELL));

  CONNECTIVITY * anEdges1 = new CONNECTIVITY(/*numberOfTypes*/1, /*Entity*/MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_EDGE, anEdges1->getEntity());
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getNumberOfTypesWithPoly(MED_EN::MED_EDGE));

  CONNECTIVITY * anEdges2 = new CONNECTIVITY(/*numberOfTypes*/2, /*Entity*/MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_EDGE, anEdges2->getEntity());
  CPPUNIT_ASSERT_EQUAL(2, anEdges2->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(2, anEdges2->getNumberOfTypesWithPoly(MED_EN::MED_EDGE));

  CONNECTIVITY * aFaces1 = new CONNECTIVITY(/*numberOfTypes*/4, /*Entity*/MED_EN::MED_FACE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces1->getEntity());
  CPPUNIT_ASSERT_EQUAL(4, aFaces1->getNumberOfTypes(MED_EN::MED_FACE));
  CPPUNIT_ASSERT_EQUAL(4, aFaces1->getNumberOfTypesWithPoly(MED_EN::MED_FACE));

  // EntityDimension
  // It would be good to set EntityDimension automatically for EDGEs and FACEs,
  // and warn about not set EntityDimension for CELLs
  // (or calculate it by given geometric types)
  aCells1.setEntityDimension(3);
  aCells2.setEntityDimension(2); // for 2D mesh
  anEdges1->setEntityDimension(1);
  anEdges2->setEntityDimension(1);
  aFaces1->setEntityDimension(2);

  CPPUNIT_ASSERT_EQUAL(3, aCells1.getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(2, aCells2.getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(1, anEdges2->getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(2, aFaces1->getEntityDimension());

  // getPolyTypeRelativeTo
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYHEDRA, aCells1.getPolyTypeRelativeTo());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYGON  , aCells2.getPolyTypeRelativeTo());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYGON  , aFaces1->getPolyTypeRelativeTo());
  // because there is no poly types for edges (2D entities)
  CPPUNIT_ASSERT_THROW(anEdges1->getPolyTypeRelativeTo(), MEDEXCEPTION);

  // setConstituent
  CPPUNIT_ASSERT_THROW(aCells1.setConstituent(&aCells2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aCells1.setConstituent(anEdges1), MEDEXCEPTION);

  aCells1.setConstituent(aFaces1);
  aCells1.setConstituent(anEdges1);

  CPPUNIT_ASSERT_EQUAL(1, aCells1.getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(4, aCells1.getNumberOfTypes(MED_EN::MED_FACE));

  aCells2.setConstituent(anEdges2);
  CPPUNIT_ASSERT_EQUAL(2, aCells2.getNumberOfTypes(MED_EN::MED_EDGE));

  // setGeometricTypes
  MED_EN::medGeometryElement aCellTypes2D[3] = {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4, MED_EN::MED_TRIA6};
  MED_EN::medGeometryElement aCellTypes3D[2] = {MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};
  MED_EN::medGeometryElement anEdgeTypes1[1] = {MED_EN::MED_SEG2};
  MED_EN::medGeometryElement anEdgeTypes2[2] = {MED_EN::MED_SEG2, MED_EN::MED_SEG3};
  MED_EN::medGeometryElement aFaceTypes4[4] =
    {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4, MED_EN::MED_TRIA6, MED_EN::MED_QUAD8};

  aCells1.setGeometricTypes(aCellTypes3D, MED_EN::MED_CELL);
  aCells1.setGeometricTypes(aFaceTypes4, MED_EN::MED_FACE);
  aCells1.setGeometricTypes(anEdgeTypes1, MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_THROW(aCells1.setGeometricTypes(anEdgeTypes1, MED_EN::MED_NODE), MEDEXCEPTION);

  aCells2.setGeometricTypes(aCellTypes2D, MED_EN::MED_CELL);
  anEdges2->setGeometricTypes(anEdgeTypes2, MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_THROW(aCells2.setGeometricTypes(aFaceTypes4, MED_EN::MED_FACE), MEDEXCEPTION);

  // setCount
  int countCell2D[4] = {1, 5, 6, 10};
  int countCell3D[3] = {1, 3, 4};
  int countEdges1[2] = {1, 21};
  int countEdges2[3] = {1, 13, 21};
  int countFaces1[5] = {1, 9, 15, 16, 17};

  aCells1.setCount(countCell3D, MED_EN::MED_CELL);
  aCells1.setCount(countEdges1, MED_EN::MED_EDGE);
  aCells1.setCount(countFaces1, MED_EN::MED_FACE);
  CPPUNIT_ASSERT_THROW(aCells1.setCount(countEdges1, MED_EN::MED_NODE), MEDEXCEPTION);

  aCells2.setCount(countCell2D, MED_EN::MED_CELL);
  aCells2.setCount(countEdges2, MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_THROW(aCells2.setCount(countFaces1, MED_EN::MED_FACE), MEDEXCEPTION);

  // setNodal

  // aCells2
  int nodesCell2D_TRIA3[12] = {3,8,7, 10,7,13, 18,13,14, 11,14,8};
  int nodesCell2D_QUAD4[4] = {7,8,14,13};
  int nodesCell2D_TRIA6[24] = {1,2,3,7,10,6, 3,4,5,9,11,8, 11,15,20,19,18,14, 18,17,16,12,10,13};

  aCells2.setNodal(nodesCell2D_TRIA3, MED_EN::MED_CELL, MED_EN::MED_TRIA3);
  aCells2.setNodal(nodesCell2D_QUAD4, MED_EN::MED_CELL, MED_EN::MED_QUAD4);
  aCells2.setNodal(nodesCell2D_TRIA6, MED_EN::MED_CELL, MED_EN::MED_TRIA6);

  int nodesEdges2_SEG2[24] = {3,8, 8,11, 11,14, 14,18, 18,13, 13,10, 10,7,
                              7,3, 7,8, 8,14, 14,13, 13,7};
  int nodesEdges2_SEG3[24] = {1,2,3, 3,4,5, 5,9,11, 11,15,20,
                              20,19,18, 18,17,16, 16,12,10, 10,6,1};

  aCells2.setNodal(nodesEdges2_SEG2, MED_EN::MED_EDGE, MED_EN::MED_SEG2);
  aCells2.setNodal(nodesEdges2_SEG3, MED_EN::MED_EDGE, MED_EN::MED_SEG3);

  // aCells1
  int nodesCell3D_PYRA5[10] = {1,2,3,4,5, 10,6,7,8,9};
  int nodesCell3D_HEXA8[8] = {2,3,4,5, 6,7,8,9};

  aCells1.setNodal(nodesCell3D_PYRA5, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
  aCells1.setNodal(nodesCell3D_HEXA8, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

  int nodesFaces1_TRIA3[24] = {1,2,3, 1,3,4, 1,4,5, 1,5,2,
                               10,7,6, 10,8,7, 10,9,8, 10,6,9};
  int nodesFaces1_QUAD4[24] = {2,3,4,5, 6,7,8,9, 2,3,7,6, 5,4,8,9, 2,5,9,6, 3,4,8,7};
  int nodesFaces1_TRIA6[6] = {11,12,13,14,15,16};
  int nodesFaces1_QUAD8[8] = {15,14,13,17,18,19,20,21};

  aCells1.setNodal(nodesFaces1_TRIA3, MED_EN::MED_FACE, MED_EN::MED_TRIA3);
  aCells1.setNodal(nodesFaces1_QUAD4, MED_EN::MED_FACE, MED_EN::MED_QUAD4);
  aCells1.setNodal(nodesFaces1_TRIA6, MED_EN::MED_FACE, MED_EN::MED_TRIA6);
  aCells1.setNodal(nodesFaces1_QUAD8, MED_EN::MED_FACE, MED_EN::MED_QUAD8);

  int nodesEdges1_SEG2[40] = {1,2, 1,3, 1,4, 1,5, 10,6, 10,7, 10,8, 10,9,
                              2,3, 3,4, 4,5, 5,2,  6,7,  7,8,  8,9,  9,6,
                              2,6, 3,7, 4,8, 5,9};

  aCells1.setNodal(nodesEdges1_SEG2, MED_EN::MED_EDGE, MED_EN::MED_SEG2);

  // setNumberOfNodes
  aCells2.setNumberOfNodes(20);
  anEdges2->setNumberOfNodes(20);

  aCells1.setNumberOfNodes(21);
  anEdges1->setNumberOfNodes(21);
  aFaces1->setNumberOfNodes(21);

  // existConnectivity
  CPPUNIT_ASSERT(aCells1.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(aCells1.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(aCells1.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells1.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(!aCells1.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells1.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!aCells1.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells1.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));

  // getNumberOfPolyType
  CPPUNIT_ASSERT_EQUAL(0, aCells1.getNumberOfPolyType());
  CPPUNIT_ASSERT_EQUAL(0, aCells2.getNumberOfPolyType());

  // getConnectivityOfAnElementWithPoly
  {
    int len_e1, len_e2, i;
    const int * nc_e1 = aCells1.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len_e1);
    CPPUNIT_ASSERT_EQUAL(5, len_e1); // PYRA5 {1,2,3,4,5}
    for (i = 0; i < len_e1; i++) {
      if (nc_e1[i] < 1 || 5 < nc_e1[i])
        CPPUNIT_FAIL("Wrong node in element");
    }

    const int * nc_e2 = aCells2.getConnectivityOfAnElementWithPoly
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/2, len_e2);
    CPPUNIT_ASSERT_EQUAL(3, len_e2); // TRIA3 {7,10,13}
    for (i = 0; i < len_e2; i++) {
      if (nc_e2[i] != 7 && nc_e2[i] != 10 && nc_e2[i] != 13)
        CPPUNIT_FAIL("Wrong node in element");
    }
  }

  //cout << "aCells2: " << endl;
  //cout << aCells2 << endl;

  //cout << "aCells1:" << endl;
  //cout << aCells1 << endl;

  //  aCells1 (2 types)  |
  //     |               |
  //  aFaces1 (4 types)  |  aCells2 (3 types)
  //     |               |     |
  //  anEdges1 (1 type)  |  anEdges2 (2 types)

  // No need to delete anEdges1 and aFaces1, because they are owned by aCells1
  // (anEdges1 is owned by aFaces1 to be precise)
  // No need to delete anEdges2, because they are owned by aCells2

  ////////////
  // TEST 4 //
  ////////////
  CONNECTIVITY * c1 = new CONNECTIVITY(/*numberOfTypes*/2, MED_EN::MED_CELL);
  createOrCheck(c1, "Creation", /*create*/true);
  createOrCheck(c1, "Check just created", /*create*/false);

  CONNECTIVITY * c2 = new CONNECTIVITY(*c1);
  createOrCheck(c2, "Check copy constructor", /*create*/false);

  delete c1;
  delete c2;

  CPPUNIT_FAIL("Case Not Complete");
}
