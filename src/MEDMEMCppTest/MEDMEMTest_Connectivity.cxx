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

#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

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
 *                          const MED_EN::medGeometryElement Type,
 *                          const int * PolyConnectivityIndex=0) throw (MEDEXCEPTION);
 *   (+)     inline void setNumberOfNodes(int NumberOfNodes);
 *   (+)     inline int getEntityDimension() const;
 *   (+)     inline void setEntityDimension(int EntityDimension);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline bool existConnectivity (MED_EN::medConnectivity connectivityType,
 *                                          MED_EN::medEntityMesh Entity) const;
 *   (+)     virtual void calculateConnectivity (MED_EN::medConnectivity connectivityType,
 *                                               MED_EN::medEntityMesh Entity);
 *   (?)     virtual void updateFamily (const vector<FAMILY*>& myFamilies);
 *
 *   (+)     inline MED_EN::medEntityMesh getEntity() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     inline int getNumberOfTypes (MED_EN::medEntityMesh Entity) const;
 *   (+)     inline const MED_EN::medGeometryElement * getGeometricTypes
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   (+)     MED_EN::medGeometryElement getElementType (MED_EN::medEntityMesh Entity, int Number) const;
 *   (+)     inline MED_EN::medGeometryElement getPolyTypeRelativeTo() const;
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     virtual inline const int * getGlobalNumberingIndex
 *                        (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     virtual const int * getConnectivity (MED_EN::medConnectivity ConnectivityType,
 *                                                MED_EN::medEntityMesh Entity,
 *                                                MED_EN::medGeometryElement Type);
 *   (+)     virtual int getConnectivityLength (MED_EN::medConnectivity ConnectivityType,
 *                                              MED_EN::medEntityMesh Entity,
 *                                              MED_EN::medGeometryElement Type);
 *   (+)     virtual const int * getConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
 *                                                     MED_EN::medEntityMesh Entity);
 *   (+)     int *getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const;
 *   (+)     int **getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces,
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
 *   (+)     void invertConnectivityForAFace(int faceId, const int *nodalConnForFace);
 *   -----------------------------------------------------------------------------------------------------
 *   (+)     bool deepCompare(const CONNECTIVITY& other) const;
 *
 *  Use code of MEDMEM/test_copie_connectivity.cxx
 *  }
 */
static void checkConnectivity(CONNECTIVITY * myConnectivity,
                              int MeshDimension, int NumberOfNodes,
                              MED_EN::medEntityMesh Entity,
                              int NumberOfTypes)
{
  int entityDim = myConnectivity->getEntityDimension();
  CPPUNIT_ASSERT_EQUAL(MeshDimension, entityDim);

  int nodesNb = myConnectivity->getNumberOf(MED_EN::MED_NODE, MED_EN::MED_NONE);
  CPPUNIT_ASSERT_EQUAL(NumberOfNodes, nodesNb); //?

  // MED_EN::MED_CELL
  MED_EN::medEntityMesh entity = myConnectivity->getEntity();
  CPPUNIT_ASSERT_EQUAL(Entity, entity);

  int typesNb = myConnectivity->getNumberOfTypes(Entity);
  CPPUNIT_ASSERT_EQUAL(NumberOfTypes, typesNb);

  const MED_EN::medGeometryElement * Types = myConnectivity->getGeometricTypes(Entity);
  CPPUNIT_ASSERT( Types );

  // TODO: implement some more checks
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
    MESSAGE_MED("ERROR : MeshDimension = 1 !");
    MESSAGE_MED("We could not see Reverse Descending Connectivity.");
    return;
  }

  myConnectivity->getReverseConnectivity(MED_EN::MED_DESCENDING, Entity);
  myConnectivity->getReverseConnectivityIndex(MED_EN::MED_DESCENDING, Entity);

  NumberOfConstituents = myConnectivity->getNumberOf(constituentEntity, MED_EN::MED_ALL_ELEMENTS);
  myConnectivity->getConnectivity(MED_EN::MED_NODAL, constituentEntity, MED_EN::MED_ALL_ELEMENTS);
  myConnectivity->getConnectivityIndex(MED_EN::MED_NODAL, constituentEntity);
}

/////////////////////////////////////////
// TEST 2: test_copie_connectivity.cxx //
/////////////////////////////////////////
static void checkCopyConnectivity()
{
  string filename = getResourceFile("pointe.med");
  string meshname = "maa1";

  //Construction d'un maillage
  MESH * myMesh = new MESH;
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

  checkConnectivity(myConnectivity1, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);

  ostringstream ostr1;
  ostr1 << *myConnectivity1;
  CPPUNIT_ASSERT(ostr1.str() != "");

  // COPY
  CONNECTIVITY * myConnectivity2 = new CONNECTIVITY(* myConnectivity0);

  // Compare
  CPPUNIT_ASSERT(myConnectivity2->deepCompare(*myConnectivity0));

  // Compare after deleting the initial connectivity
  myMesh->removeReference();
  myMesh = NULL;
  myConnectivity0 = NULL;

  MED_EN::medEntityMesh anEntity2 = myConnectivity2->getEntity();
  CPPUNIT_ASSERT_EQUAL(anEntity0, anEntity2);

  checkConnectivity(myConnectivity2, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);

  ostringstream ostr2;
  ostr2 << *myConnectivity2;
  CPPUNIT_ASSERT(ostr1.str() == ostr2.str());

  // ONE MORE COPY
  CONNECTIVITY * myConnectivity3 = new CONNECTIVITY(* myConnectivity2);
  delete myConnectivity2;

  MED_EN::medEntityMesh anEntity3 = myConnectivity3->getEntity();
  CPPUNIT_ASSERT_EQUAL(anEntity0, anEntity3);

  checkConnectivity(myConnectivity3, aMeshDimension, aNumberOfNodes, anEntity0, nbOfTypes);

  ostringstream ostr3;
  ostr3 << *myConnectivity3;
  CPPUNIT_ASSERT_EQUAL(ostr1.str(), ostr3.str());

  delete myConnectivity3;
}

