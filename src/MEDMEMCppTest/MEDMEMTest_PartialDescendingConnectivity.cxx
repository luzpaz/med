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

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Field.hxx"

#include <numeric>

void MEDMEMTest::testPartialDescendingConnectivity()
{
  double coords[24]= {0., 0., 3., 0., 0., 0., 0., 3., 3., 0., 3., 0., 3., 0., 3., 3., 0., 0., 3., 3., 3., 3., 3., 0.};

  const int connNodalCellClassical[8]={2, 6, 5, 1, 4, 8, 7, 3};

  const int connNodalFaceClassical[24]={3, 1, 5, 7, 5, 6, 8, 7, 2, 1, 3, 4, 2, 4, 8, 6, 8, 4, 3, 7, 2, 6, 5, 1};

  const int connNodalEdgeClassical[24]={4,8,8,7,2,1,1,3,5,7,2,6,1,5,6,8,6,5,4,3,3,7,2,4};
  //

  MESHING* myMesh = new MESHING;
  myMesh->setName( "TESTMESH" );
  const int nNodes=8;
  myMesh->setCoordinates(3, nNodes, coords, "CARTESIAN", MED_EN::MED_FULL_INTERLACE);
  string coordname[3] = { "x", "y", "z" };
  myMesh->setCoordinatesNames(coordname);
  string coordunit[3] = { "m", "m", "m" };
  myMesh->setCoordinatesUnits(coordunit);
  //Cell connectivity info for classical elts
  const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_HEXA8};
  const int nbOfCellElts[1]={1};
  myMesh->setNumberOfTypes(1,MED_EN::MED_CELL);
  myMesh->setTypes(classicalTypesCell,MED_EN::MED_CELL);
  myMesh->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
  //Face connectivity info for classical elts
  const MED_EN::medGeometryElement classicalTypesFace[1]={MED_EN::MED_QUAD4};
  const int nbOfFaceElts[1]={6};
  myMesh->setNumberOfTypes(1,MED_EN::MED_FACE);
  myMesh->setTypes(classicalTypesFace,MED_EN::MED_FACE);
  myMesh->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
  //
  const MED_EN::medGeometryElement classicalTypesEdge[1]={MED_EN::MED_SEG2};
  const int nbOfEdgeElts[1]={12};
  myMesh->setNumberOfTypes(1,MED_EN::MED_EDGE);
  myMesh->setTypes(classicalTypesEdge,MED_EN::MED_EDGE);
  myMesh->setNumberOfElements(nbOfEdgeElts,MED_EN::MED_EDGE);
  //All cell conn
  myMesh->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_HEXA8,connNodalCellClassical);
  //All face conn
  myMesh->setConnectivity(MED_EN::MED_FACE,MED_EN::MED_QUAD4,connNodalFaceClassical);
  //All edge conn
  myMesh->setConnectivity(MED_EN::MED_EDGE,MED_EN::MED_SEG2,connNodalEdgeClassical);
  //
  /*const int *ConnNodal = */myMesh->getConnectivity(MED_NODAL,MED_CELL,MED_ALL_ELEMENTS);
  /*const int *ConnNodalIndex = */myMesh->getConnectivityIndex(MED_NODAL,MED_CELL);

  const int *Conn      = myMesh->getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
  const int *ConnIndex = myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  int lgth=myMesh->getConnectivityLength(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
  CPPUNIT_ASSERT_EQUAL(6,lgth);
  CPPUNIT_ASSERT_EQUAL(1,ConnIndex[0]);
  CPPUNIT_ASSERT_EQUAL(7,ConnIndex[1]);
  int ConnTmp[6];
  copy(Conn,Conn+6,ConnTmp);
  for(int i=0;i<6;i++)
    ConnTmp[i]=abs(ConnTmp[i]);
  int sum=accumulate(ConnTmp,ConnTmp+6,0);
  CPPUNIT_ASSERT_EQUAL(21,sum);//1+2+3+4+5+6
  CPPUNIT_ASSERT_EQUAL(6, *max_element(ConnTmp,ConnTmp+6));
  CPPUNIT_ASSERT_EQUAL(1, *min_element(ConnTmp,ConnTmp+6));
  //
  const int *ConnFaceNodal = myMesh->getConnectivity(MED_NODAL,MED_FACE,MED_ALL_ELEMENTS);
  const int *ConnFaceNodalIndex = myMesh->getConnectivityIndex(MED_NODAL,MED_FACE);
  const int expected3[7]={1,5,9,13,17,21,25};
  int i;
  for(i=0;i<24;i++)
    CPPUNIT_ASSERT_EQUAL(connNodalFaceClassical[i],ConnFaceNodal[i]);
  for(i=0;i<7;i++)
    CPPUNIT_ASSERT_EQUAL(expected3[i],ConnFaceNodalIndex[i]);
  //
  myMesh->removeReference();
}
