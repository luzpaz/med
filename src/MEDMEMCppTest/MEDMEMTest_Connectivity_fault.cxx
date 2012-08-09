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

using namespace std;
using namespace MEDMEM;

static void createOrCheck (CONNECTIVITY * theC, string msg, bool create = false)
{
  // Preconditions: Entity and NumberOfTypes
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, MED_EN::MED_CELL, theC->getEntity());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_CELL));

  // EntityDimension
  if (create)
    theC->setEntityDimension(3);

  // NumberOfNodes
  int nbNodes = 20;

  if (create) {
    theC->setNumberOfNodes(nbNodes);
  }

  // GeometricTypes
  MED_EN::medGeometryElement aCellTypes[2] = {MED_EN::MED_PYRA5, MED_EN::MED_HEXA8};

  // this variable is needed in check mode (!create)
  // because of bug with getGlobalNumberingIndex() method (see below)

  if (create) {
    theC->setGeometricTypes(aCellTypes, MED_EN::MED_CELL);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_FACE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setGeometricTypes(aCellTypes, MED_EN::MED_EDGE), MEDEXCEPTION);
  }

  // Nodal Connectivity for standard types
  int countCells[3] = {1, 3, 4}; // 2 PYRA5 and 1 HEXA8
  int nodesCells_PYRA5[10] = {2,3,4,5,1, 6,7,8,9,10};
  int nodesCells_HEXA8[8] = {2,3,4,5, 6,7,8,9};

  if (create) {
    theC->setCount(countCells, MED_EN::MED_CELL);
    theC->setNodal(nodesCells_PYRA5, MED_EN::MED_CELL, MED_EN::MED_PYRA5);
    theC->setNodal(nodesCells_HEXA8, MED_EN::MED_CELL, MED_EN::MED_HEXA8);

    // Invalid cases
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_NODE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_EDGE), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW(theC->setCount(countCells, MED_EN::MED_FACE), MEDEXCEPTION);

    CPPUNIT_ASSERT_THROW(theC->setNodal(nodesCells_PYRA5, MED_EN::MED_FACE, MED_EN::MED_PYRA5), MEDEXCEPTION);
  }

  // 2 POLYHEDRA
  const int nbPolyhedron = 2;
  const int nbPolyNodes = 52 + 14 - 2; // = 64

  int aPolyhedronIndex[nbPolyhedron + 1] = {1,33, 65};

  int aPolyhedronNodalConnectivity[nbPolyNodes] = {
    11,15,19,20,17,13,-1, 11,13,14,-1,  14,13,17,18,-1,  18,17,20,-1,  11,14,15,-1,  15,14,18,19,-1,  19,18,20,
    11,13,17,20,19,15,-1, 11,12,13,-1,  13,12,16,17,-1,  17,16,20,-1,  11,15,12,-1,  12,15,19,16,-1,  16,19,20};

  if (create) {
    theC->setNodal(aPolyhedronNodalConnectivity,
                   MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA, aPolyhedronIndex);
  }
  else {
    // CELLS(3D): theC
    {
      // Polyhedron-specific methods
      {
        // invalid polyhedron Id
//#ifdef ENABLE_FAULTS
        int lenPolyh3nodes;
        int nbFaces3;
        int *nbNodes3;
        // (BUG) Segmentation fault instead of MEDEXCEPTION
        CPPUNIT_ASSERT_THROW(theC->getNodesOfPolyhedron(1, lenPolyh3nodes), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesOfPolyhedron(3+3, lenPolyh3nodes), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesPerFaceOfPolyhedron
                             (/*polyhedronId*/1, nbFaces3, nbNodes3), MEDEXCEPTION);
        CPPUNIT_ASSERT_THROW(theC->getNodesPerFaceOfPolyhedron
                             (/*polyhedronId*/3+3, nbFaces3, nbNodes3), MEDEXCEPTION);
//#endif
      } // Polyhedron-specific methods
    } // CELLS: theC
  }

  if (create) {
    // force _constituent computation
    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg, 2, theC->getNumberOfTypes(MED_EN::MED_FACE));
  }
}

int main (int argc, char** argv)
{
  CONNECTIVITY * c1 = new CONNECTIVITY(/*numberOfTypes*/2, MED_EN::MED_CELL);
  createOrCheck(c1, "Creation", /*create*/true);
  createOrCheck(c1, "Check just created", /*create*/false);
}
