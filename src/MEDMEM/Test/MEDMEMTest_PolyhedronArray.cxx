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
 *   (yetno) POLYHEDRONARRAY();
 *   (yetno) POLYHEDRONARRAY(MED_EN::med_int numberofpolyhedron,
 *                           MED_EN::med_int numberoffaces, MED_EN::med_int numberofnodes);
 *   (yetno) POLYHEDRONARRAY(const POLYHEDRONARRAY& m);
 *   (yetno) inline MED_EN::med_int getNumberOfPolyhedron() const;
 *   (yetno) inline MED_EN::med_int getNumberOfFaces() const;
 *   (yetno) inline MED_EN::med_int getNumberOfNodes() const;
 *   (yetno) inline const MED_EN::med_int* getPolyhedronIndex() const;
 *   (yetno) inline const MED_EN::med_int* getFacesIndex() const;
 *   (yetno) inline const MED_EN::med_int* getNodes() const;
 *   (yetno) inline void setPolyhedronIndex(const MED_EN::med_int* polyhedronindex);
 *   (yetno) inline void setFacesIndex(const MED_EN::med_int* facesindex);
 *   (yetno) inline void setNodes(const MED_EN::med_int* nodes);
 *  }
 */
void MEDMEMTest::testPolyhedronArray()
{
  const med_int nbPolyhedrons = 2;
  const med_int nbFaces = 11;
  const med_int nbNodes = 40;
  POLYHEDRONARRAY myPArray(nbPolyhedrons,nbFaces,nbNodes);
  med_int polyhedronindex[nbPolyhedrons] = {1,7};
  med_int facesindex[nbFaces] = {1,5,9,13,17,21,25,28,31,34,37};
  med_int nodes[nbNodes] = {1,2,6,5,2,3,7,6,4,3,7,8,1,4,8,5,1,2,3,4,5,6,7,8,5,6,9,6,7,9,7,8,9,8,5,9,5,6,7,8};

  CPPUNIT_ASSERT_NO_THROW(myPArray.setPolyhedronIndex(polyhedronindex));
  CPPUNIT_ASSERT_NO_THROW(myPArray.setFacesIndex(facesindex));
  CPPUNIT_ASSERT_NO_THROW(myPArray.setNodes(nodes));

  POLYHEDRONARRAY myPArray2(myPArray);

  const med_int* __polyhedronindex;
  __polyhedronindex = myPArray2.getPolyhedronIndex();
  const med_int* __facesindex = myPArray2.getFacesIndex();
  const med_int* __nodes = myPArray2.getNodes();

  cout << "__polyhedronindex =" << endl;
  for (int i=0; i<2; i++)
    cout << __polyhedronindex[i] << endl;
  cout << "__facesindex =" << endl;
  for (int i=0; i<11; i++)
    cout << __facesindex[i] << endl;
  cout << "__nodes =" << endl;
  for (int i=0; i<40; i++)
    cout << __nodes[i] << endl;
}
