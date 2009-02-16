//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "MEDMEMTest.hxx"
#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

#include "MEDMEM_PolyhedronArray.hxx"
#include "MEDMEM_define.hxx"

#include <sstream>
#include <cmath>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*!
 *  Check methods (12), defined in MEDMEM_PolyhedronArray.hxx:
 *  class POLYHEDRONARRAY {
 *   (+) POLYHEDRONARRAY();
 *   (+) POLYHEDRONARRAY(MED_EN::med_int numberofpolyhedron,
 *                       MED_EN::med_int numberoffaces, MED_EN::med_int numberofnodes);
 *   (+) POLYHEDRONARRAY(const POLYHEDRONARRAY& m);
 *
 *   (+) inline MED_EN::med_int getNumberOfPolyhedron() const;
 *   (+) inline MED_EN::med_int getNumberOfFaces() const;
 *   (+) inline MED_EN::med_int getNumberOfNodes() const;
 *
 *   (+) inline const MED_EN::med_int* getPolyhedronIndex() const;
 *   (+) inline const MED_EN::med_int* getFacesIndex() const;
 *   (+) inline const MED_EN::med_int* getNodes() const;
 *
 *   (+) inline void setPolyhedronIndex(const MED_EN::med_int* polyhedronindex);
 *   (+) inline void setFacesIndex(const MED_EN::med_int* facesindex);
 *   (+) inline void setNodes(const MED_EN::med_int* nodes);
 *  }
 */
void MEDMEMTest::testPolyhedronArray()
{
  const med_int nbPolyhedrons = 2;
  const med_int nbFaces = 11;
  const med_int nbNodes = 40;

  med_int polyhedronindex[nbPolyhedrons] = {1,7};
  med_int facesindex[nbFaces] = {1,5,9,13,17,21,25,28,31,34,37};
  med_int nodes[nbNodes] = {1,2,6,5,2,3,7,6,4,3,7,8,1,4,8,5,1,2,3,4,5,6,7,8,5,6,9,6,7,9,7,8,9,8,5,9,5,6,7,8};

  // construction with parameters
  POLYHEDRONARRAY myPArray (nbPolyhedrons, nbFaces, nbNodes);

  CPPUNIT_ASSERT_NO_THROW(myPArray.setPolyhedronIndex(polyhedronindex));
  CPPUNIT_ASSERT_NO_THROW(myPArray.setFacesIndex(facesindex));
  CPPUNIT_ASSERT_NO_THROW(myPArray.setNodes(nodes));

  // copy constructor
  POLYHEDRONARRAY myPArray2 (myPArray);

  const med_int* __polyhedronindex = myPArray2.getPolyhedronIndex();
  const med_int* __facesindex = myPArray2.getFacesIndex();
  const med_int* __nodes = myPArray2.getNodes();

  CPPUNIT_ASSERT_EQUAL(myPArray2.getNumberOfPolyhedron(), nbPolyhedrons);
  CPPUNIT_ASSERT_EQUAL(myPArray2.getNumberOfFaces(), nbFaces);
  CPPUNIT_ASSERT_EQUAL(myPArray2.getNumberOfNodes(), nbNodes);

  for (int i = 0; i < nbPolyhedrons; i++)
    CPPUNIT_ASSERT_EQUAL(__polyhedronindex[i], polyhedronindex[i]);

  for (int i = 0; i < nbFaces; i++)
   CPPUNIT_ASSERT_EQUAL(__facesindex[i], facesindex[i]);

  for (int i = 0; i < nbNodes; i++)
    CPPUNIT_ASSERT_EQUAL(__nodes[i], nodes[i]);

  // default constructor (has no sense, must be private)
  POLYHEDRONARRAY myPArray3;

  CPPUNIT_ASSERT_EQUAL(0, int(myPArray3.getNumberOfPolyhedron()));
  CPPUNIT_ASSERT_EQUAL(0, int(myPArray3.getNumberOfFaces()));
  CPPUNIT_ASSERT_EQUAL(0, int(myPArray3.getNumberOfNodes()));

  CPPUNIT_ASSERT(myPArray3.getPolyhedronIndex() == NULL);
  CPPUNIT_ASSERT(myPArray3.getFacesIndex() == NULL);
  CPPUNIT_ASSERT(myPArray3.getNodes() == NULL);
}