static void createOrCheck (CONNECTIVITY * theC, string msg, bool create = false)
{
  // Preconditions: Entity and NumberOfTypes
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_CELL, theC->getEntity());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypes(MED_EN::MED_CELL));

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
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbNodes, theC->getNumberOf
                                 (MED_EN::MED_NODE, MED_EN::MED_NONE));
  }

  // GeometricTypes
  MED_EN::medGeometryElement aCellTypes[3] = {MED_EN::MED_PYRA5, MED_EN::MED_HEXA8,MED_EN::MED_POLYHEDRA};

  // this variable is needed in check mode (!create)
  // because of bug with getGlobalNumberingIndex() method (see below)
  bool triaFirst = true;

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
    delete [] aCellTypesNames;

    // FACES: theC->_constituent
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypes(MED_EN::MED_FACE));
    const MED_EN::medGeometryElement * aFaceTypesBack = theC->getGeometricTypes(MED_EN::MED_FACE);
    triaFirst = (aFaceTypesBack[0] == MED_EN::MED_TRIA3 && aFaceTypesBack[1] == MED_EN::MED_QUAD4);
    CPPUNIT_ASSERT_MESSAGE(msg, triaFirst || (aFaceTypesBack[1] == MED_EN::MED_TRIA3 &&
                                              aFaceTypesBack[0] == MED_EN::MED_QUAD4));

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
    delete [] aFaceTypesNames;
    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfTypes(MED_EN::MED_EDGE));
    //const MED_EN::medGeometryElement * anEdgeTypesBack = theC->getGeometricTypes(MED_EN::MED_EDGE);
    //const CELLMODEL * anEdgeModels = theC->getCellsTypes(MED_EN::MED_EDGE);

    // invalid cases
    CPPUNIT_ASSERT(!theC->getGeometricTypes(MED_EN::MED_NODE));
    CPPUNIT_ASSERT(!theC->getGeometricTypes(MED_EN::MED_ALL_ENTITIES));
    CPPUNIT_ASSERT_THROW(theC->getCellsTypes(MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getCellTypeNames(MED_EN::MED_ALL_ENTITIES), MEDEXCEPTION);
  }

  // 2 POLYHEDRA
  const int nbPolyhedron = 2;
  //const int nbPolyFaces = 14; // 13 unique
  const int nbPolyNodes = 52 + 14 - 2; // = 64

  int aPolyhedronIndex[nbPolyhedron + 1] = {1,33, 65};

  int aPolyhedronNodalConnectivity[nbPolyNodes] = {
    11,15,19,20,17,13,-1, 11,13,14,-1,  14,13,17,18,-1,  18,17,20,-1,  11,14,15,-1,  15,14,18,19,-1,  19,18,20,
    11,13,17,20,19,15,-1, 11,12,13,-1,  13,12,16,17,-1,  17,16,20,-1,  11,15,12,-1,  12,15,19,16,-1,  16,19,20};

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

  // Nodal Connectivity
  int countCells[4] = {1, 3, 4, 6}; // 2 PYRA5 and 1 HEXA8 and 2 POLYHEDRA
  int nodesCells_PYRA5[10] = {2,3,4,5,1, 6,7,8,9,10};
  int nodesCells_HEXA8[8] = {2,3,4,5, 6,7,8,9};
  const int nbClassicCells = countCells[2]-countCells[0];

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
    theC->setNodal(aPolyhedronNodalConnectivity, MED_CELL, MED_POLYHEDRA, aPolyhedronIndex);

    // Invalid cases
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
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS));

    // sorted by geometric type (order is given by the typedef enum medGeometryElement)
    const int * countCellsBack = theC->getGlobalNumberingIndex(MED_EN::MED_CELL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, countCells[0], countCellsBack[0]); // 1: always
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, countCells[1], countCellsBack[1]); // 3: +2 PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, countCells[2], countCellsBack[2]); // 4: +1 HEXA8
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, countCells[3], countCellsBack[3]); // 6: +2 POLYHEDRA

    // nodal connectivity length
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 18+64, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                                      MED_EN::MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 10, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                                      MED_EN::MED_PYRA5));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  8, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                                      MED_EN::MED_HEXA8));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  64, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                                       MED_EN::MED_POLYHEDRA));

    // nodal connectivity index
    const int * connAllIndex = theC->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, connAllIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, connAllIndex[1]); // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 11, connAllIndex[2]); // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 19, connAllIndex[3]); // +8 nodes of HEXA8
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 19+32, connAllIndex[4]); // +32 nodes of POLYHEDRA
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 19+64, connAllIndex[5]); // +32 nodes of POLYHEDRA

    // nodal connectivity
    const int * connAll = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                MED_EN::MED_ALL_ELEMENTS);
    const int * connPYRA5 = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                  MED_EN::MED_PYRA5);
    const int * connHEXA8 = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                  MED_EN::MED_HEXA8);
    const int * connPOLYH = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                  MED_EN::MED_POLYHEDRA);
    for (int i = 0; i < 10; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_PYRA5[i], connPYRA5[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_PYRA5[i], connAll[i]);
    }
    for (int i = 0; i < 8; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_HEXA8[i], connHEXA8[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nodesCells_HEXA8[i], connAll[10 + i]);
    }
    for (int i = 0; i < 64; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aPolyhedronNodalConnectivity[i], connPOLYH[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aPolyhedronNodalConnectivity[i], connAll[18 + i]);
    }

    // descending connectivity length
    // 10 faces in 2 pyra
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 10, theC->getConnectivityLength
                                 (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_PYRA5));
    // 6 faces in 1 hexa
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, theC->getConnectivityLength
                                 (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_HEXA8));
    // 14 faces in 2 polyhedrons
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  14, theC->getConnectivityLength
                                 (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));
    // 10 + 6 + 14 faces
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 30, theC->getConnectivityLength
                                 (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS));

    // descending connectivity index
    const int * descAllIndex = theC->getConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, descAllIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, descAllIndex[1]); // +5 faces of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 11, descAllIndex[2]); // +5 faces of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 17, descAllIndex[3]); // +6 faces of HEXA8
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 24, descAllIndex[4]); // +7 faces of POLYHEDRA
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 31, descAllIndex[5]); // +7 faces of POLYHEDRA

    // descending connectivity
    {
      const int * descAll = theC->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                  MED_EN::MED_ALL_ELEMENTS);
      const int * descPYRA5 = theC->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                    MED_EN::MED_PYRA5);
      const int * descHEXA8 = theC->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                    MED_EN::MED_HEXA8);
      const int * descPOLYH = theC->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                    MED_EN::MED_POLYHEDRA);
      for (int i = 0; i < 10; i++) {
        CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(descPYRA5[i]) && labs(descPYRA5[i]) < 16);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, descAll[i], descPYRA5[i]);
      }
      for (int i = 0; i < 6; i++) {
        CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(descHEXA8[i]) && labs(descHEXA8[i]) < 16);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, descAll[10 + i], descHEXA8[i]);
      }
      for (int i = 0; i < 14; i++) {
        CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(descPOLYH[i]) && labs(descPOLYH[i]) < 31);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, descAll[16 + i], descPOLYH[i]);
      }
    }

    // FACES: theC->_constituent
    CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
    //CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  8, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_TRIA3));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_QUAD4));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  13, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_POLYGON));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 27, theC->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS));

    // sorted by geometric type
    const int * countFacesBack = theC->getGlobalNumberingIndex(MED_EN::MED_FACE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, countFacesBack[0]); // always

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  9, countFacesBack[1]); // +8 TRIA3

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 15, countFacesBack[2]); // 1+8+6

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 28, countFacesBack[3]); // 1+8+6+13

    // nodal connectivity length // 8*3 + 6*4 + 46
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 48+46, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                                                  MED_EN::MED_ALL_ELEMENTS));

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 24, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                                      MED_EN::MED_TRIA3));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 24, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                                      MED_EN::MED_QUAD4));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 46, theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                                               MED_EN::MED_POLYGON));

    // nodal connectivity index
    const int * connFaceAllIndex = theC->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_FACE);
    {
      CPPUNIT_ASSERT(triaFirst);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, connFaceAllIndex[0]);
      int typeChangeIndex = 8;
      int nbNodes1 = 3;
      int nbNodes2 = 4;
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 25, connFaceAllIndex[typeChangeIndex]); // + 3*8 or 4*6
      for (int i = 1; i < 14; i++) {
        if (i < typeChangeIndex)
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1 + i*nbNodes1, connFaceAllIndex[i]);
        else
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 25 + (i-typeChangeIndex)*nbNodes2, connFaceAllIndex[i]);
      }
      // + 3*8 nodes of TRIA3 + 4*6 nodes of QUAD4
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 49, connFaceAllIndex[14]);
    }

    // nodal connectivity
    const int * connFaceAll = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                    MED_EN::MED_ALL_ELEMENTS);
    const int * connTRIA3 = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                  MED_EN::MED_TRIA3);
    const int * connQUAD4 = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE,
                                                  MED_EN::MED_QUAD4);
    for (int i = 0; i < 24; i++) {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, connFaceAll[   i], connTRIA3[i]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, connFaceAll[24+i], connQUAD4[i]);
    }

    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_MESSAGE(msg, theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));

    // Invalid cases
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
    CPPUNIT_ASSERT_MESSAGE(msg, !theC->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_TETRA4));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOf(MED_EN::MED_CELL,
                                                                   MED_EN::MED_TETRA4));
    CPPUNIT_ASSERT_THROW(theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                     MED_EN::MED_TRIA3), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getConnectivityLength(MED_EN::MED_DESCENDING, MED_EN::MED_CELL,
                                                     MED_EN::MED_NONE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getConnectivityLength(MED_EN::MED_NODAL, MED_EN::MED_CELL,
                                                     MED_EN::MED_POLYGON), MEDEXCEPTION);
  }


  if (create) {
  }
  else {
    // CELLS(3D): theC
    {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypes(MED_EN::MED_CELL));

      {
        const MED_EN::medGeometryElement * aCellTypesBack = theC->getGeometricTypes(MED_EN::MED_CELL);
        CPPUNIT_ASSERT_MESSAGE(msg, ((aCellTypesBack[0] == MED_EN::MED_PYRA5 &&
                                      aCellTypesBack[1] == MED_EN::MED_HEXA8) ||
                                     (aCellTypesBack[0] == MED_EN::MED_HEXA8 &&
                                      aCellTypesBack[1] == MED_EN::MED_PYRA5)));
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYHEDRA, aCellTypesBack[2]);
      }

      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOf(MED_EN::MED_CELL,
                                                             MED_EN::MED_POLYHEDRA));
      //checking that 0 is returned if polygons are asked as cells instead of faces
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOf(MED_EN::MED_CELL,
                                                             MED_EN::MED_POLYGON));


      int nbCellAll = 5; // 2 (PYRA5) + 1 (HEXA8) + 2 (POLYHEDRA)

      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbCellAll, theC->getNumberOf
                                   (MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS));


      // first PYRA5 {1,2,3,4,5}
      {
        int len;
        const int * c1 = theC->getConnectivityOfAnElement
          (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, len);
        for (int i = 0; i < len; i++) {
          if (c1[i] < 1 || 5 < c1[i]) CPPUNIT_FAIL(msg);
        }
      }

      // first POLYHEDRA
      {
        int len;
        const int* con = 0;
        CPPUNIT_ASSERT_NO_THROW(con=theC->getConnectivityOfAnElement
                                (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/4, len));
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 32, len);
        for (int i = 0; i < len; i++)
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,aPolyhedronNodalConnectivity[i],con[i]);
      }

      // cells descending connectivity
      for (int i = 1; i <= nbCellAll; i++) {
        int len;
        const int * ci = theC->getConnectivityOfAnElement
          (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/i, len);

        MED_EN::medGeometryElement aCurElemType = theC->getElementType(MED_EN::MED_CELL, i);

        if (i <= 3) { // nb.standard cells = 3
          // sign of connectivity array value means element direction
          for (int j = 0; j < len; j++) {
            CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(ci[j]) && labs(ci[j]) <= 14); // nb.standard faces = 14
          }
        }
        else {
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYHEDRA, aCurElemType);
          for (int j = 0; j < len; j++) {
            CPPUNIT_ASSERT_MESSAGE(msg, 14 < labs(ci[j]) && labs(ci[j]) <= 27); // nb.polygons = 13
          }
        }

        switch (aCurElemType) {
        case MED_EN::MED_PYRA5:     CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, len); break;
        case MED_EN::MED_HEXA8:     CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, len); break;
        case MED_EN::MED_POLYHEDRA: CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 7, len); break;
        default:
          CPPUNIT_FAIL(msg); // wrong element type
        }
      }

      // Polyhedron-specific methods
      {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOf(MED_EN::MED_CELL,
                                                               MED_EN::MED_POLYHEDRA));

        // getNodesOfPolyhedron
        int lenPolyh1nodes, lenPolyh2nodes; // nb of unique nodes
        int * polyh1nodes = theC->getNodesOfPolyhedron(/*polyhedronId*/3+1, lenPolyh1nodes);
        int * polyh2nodes = theC->getNodesOfPolyhedron(/*polyhedronId*/3+2, lenPolyh2nodes);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 8, lenPolyh1nodes);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 8, lenPolyh2nodes);

        set<int> polyh1nodesCheck;
        set<int> polyh2nodesCheck;

        polyh1nodesCheck.insert(11);
        polyh1nodesCheck.insert(13);
        polyh1nodesCheck.insert(14);
        polyh1nodesCheck.insert(15);
        polyh1nodesCheck.insert(17);
        polyh1nodesCheck.insert(18);
        polyh1nodesCheck.insert(19);
        polyh1nodesCheck.insert(20);

        polyh2nodesCheck.insert(11);
        polyh2nodesCheck.insert(12);
        polyh2nodesCheck.insert(13);
        polyh2nodesCheck.insert(15);
        polyh2nodesCheck.insert(16);
        polyh2nodesCheck.insert(17);
        polyh2nodesCheck.insert(19);
        polyh2nodesCheck.insert(20);

        for (int i = 0; i < 8; i++) {
          CPPUNIT_ASSERT_MESSAGE(msg, polyh1nodesCheck.count(polyh1nodes[i]));
          CPPUNIT_ASSERT_MESSAGE(msg, polyh2nodesCheck.count(polyh2nodes[i]));
        }
        delete [] polyh1nodes;
        delete [] polyh2nodes;

        // getNodesPerFaceOfPolyhedron
        int nbFaces1, nbFaces2;
        int *nbNodes1, *nbNodes2; // len = nb.faces (7)
        int ** polyh1nodesPerFace =
          theC->getNodesPerFaceOfPolyhedron(/*polyhedronId*/3+1, nbFaces1, nbNodes1);
        int ** polyh2nodesPerFace =
          theC->getNodesPerFaceOfPolyhedron(/*polyhedronId*/3+2, nbFaces2, nbNodes2);

        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 7, nbFaces1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 7, nbFaces2);

        int nbNodesCheck [7] = {6,3,4,3,3,4,3};
        int startNode1 = aPolyhedronIndex[0] - 1;
        int startNode2 = aPolyhedronIndex[1] - 1;
        for (int i = 0; i < 7; i++) {
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbNodesCheck[i], nbNodes1[i]);
          CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbNodesCheck[i], nbNodes2[i]);

          for (int j = 0; j < nbNodesCheck[i]; j++) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aPolyhedronNodalConnectivity[startNode1],
                                         polyh1nodesPerFace[i][j]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, aPolyhedronNodalConnectivity[startNode2],
                                         polyh2nodesPerFace[i][j]);
            startNode1++;
            startNode2++;
          }
          startNode1++;
          startNode2++;
        }

        delete [] nbNodes1;
        delete [] nbNodes2;
        delete [] polyh1nodesPerFace;
        delete [] polyh2nodesPerFace;

        // invalid polyhedron Id

        int lenPolyh3nodes;
        int nbFaces3;
        int *nbNodes3;
      
        CPPUNIT_ASSERT_THROW(theC->getNodesOfPolyhedron(1, lenPolyh3nodes), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesOfPolyhedron(3+3, lenPolyh3nodes), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesPerFaceOfPolyhedron
                             (/*polyhedronId*/1, nbFaces3, nbNodes3), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesPerFaceOfPolyhedron
                             (/*polyhedronId*/3+3, nbFaces3, nbNodes3), MEDEXCEPTION);


        // Descending

        // PolyhedronIndex: array of size (NumberOfPolyhedron + 1)
        const int* polyhDesceIndex = theC->getConnectivityIndex(MED_EN::MED_DESCENDING,MED_EN::MED_CELL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  polyhDesceIndex[nbClassicCells+0]+7, polyhDesceIndex[nbClassicCells+1]); // +7 faces
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  polyhDesceIndex[nbClassicCells+1]+7, polyhDesceIndex[nbClassicCells+2]); // +7 faces

        // Polyhedron Descending Connectivity: array of size (NumberOfPolyhedronFaces)
        const int* polyhDesceConn = theC->getConnectivity(MED_EN::MED_DESCENDING,MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA);
        // 15,16,17,18,19,20,21, -15,22,23,24,25,26,27
        for (int i = 0; i < 14; i++) {
          // nb. poly faces = 13, because one face is common for two polyhedra
          // nb. standard faces < poly-face id <= 27 (27 = 14 + 13)
          CPPUNIT_ASSERT_MESSAGE(msg, 14 < labs(polyhDesceConn[i]) && labs(polyhDesceConn[i]) <= 27);
        }
      } // Polyhedron-specific methods

      // Polygon-specific methods
      {
        // Invalid cases: no polygons for MED_CELL in theC
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, theC->getNumberOf(MED_EN::MED_CELL,MED_EN::MED_POLYGON));
        CPPUNIT_ASSERT_THROW(theC->getConnectivity(MED_EN::MED_DESCENDING,
                                                   MED_EN::MED_CELL,MED_EN::MED_POLYGON), MEDEXCEPTION);
      }
    } // CELLS: theC

    // FACES: theC->_constituent
    {
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypes(MED_EN::MED_FACE));

      const MED_EN::medGeometryElement * aFaceTypesBack = theC->getGeometricTypes(MED_EN::MED_FACE);
      CPPUNIT_ASSERT_MESSAGE(msg, ((aFaceTypesBack[0] == MED_EN::MED_TRIA3 &&
                                    aFaceTypesBack[1] == MED_EN::MED_QUAD4) ||
                                   (aFaceTypesBack[0] == MED_EN::MED_QUAD4 &&
                                    aFaceTypesBack[1] == MED_EN::MED_TRIA3)));
      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYGON, aFaceTypesBack[2]);

      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  0, theC->getNumberOf
                                   (MED_EN::MED_FACE, MED_EN::MED_POLYHEDRA));

      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 13, theC->getNumberOf
                                   (MED_EN::MED_FACE, MED_EN::MED_POLYGON));

      int nbFaAll = 27; // 6 (QUAD4) + 8 (TRIA3) + 13 (POLYGON)

      CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, nbFaAll, theC->getNumberOf
                                   (MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS));


      bool isHexagon = false;
      for (int i = 1; i <= nbFaAll; i++) {
        int len;
        const int * ci = theC->getConnectivityOfAnElement(MED_EN::MED_NODAL,
                                                                  MED_EN::MED_FACE, /*Number*/i, len);
        MED_EN::medGeometryElement aCurElemType = theC->getElementType(MED_EN::MED_FACE, i);

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

        switch (aCurElemType) {
        case MED_EN::MED_TRIA3:   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, len); break;
        case MED_EN::MED_QUAD4:   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 4, len); break;
        case MED_EN::MED_POLYGON: CPPUNIT_ASSERT_MESSAGE(msg, len == 3 || len == 4 || len == 6); break;
        default:
          CPPUNIT_FAIL(msg); // wrong element type
        }
      }
      CPPUNIT_ASSERT_MESSAGE(msg, isHexagon); // hexagon must exist

      // Polygon-specific methods
      {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 13, theC->getNumberOf(MED_EN::MED_FACE,MED_EN::MED_POLYGON));

        const int * pgIndx = theC->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_FACE) + 8+6;
        CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 46, pgIndx[13]-pgIndx[0]);

        const int * pgConn = theC->getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS);

        // face #  1: 11 15 19 20 17 13
        // face #  2: 11 13 14
        // face #  3: 14 13 17 18
        // face #  4: 18 17 20
        // face #  5: 11 14 15
        // face #  6: 15 14 18 19
        // face #  7: 19 18 20
        // face #  8: 11 12 13
        // face #  9: 13 12 16 17
        // face # 10: 17 16 20
        // face # 11: 11 15 12
        // face # 12: 12 15 19 16
        // face # 13: 16 19 20

        for (int i = 0; i < 13; i++) {
          int startNode = pgIndx[i];
          int finishNode = pgIndx[i+1];
          // check nodes uniqueness inside one polygon
          set<int> curNodes;
          for (int j = startNode; j < finishNode; j++) {
            CPPUNIT_ASSERT_MESSAGE(msg, (curNodes.insert(pgConn[j - 1])).second);
          }
        }
      }
    } // FACES: theC->_constituent

    // EDGES: theC->_constituent->_constituent
    //CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_EDGE));
  }

  if (create) {
    // force _constituent computation
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, theC->getNumberOfTypes(MED_EN::MED_FACE));


    //N-2 Connectivity not supported in MEDMEM
    //CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 1, theC->getNumberOfTypes(MED_EN::MED_EDGE));

  }
  else {
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_POLYHEDRA, theC->getPolyTypeRelativeTo());

    // The following methods are not recursive, i.e. they return types
    // of this connectivity, but do not return types of _constituent.
    // And these methods DO work with poly-types.

    // getType
    const CELLMODEL & aPYRA5_type = theC->getType(MED_EN::MED_PYRA5);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_PYRA5, aPYRA5_type.getType());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 5, aPYRA5_type.getNumberOfVertexes());

    const CELLMODEL & aHEXA8_type = theC->getType(MED_EN::MED_HEXA8);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 8, aHEXA8_type.getNumberOfNodes());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, aHEXA8_type.getDimension());
    // nb. of sub-faces (nb. of constituents with dimension = 3 - 1)
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 6, aHEXA8_type.getNumberOfConstituents(1));

    const CELLMODEL & aPOLYH_type = theC->getType(MED_EN::MED_POLYHEDRA);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, aPOLYH_type.getNumberOfNodes());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 3, aPOLYH_type.getDimension());

    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_TRIA3), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getType(MED_EN::MED_POLYGON), MEDEXCEPTION);
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

    int nbFacesInPOLYH = theC->getNumberOfSubCellInType(MED_EN::MED_POLYHEDRA);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 0, nbFacesInPOLYH);

    CPPUNIT_ASSERT_THROW(theC->getNumberOfSubCellInType(MED_EN::MED_QUAD4), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getNumberOfSubCellInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

    // getValueIndex
    const int* nodalIndex = theC->getValueIndex(MED_EN::MED_NODAL);
    const int* desceIndex = theC->getValueIndex(MED_EN::MED_DESCENDING);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, nodalIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, nodalIndex[1]); // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 11, nodalIndex[2]); // +5 nodes of PYRA5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 19, nodalIndex[3]); // +8 nodes of HEXA8

    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  1, desceIndex[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,  6, desceIndex[1]); // +5 faces of PYRA5
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
    const int* descePYRA5 = theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_PYRA5);
    const int* desceHEXA8 = theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_HEXA8);

    for (int i = 0; i < 10; i++) {
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(descePYRA5[i]) && labs(descePYRA5[i]) < 16);

      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceValue[i]) && labs(desceValue[i]) < 16);
    }
    for (int i = 0; i < 6; i++) {
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceHEXA8[i]) && labs(desceHEXA8[i]) < 16);
      CPPUNIT_ASSERT_MESSAGE(msg, 0 < labs(desceValue[10 + i]) && labs(desceValue[10 + i]) < 16);
    }

    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_QUAD4), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_NODAL, MED_EN::MED_POLYGON), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->getValue(MED_EN::MED_DESCENDING, MED_EN::MED_NONE), MEDEXCEPTION);
  }
}

