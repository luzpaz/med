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

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"

#include <cppunit/Message.h>
#include <cppunit/TestAssert.h>

using namespace std;
using namespace MEDMEM;

void MEDMEMTest::testGetVolumeAbs()
{
  double coords[63]=
    {
      0., 0., 0. 
      ,2., 0., 0. 
      ,2., 1., 0. 
      ,1., 2., 0. 
      ,0., 1., 0. 
      ,2., 2., 0. 
      , 0., 2., 0. 
      , 0., 0., 1. 
      , 2., 0., 1. 
      , 2., 1., 1. 
      , 1., 2., 1. 
      , 0., 1., 1. 
      , 2., 2., 1. 
      , 0., 2., 1. 
      , 0., 0., 2. 
      , 2., 0., 2. 
      , 2., 1., 2. 
      , 1., 2., 2. 
      , 0., 1., 2. 
      , 2., 2., 2. 
      , 0., 2., 2. 
    };
  int connNodalCellClassical[24]=
    {
      10, 11, 13, 17, 18, 20,
      4,  5,  7, 11, 12, 14,
      11, 12, 14, 18, 19, 21,
      3,  4,  6, 10, 11, 13 
    };
  int polyHedraConn[60+12]=
    {
      8, 9, 10, 11, 12,   -1,
      15, 16, 17, 18, 19, -1,
      8, 9, 16, 15,       -1,
      9,  10, 17, 16,     -1,
      10, 11, 18, 17,     -1,
      11, 12, 19, 18,     -1,
      12, 8, 15, 19,
      //cell2
      1, 2, 3, 4, 5,      -1,
      8, 9, 10, 11, 12,   -1,
      1, 2, 9, 8,         -1,
      2, 3, 10, 9,        -1,
      3, 4, 11, 10,       -1,
      4, 5, 12, 11,       -1,
      5, 1, 8, 12 
    };
  int polyHedraInd[3]=
    {
      1,37,73
    };
  MESHING* meshing = new MESHING;
  meshing->setName( "TESTMESH" );
  const int nNodes=21;
  meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",
                          MED_EN::MED_FULL_INTERLACE);
  string coordname[3] = 
    {
      "x", "y", "z" 
    };
  meshing->setCoordinatesNames(coordname);
  string coordunit[3] = 
    {
      "m", "m", "m" 
    };
  meshing->setCoordinatesUnits(coordunit);
  //Cell connectivity info
  const MED_EN::medGeometryElement classicalTypesCell[2]=
    {
      MED_EN::MED_PENTA6,MED_POLYHEDRA
    };
  const int nbOfCellElts[2]=
    {
      4,2
    };
  meshing->setNumberOfTypes(2,MED_EN::MED_CELL);
  meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
  meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
  //All cell conn
  meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_PENTA6,connNodalCellClassical);
  meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA, polyHedraConn,polyHedraInd);
  //
  const SUPPORT *sup=meshing->getSupportOnAll(MED_CELL);
  FIELD<double> *volumes=meshing->getVolume(sup);
  const double *vals=volumes->getValue();
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[0],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[1],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[2],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[3],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.,vals[4],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(3.,vals[5],1e-13);
  volumes->removeReference();
  volumes=meshing->getVolume(sup,false);
  vals=volumes->getValue();
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[0],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[1],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[2],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5,vals[3],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2.,vals[4],1e-13);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(2.,vals[5],1e-13);
  //
  volumes->removeReference();
  meshing->removeReference();
}