void MEDMEMTest::testConnectivity()
{
  //////////////////////////////////////////////
  // TEST 1: test_MEDMEM_PolyConnectivity.cxx //
  //////////////////////////////////////////////
  CONNECTIVITY myNodalConnectivity(1); // one type
  myNodalConnectivity.setEntityDimension(3);

  MED_EN::medGeometryElement types[] = { MED_EN::MED_POLYHEDRA };

  //POLYHEDRON
  const int NumberOfPolyhedron = 2;
  int PolyhedronIndex[NumberOfPolyhedron+1] = {1,47,91};

  //Nodal
  const int NumberOfFaces = 19;
  int PolyhedronNodalConnectivity[] ={1, 2, 3, 4, 5, 6, -1,// Polyhedron 1
                                      1, 7, 8, 2,       -1,
                                      2, 8, 9, 3,       -1,
                                      4, 3, 9, 10,      -1,
                                      5, 4, 10, 11,     -1,
                                      6, 5, 11, 12,     -1,
                                      1, 6, 12, 7,      -1,
                                      7, 12, 8,         -1,
                                      10, 9, 8, 12, 11,     

                                      13, 14, 15, 3, 2, -1,// Polyhedron 2
                                      13, 2, 8, 16,     -1,
                                      14, 13, 16, 17,   -1,
                                      15, 14, 17,       -1,
                                      15, 17, 18,       -1,
                                      15, 18, 9,        -1,
                                      3, 15, 9,         -1,
                                      2, 3, 9, 8,       -1,
                                      8, 9, 17, 16,     -1,
                                      9, 18, 17 };

  const int count[] = { 1, 1+NumberOfPolyhedron };
  myNodalConnectivity.setGeometricTypes( &types[0], MED_EN::MED_CELL );
  myNodalConnectivity.setCount( count, MED_EN::MED_CELL );
  myNodalConnectivity.setNodal( PolyhedronNodalConnectivity, MED_EN::MED_CELL,
                                MED_EN::MED_POLYHEDRA, PolyhedronIndex);

  myNodalConnectivity.setNumberOfNodes(777);
  CPPUNIT_ASSERT_EQUAL(777, myNodalConnectivity.getNumberOf
                       (MED_EN::MED_NODE, MED_EN::MED_NONE));
//   // Throws because _constituent is not calculated
//   CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOf
//                        (MED_EN::MED_FACE, MED_EN::MED_NONE), MEDEXCEPTION);

//   // Returns zero, because EntityDimension is not set
//   CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOf
//                        (MED_EN::MED_CELL, MED_EN::MED_POLYGON));
//   CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOf
//                        (MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));

  // Throws because entity must differ from MED_NONE and MED_ALL_ELEMENTS
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_NONE), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);

  // Throws because types are not defined
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_ALL_ELEMENTS), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(myNodalConnectivity.getNumberOfNodesInType(MED_EN::MED_POLYGON), MEDEXCEPTION);

  // does not throw any more exception because 
  // it is now possible to work on meshes with only polyhedric elements
  CPPUNIT_ASSERT_NO_THROW(myNodalConnectivity.calculateConnectivity (MED_EN::MED_DESCENDING,
                                                                     MED_EN::MED_CELL));

  // existPolygonsConnectivity
  CPPUNIT_ASSERT(myNodalConnectivity.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!myNodalConnectivity.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));

  // existPolyhedronConnectivity
  CPPUNIT_ASSERT(myNodalConnectivity.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(myNodalConnectivity.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));


  // setEntityDimension
  // it contains cells of different dimension (2D and 3D)
  // We set here EntityDimension for good work of below methods

  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYHEDRA, myNodalConnectivity.getPolyTypeRelativeTo());

  // Poly types
  CPPUNIT_ASSERT_EQUAL(1, myNodalConnectivity.getNumberOfTypes(MED_EN::MED_CELL));

  CPPUNIT_ASSERT_EQUAL(1, myNodalConnectivity.getNumberOfTypes(MED_EN::MED_FACE));

  // getNumberOf
  CPPUNIT_ASSERT_EQUAL(NumberOfPolyhedron, myNodalConnectivity.getNumberOf
                       (MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA));
  CPPUNIT_ASSERT_EQUAL(0, myNodalConnectivity.getNumberOf
                       (MED_EN::MED_CELL, MED_EN::MED_POLYGON));
  //Minus 1 because 2,3,8,9 is a given twice in connectivity
  CPPUNIT_ASSERT_EQUAL(NumberOfFaces-1, myNodalConnectivity.getNumberOf (MED_EN::MED_FACE,
                                                                         MED_EN::MED_POLYGON));

  // getConnectivityOfAnElement
  {
    //first polyhedron
    const int *ph1, *ph2;
    int len_ph1, len_ph2;
    CPPUNIT_ASSERT_NO_THROW(ph1 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len_ph1));
    CPPUNIT_ASSERT_EQUAL( PolyhedronIndex[1]-PolyhedronIndex[0], len_ph1 );
    CPPUNIT_ASSERT( vector<int>( PolyhedronNodalConnectivity, PolyhedronNodalConnectivity + len_ph1) ==
                    vector<int>( ph1, ph1+len_ph1 ));

    //second polyhedron
    CPPUNIT_ASSERT_NO_THROW(ph2 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/2, len_ph2));
    CPPUNIT_ASSERT_EQUAL( PolyhedronIndex[2]-PolyhedronIndex[1], len_ph2 );
    CPPUNIT_ASSERT( vector<int>( PolyhedronNodalConnectivity + len_ph1, PolyhedronNodalConnectivity + len_ph1 + len_ph2) ==
                    vector<int>( ph2, ph2+len_ph2 ));

    // MED_DESCENDING
    //first polyhedron
    CPPUNIT_ASSERT_NO_THROW(ph1 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/1, len_ph1));
    CPPUNIT_ASSERT_EQUAL( 9, len_ph1 );
    const int faces1[] = { 1,2,3,4,5,6,7,8,9 };
    CPPUNIT_ASSERT( vector<int>( faces1, faces1 + len_ph1 ) ==
                    vector<int>( ph1, ph1+len_ph1 ));

    //second polyhedron
    CPPUNIT_ASSERT_NO_THROW(ph2 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_DESCENDING, MED_EN::MED_CELL, /*Number*/2, len_ph2));
    CPPUNIT_ASSERT_EQUAL( 10, len_ph2 );
    const int faces2[] = { 10,11,12,13,14,15,16,-3,17,18 };
    CPPUNIT_ASSERT( vector<int>( faces2, faces2 + len_ph2 ) ==
                    vector<int>( ph2, ph2+len_ph2 ));
  }

  // We reset here EntityDimension to check getConnectivityOfAnElement()
  //myNodalConnectivity.setEntityDimension(2);

  {
    const int *ph1, *ph2;
    int len_ph1, len_ph2;
    // first polygon
    CPPUNIT_ASSERT_NO_THROW(ph1 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_NODAL, MED_EN::MED_FACE, /*Number*/1, len_ph1));
    CPPUNIT_ASSERT_EQUAL( 6, len_ph1 );
    CPPUNIT_ASSERT( vector<int>( PolyhedronNodalConnectivity, PolyhedronNodalConnectivity + len_ph1) ==
                    vector<int>( ph1, ph1+len_ph1 ));

    // second polygon
    CPPUNIT_ASSERT_NO_THROW(ph2 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_NODAL, MED_EN::MED_FACE, /*Number*/2, len_ph2));
    CPPUNIT_ASSERT_EQUAL( 4, len_ph2 );
    CPPUNIT_ASSERT( vector<int>( PolyhedronNodalConnectivity + len_ph1 + 1, PolyhedronNodalConnectivity + len_ph1 + 1 + len_ph2) ==
                    vector<int>( ph2, ph2+len_ph2 ));

    // MED_DESCENDING
    // first polygon
    CPPUNIT_ASSERT_NO_THROW(ph1 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_DESCENDING, MED_EN::MED_FACE, /*Number*/1, len_ph1));
    CPPUNIT_ASSERT_EQUAL( 6, len_ph1 );
    const int edges1[6] = { 1,2,3,4,5,6 };
    CPPUNIT_ASSERT( vector<int>( edges1, edges1 + len_ph1 ) ==
                    vector<int>( ph1, ph1+len_ph1 ));

    // second polygon
    CPPUNIT_ASSERT_NO_THROW(ph2 = myNodalConnectivity.getConnectivityOfAnElement
                            (MED_EN::MED_DESCENDING, MED_EN::MED_FACE, /*Number*/2, len_ph2));
    CPPUNIT_ASSERT_EQUAL( 4, len_ph2 );
    const int edges2[4] = { 7,8,9,-1 };
    CPPUNIT_ASSERT( vector<int>( edges2, edges2 + len_ph2 ) ==
                    vector<int>( ph2, ph2+len_ph2 ));
  }

  ////////////
  // TEST 2 //
  ////////////
  checkCopyConnectivity();

  ////////////
  // TEST 3 //
  ////////////

  CONNECTIVITY *aCells1=new CONNECTIVITY(/*numberOfTypes*/2, /*Entity*/MED_EN::MED_CELL);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aCells1->getEntity());
  CPPUNIT_ASSERT_EQUAL(2, aCells1->getNumberOfTypes(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_EQUAL(2, aCells1->getNumberOfTypes(MED_EN::MED_CELL));

  CONNECTIVITY aCells2 (/*numberOfTypes*/3/*, Entity=MED_EN::MED_CELL*/);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_CELL, aCells2.getEntity());
  CPPUNIT_ASSERT_EQUAL(3, aCells2.getNumberOfTypes(MED_EN::MED_CELL));
  CPPUNIT_ASSERT_EQUAL(3, aCells2.getNumberOfTypes(MED_EN::MED_CELL));

  CONNECTIVITY * anEdges1 = new CONNECTIVITY(/*numberOfTypes*/1, /*Entity*/MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_EDGE, anEdges1->getEntity());
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getNumberOfTypes(MED_EN::MED_EDGE));

  CONNECTIVITY * anEdges2 = new CONNECTIVITY(/*numberOfTypes*/2, /*Entity*/MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_EDGE, anEdges2->getEntity());
  CPPUNIT_ASSERT_EQUAL(2, anEdges2->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(2, anEdges2->getNumberOfTypes(MED_EN::MED_EDGE));

  CONNECTIVITY * aFaces1 = new CONNECTIVITY(/*numberOfTypes*/2, /*Entity*/MED_EN::MED_FACE);
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_FACE, aFaces1->getEntity());
  CPPUNIT_ASSERT_EQUAL(2, aFaces1->getNumberOfTypes(MED_EN::MED_FACE));
  CPPUNIT_ASSERT_EQUAL(2, aFaces1->getNumberOfTypes(MED_EN::MED_FACE));

  // No need to delete anEdges1 and aFaces1, because they are owned by aCells1
  // (anEdges1 is owned by aFaces1 to be precise)
  // No need to delete anEdges2, because they are owned by aCells2

  // EntityDimension
  // It would be good to set EntityDimension automatically for EDGEs and FACEs,
  // and warn about not set EntityDimension for CELLs
  // (or calculate it by given geometric types)
  aCells1->setEntityDimension(3);
  aCells2.setEntityDimension(2); // for 2D mesh
  anEdges1->setEntityDimension(1);
  anEdges2->setEntityDimension(1);
  aFaces1->setEntityDimension(2);

  CPPUNIT_ASSERT_EQUAL(3, aCells1->getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(2, aCells2.getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(1, anEdges1->getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(1, anEdges2->getEntityDimension());
  CPPUNIT_ASSERT_EQUAL(2, aFaces1->getEntityDimension());

  // getPolyTypeRelativeTo
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYHEDRA, aCells1->getPolyTypeRelativeTo());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYGON  , aCells2.getPolyTypeRelativeTo());
  CPPUNIT_ASSERT_EQUAL(MED_EN::MED_POLYGON  , aFaces1->getPolyTypeRelativeTo());
  // because there is no poly types for edges (2D entities)
  CPPUNIT_ASSERT_THROW(anEdges1->getPolyTypeRelativeTo(), MEDEXCEPTION);

  // setConstituent
  CPPUNIT_ASSERT_THROW(aCells1->setConstituent(&aCells2), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW(aCells1->setConstituent(anEdges1), MEDEXCEPTION);

  aCells1->setConstituent(aFaces1);
  aCells1->setConstituent(anEdges1);

  CPPUNIT_ASSERT_EQUAL(1, aCells1->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(2, aCells1->getNumberOfTypes(MED_EN::MED_FACE));

  aCells2.setConstituent(anEdges2);
  CPPUNIT_ASSERT_EQUAL(2, aCells2.getNumberOfTypes(MED_EN::MED_EDGE));

  // setGeometricTypes
  MED_EN::medGeometryElement aCellTypes2D[3] = {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4, MED_EN::MED_TRIA6};
  MED_EN::medGeometryElement aCellTypes3D[2] = {MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};
  MED_EN::medGeometryElement anEdgeTypes1[1] = {MED_EN::MED_SEG2};
  MED_EN::medGeometryElement anEdgeTypes2[2] = {MED_EN::MED_SEG2, MED_EN::MED_SEG3};
  MED_EN::medGeometryElement aFaceTypes2[2] =
    {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4};

  aCells1->setGeometricTypes(aCellTypes3D, MED_EN::MED_CELL);
  aCells1->setGeometricTypes(aFaceTypes2, MED_EN::MED_FACE);
  aCells1->setGeometricTypes(anEdgeTypes1, MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_THROW(aCells1->setGeometricTypes(anEdgeTypes1, MED_EN::MED_NODE), MEDEXCEPTION);

  aCells2.setGeometricTypes(aCellTypes2D, MED_EN::MED_CELL);
  anEdges2->setGeometricTypes(anEdgeTypes2, MED_EN::MED_EDGE);
  CPPUNIT_ASSERT_THROW(aCells2.setGeometricTypes(aFaceTypes2, MED_EN::MED_FACE), MEDEXCEPTION);

  // setCount
  int countCell2D[4] = {1, 5, 6, 10};
  int countCell3D[3] = {1, 3, 4};
  int countEdges1[2] = {1, 21};
  int countEdges2[3] = {1, 13, 21};
  int countFaces1[3] = {1, 9, 15};

  aCells1->setCount(countCell3D, MED_EN::MED_CELL);
  aCells1->setCount(countEdges1, MED_EN::MED_EDGE);
  aCells1->setCount(countFaces1, MED_EN::MED_FACE);
  CPPUNIT_ASSERT_THROW(aCells1->setCount(countEdges1, MED_EN::MED_NODE), MEDEXCEPTION);

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
  int nodesCell3D_PYRA5[10] = {5,4,3,2,1, 6,7,8,9,10};
  int nodesCell3D_HEXA8[8] = {2,3,4,5, 6,7,8,9};

  aCells1->setNodal(nodesCell3D_PYRA5, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
  aCells1->setNodal(nodesCell3D_HEXA8, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

  int nodesFaces1_TRIA3[24] = {1,2,3, 1,3,4, 1,4,5, 1,5,2,
                               10,6,7, 10,7,8,  10,8,9, 10,9,6};
  int nodesFaces1_QUAD4[24] = {2,3,4,5, 6,7,8,9, 2,3,7,6, 5,4,8,9, 2,5,9,6, 3,4,8,7};
  // int nodesFaces1_TRIA6[6] = {11,12,13,14,15,16};
  // int nodesFaces1_QUAD8[8] = {15,14,13,17,18,19,20,21};

  aCells1->setNodal(nodesFaces1_TRIA3, MED_EN::MED_FACE, MED_EN::MED_TRIA3);
  aCells1->setNodal(nodesFaces1_QUAD4, MED_EN::MED_FACE, MED_EN::MED_QUAD4);
  //aCells1->setNodal(nodesFaces1_TRIA6, MED_EN::MED_FACE, MED_EN::MED_TRIA6);
  //aCells1->setNodal(nodesFaces1_QUAD8, MED_EN::MED_FACE, MED_EN::MED_QUAD8);

  int nodesEdges1_SEG2[40] = {1,2, 1,3, 1,4, 1,5, 10,6, 10,7, 10,8, 10,9,
                              2,3, 3,4, 4,5, 5,2,  6,7,  7,8,  8,9,  9,6,
                              2,6, 3,7, 4,8, 5,9};

  aCells1->setNodal(nodesEdges1_SEG2, MED_EN::MED_EDGE, MED_EN::MED_SEG2);

  // setNumberOfNodes
  aCells2.setNumberOfNodes(20);
  anEdges2->setNumberOfNodes(20);

  aCells1->setNumberOfNodes(10);
  anEdges1->setNumberOfNodes(10);
  aFaces1->setNumberOfNodes(10);

  // existConnectivity
  CPPUNIT_ASSERT(aCells1->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(aCells1->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(aCells1->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells1->existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(!aCells1->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells1->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!aCells1->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells1->existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_NODAL, MED_EN::MED_NODE));

  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_FACE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_EDGE));
  CPPUNIT_ASSERT(!aCells2.existConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_NODE));

  // getConnectivityOfAnElement
  {
    int len_e1, len_e2, i;
    const int * nc_e1 = aCells1->getConnectivityOfAnElement
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/1, len_e1);
    CPPUNIT_ASSERT_EQUAL(5, len_e1); // PYRA5 {1,2,3,4,5}
    for (i = 0; i < len_e1; i++) {
      if (nc_e1[i] < 1 || 5 < nc_e1[i])
        CPPUNIT_FAIL("Wrong node in element");
    }

    const int * nc_e2 = aCells2.getConnectivityOfAnElement
      (MED_EN::MED_NODAL, MED_EN::MED_CELL, /*Number*/2, len_e2);
    CPPUNIT_ASSERT_EQUAL(3, len_e2); // TRIA3 {7,10,13}
    for (i = 0; i < len_e2; i++) {
      if (nc_e2[i] != 7 && nc_e2[i] != 10 && nc_e2[i] != 13)
        CPPUNIT_FAIL("Wrong node in element");
    }
  }

  //  aCells1 (2 types)  |
  //     |               |
  //  aFaces1 (4 types)  |  aCells2 (3 types)
  //     |               |     |
  //  anEdges1 (1 type)  |  anEdges2 (2 types)
  
  MESH* mesh=new MESH; //updateFamily method requires a pointer to the mesh 
  mesh->setConnectivityptr(aCells1);
  // updateFamily
  {
    FAMILY *aFamilyOnFaces=new FAMILY;
    aFamilyOnFaces->setEntity(MED_EN::MED_FACE);
    aFamilyOnFaces->setMeshName("Mesh 1");
    aFamilyOnFaces->setMesh(mesh);
    mesh->removeReference();
    aFamilyOnFaces->setName("Support On Faces 1");
    //aFamilyOnFaces->setAll(true);

    int nbTypesFam1 = 2;
    MED_EN::medGeometryElement aSCTypes[4] = {MED_EN::MED_TRIA3, MED_EN::MED_QUAD4};
    int nbEltsSC[4] = {8,6};
    int indexSC[5] = {1,9,15}; // length = nb.types + 1
    int valueSC[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14}; // length = total nb. of elements
    //int nbTypesFam1 = 1;
    //MED_EN::medGeometryElement aSCTypes[1] = {MED_EN::MED_TRIA3};
    //int nbEltsSC[1] = {8};
    //int indexSC[2] = {1,9}; // length = nb.types + 1
    //int valueSC[8] = {1,3,5,7,9,11,13,15}; // length = total nb. of elements

    aFamilyOnFaces->setNumberOfGeometricType(nbTypesFam1);
    aFamilyOnFaces->setGeometricType(aSCTypes);
    aFamilyOnFaces->setNumberOfElements(nbEltsSC);
    aFamilyOnFaces->setNumber(indexSC, valueSC);

    vector<FAMILY*> aFamsOnFaces (1);
    aFamsOnFaces[0] = aFamilyOnFaces;

    // Attention!!! By default ENABLE_UPDATE_FAMILY is not defined!!!
    // I do not undestand, what this method should do
    // and what I must give to it to obtain good result

    CPPUNIT_ASSERT_NO_THROW(aCells1->updateFamily(aFamsOnFaces));
    aFamilyOnFaces->removeReference();
  }

  ////////////
  // TEST 4 //
  ////////////
  CONNECTIVITY * c1 = new CONNECTIVITY(/*numberOfTypes*/3, MED_EN::MED_CELL);
  createOrCheck(c1, "Creation", /*create*/true);
  createOrCheck(c1, "Check just created", /*create*/false);

  CONNECTIVITY * c2 = new CONNECTIVITY(*c1);
  createOrCheck(c2, "Check copy constructor", /*create*/false);

  // invertConnectivityForAFace
  int nbFacesC2 = c2->getNumberOf(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS);
  for (int faceId = 1; faceId <= nbFacesC2; faceId++) {

    // this face nodal connectivity before inversion:
    int oldLen, newLen;
    const int * oldConn = c2->getConnectivityOfAnElement(MED_EN::MED_NODAL,
                                                         MED_EN::MED_FACE, faceId, oldLen);

    // descending connectivity before inversion:
    int before_NumberOfElements = c2->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    const int * before_connectivity_shared =
      c2->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    const int * before_connectivity_index =
      c2->getConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);

    // copy connectivity, because the pointer, returned by getConnectivity,
    // will point to the same memory before and after inversion
    int lenDC = before_connectivity_index[before_NumberOfElements] - 1;
    int * before_connectivity = new int[lenDC];
    for (int i = 0; i < lenDC; i++)
      before_connectivity[i] = before_connectivity_shared[i];

    // reverse descending connectivity before inversion:
    const int * before_ReverseDescendingConnectivity_shared =
      c2->getReverseConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);
    const int * before_ReverseDescendingConnectivityIndex =
      c2->getReverseConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);

    int lenRDC = before_ReverseDescendingConnectivityIndex[nbFacesC2] - 1;
    int * before_ReverseDescendingConnectivity = new int[lenRDC];
    for (int i = 0; i < lenRDC; i++)
      before_ReverseDescendingConnectivity[i] = before_ReverseDescendingConnectivity_shared[i];

    // perform inversion
    int * newNodesForFace = new int[oldLen];
    if (oldLen == 3) {
      newNodesForFace[0] = oldConn[1];
      newNodesForFace[1] = oldConn[0];
      newNodesForFace[2] = oldConn[2];
    } else {
      newNodesForFace[0] = oldConn[2];
      newNodesForFace[1] = oldConn[1];
      newNodesForFace[2] = oldConn[0];
      newNodesForFace[3] = oldConn[3];
    }
    c2->invertConnectivityForAFace(faceId, newNodesForFace);

    // reverse descending connectivity after inversion:
    const int * after_ReverseDescendingConnectivity =
      c2->getReverseConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);
    const int * after_ReverseDescendingConnectivityIndex =
      c2->getReverseConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);

    // Faces, which are on bound (have one neighbouring), are not inverted.
    bool isOnBound = false;

    for (int i = 0; i < nbFacesC2; i++) {
      int plus = after_ReverseDescendingConnectivityIndex[i] - 1;
      // always two neighbourings
      if ((i + 1) == faceId) {
        // no second neighbouring
        isOnBound = (before_ReverseDescendingConnectivity[plus + 1] == 0);
      }
      if ((i + 1) == faceId && !isOnBound) {
        CPPUNIT_ASSERT_EQUAL(before_ReverseDescendingConnectivity[plus + 0],
                             after_ReverseDescendingConnectivity[plus + 1]);
        CPPUNIT_ASSERT_EQUAL(before_ReverseDescendingConnectivity[plus + 1],
                             after_ReverseDescendingConnectivity[plus + 0]);
      }
      else {
        CPPUNIT_ASSERT_EQUAL(before_ReverseDescendingConnectivity[plus + 0],
                             after_ReverseDescendingConnectivity[plus + 0]);
        CPPUNIT_ASSERT_EQUAL(before_ReverseDescendingConnectivity[plus + 1],
                             after_ReverseDescendingConnectivity[plus + 1]);
      }
    }

    // descending connectivity after inversion:
    int after_NumberOfElements = c2->getNumberOf(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    const int * after_connectivity =
      c2->getConnectivity(MED_EN::MED_DESCENDING, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    const int * after_connectivity_index =
      c2->getConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL);

    CPPUNIT_ASSERT_EQUAL(before_NumberOfElements, after_NumberOfElements);

    for (int j = 0; j < before_NumberOfElements; j++) {
      for (int k = after_connectivity_index[j]; k < after_connectivity_index[j+1]; k++) {
        if (labs(before_connectivity[k-1]) == faceId && !isOnBound) {
          CPPUNIT_ASSERT_EQUAL(before_connectivity[k-1], - after_connectivity[k-1]);
        }
        else {
          CPPUNIT_ASSERT_EQUAL(before_connectivity[k-1], after_connectivity[k-1]);
        }
      }
    }

    // this face nodal connectivity after inversion:
    if (!isOnBound) {
      const int * newConn = c2->getConnectivityOfAnElement(MED_EN::MED_NODAL,
                                                           MED_EN::MED_FACE, faceId, newLen);
      CPPUNIT_ASSERT_EQUAL(oldLen, newLen);
      for (int i = 0; i < newLen && i < 4; i++) {
        CPPUNIT_ASSERT_EQUAL(newNodesForFace[i], newConn[i]);
      }
    }
    delete [] newNodesForFace;

    delete [] before_connectivity;
    delete [] before_ReverseDescendingConnectivity;

    // ATTENTION: invertConnectivityForAFace() is not tested on polygons!!!
  }


  delete c1;
  delete c2;
}
