// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static void addMedFacesGroup (MESHING& meshing, int nFaces, int *groupValue, string groupName)
{
  GROUP *faces=new GROUP;
  int myNumberOfElements[1];
  medGeometryElement mytypes[1];
  int index[2];
  faces->setName(groupName);
  faces->setMesh(&meshing);
  faces->setEntity(MED_EN::MED_FACE);
  faces->setNumberOfGeometricType(1);
  mytypes[0] = MED_EN::MED_POLYGON;
  faces->setGeometricType(mytypes);
  myNumberOfElements[0] = nFaces;
  faces->setNumberOfElements(myNumberOfElements);
  index[0] = 1;
  index[1] = nFaces;
  faces->setNumber(index, groupValue);
  meshing.addGroup(*faces);
  faces->removeReference();
}

int main()
{
  MESHING *pmesh = new MESHING;
  pmesh->setName( "C2MESH" );
  pmesh->setNumberOfTypes(0, MED_EN::MED_CELL);
  const int nb_hexagone = 7;
  const int nbmailles_axiales = 2;
  const int nb_polyH = nb_hexagone*(nbmailles_axiales-1);
  const int nb_polyG = nb_hexagone*nbmailles_axiales;
  const int nNodes = 6 * nb_hexagone * nbmailles_axiales;

 double coord[] = {
    44.401823743922364, 0                 , 0, 55.698259110886582, 6.5220000000000002, 0,
    55.698259110886582, 19.566000000000003, 0, 44.401823743922364, 26.088000000000001, 0,
    33.105388376958146, 19.566000000000003, 0, 33.105388376958146, 6.5220000000000002, 0,
    66.9946944778508  , 0                 , 0, 78.291129844815018, 6.5220000000000002, 0,
    78.291129844815018, 19.566000000000003, 0, 66.9946944778508  , 26.088000000000001, 0,
    55.698259110886582, 19.566000000000003, 0, 55.698259110886582, 6.5220000000000002, 0,
    33.105388376958146, 19.565999999999999, 0, 44.401823743922364, 26.088000000000001, 0,
    44.401823743922364, 39.131999999999998, 0, 33.105388376958146, 45.653999999999996, 0,
    21.808953009993928, 39.131999999999998, 0, 21.808953009993928, 26.088000000000001, 0,
    55.698259110886582, 19.565999999999999, 0, 66.9946944778508  , 26.088000000000001, 0,
    66.9946944778508  , 39.131999999999998, 0, 55.698259110886582, 45.653999999999996, 0,
    44.401823743922364, 39.131999999999998, 0, 44.401823743922364, 26.088000000000001, 0,
    78.291129844815018, 19.565999999999999, 0, 89.587565211779236, 26.088000000000001, 0,
    89.587565211779236, 39.131999999999998, 0, 78.291129844815018, 45.653999999999996, 0,
    66.9946944778508  , 39.131999999999998, 0, 66.9946944778508  , 26.088000000000001, 0,
    44.401823743922364, 39.132000000000005, 0, 55.698259110886582, 45.654000000000003, 0,
    55.698259110886582, 58.698            , 0, 44.401823743922364, 65.219999999999999, 0,
    33.105388376958146, 58.698            , 0, 33.105388376958146, 45.654000000000003, 0,
    66.9946944778508  , 39.132000000000005, 0, 78.291129844815018, 45.654000000000003, 0,
    78.291129844815018, 58.698            , 0, 66.9946944778508  , 65.219999999999999, 0,
    55.698259110886582, 58.698            , 0, 55.698259110886582, 45.654000000000003, 0,
    44.401823743922364, 0                 , 1, 55.698259110886582, 6.5220000000000002, 1,
    55.698259110886582, 19.566000000000003, 1, 44.401823743922364, 26.088000000000001, 1,
    33.105388376958146, 19.566000000000003, 1, 33.105388376958146, 6.5220000000000002, 1,
    66.9946944778508  , 0                 , 1, 78.291129844815018, 6.5220000000000002, 1,
    78.291129844815018, 19.566000000000003, 1, 66.9946944778508  , 26.088000000000001, 1,
    55.698259110886582, 19.566000000000003, 1, 55.698259110886582, 6.5220000000000002, 1,
    33.105388376958146, 19.565999999999999, 1, 44.401823743922364, 26.088000000000001, 1,
    44.401823743922364, 39.131999999999998, 1, 33.105388376958146, 45.653999999999996, 1,
    21.808953009993928, 39.131999999999998, 1, 21.808953009993928, 26.088000000000001, 1,
    55.698259110886582, 19.565999999999999, 1, 66.9946944778508  , 26.088000000000001, 1,
    66.9946944778508  , 39.131999999999998, 1, 55.698259110886582, 45.653999999999996, 1,
    44.401823743922364, 39.131999999999998, 1, 44.401823743922364, 26.088000000000001, 1,
    78.291129844815018, 19.565999999999999, 1, 89.587565211779236, 26.088000000000001, 1,
    89.587565211779236, 39.131999999999998, 1, 78.291129844815018, 45.653999999999996, 1,
    66.9946944778508  , 39.131999999999998, 1, 66.9946944778508  , 26.088000000000001, 1,
    44.401823743922364, 39.132000000000005, 1, 55.698259110886582, 45.654000000000003, 1,
    55.698259110886582, 58.698            , 1, 44.401823743922364, 65.219999999999999, 1,
    33.105388376958146, 58.698            , 1, 33.105388376958146, 45.654000000000003, 1,
    66.9946944778508  , 39.132000000000005, 1, 78.291129844815018, 45.654000000000003, 1,
    78.291129844815018, 58.698            , 1, 66.9946944778508  , 65.219999999999999, 1,
    55.698259110886582, 58.698            , 1, 55.698259110886582, 45.654000000000003, 1};

  pmesh->setCoordinates(3, nNodes, coord, "CARTESIAN", MED_EN::MED_FULL_INTERLACE);

  string coordname[3] = { "x", "y", "z" };
  pmesh->setCoordinatesNames(coordname);

  // Set coordinates units
  string coordunit[3] = { "cm", "cm", "cm" };
  pmesh->setCoordinatesUnits(coordunit);

  const medGeometryElement types[] = { MED_EN::MED_POLYGON, MED_EN::MED_POLYHEDRA };
  
  pmesh->setNumberOfTypes(1, MED_EN::MED_CELL);
  pmesh->setTypes( & types[1], MED_EN::MED_CELL );
  pmesh->setNumberOfElements( & nb_polyH, MED_CELL );

  pmesh->setNumberOfTypes(1, MED_EN::MED_FACE);
  pmesh->setTypes( & types[0], MED_EN::MED_FACE );
  pmesh->setNumberOfElements( & nb_polyG, MED_FACE );

  int polyHInd[nb_polyH+1] = {1, 44, 87, 130, 173, 216, 259, 302 };
  int polyHConn[301] = {
    1,2,3,4,5,6,      -1, 43,44,45,46,47,48,-1, 1,2,44,43,  -1, 2,3,45,44,  -1,
    3,4,46,45,        -1, 4,5,47,46,        -1, 5,6,48,47,  -1, 6,1,43,48,       // #1
    7,8,9,10,3,2,     -1, 49,50,51,52,45,44,-1, 7,8,50,49,  -1, 8,9,51,50,  -1,
    9,10,52,51,       -1, 10,3,45,52,       -1, 3,2,44,45,  -1, 2,7,49,44,       // #2
    5,4,15,16,17,18,  -1, 47,46,57,58,59,60,-1, 5,4,46,47,  -1, 4,15,57,46, -1,
    15,16,58,57,      -1, 16,17,59,58,      -1, 17,18,60,59,-1, 18,5,47,60,      // #3
    3,10,21,22,15,4,  -1, 45,52,63,64,57,46,-1, 3,10,52,45, -1, 10,21,63,52,-1,
    21,22,64,63,      -1, 22,15,57,64,      -1, 15,4,46,57, -1, 4,3,45,46,       // #4
    9,26,27,28,21,10, -1, 51,68,69,70,63,52,-1, 9,26,68,51, -1, 26,27,69,68,-1,
    27,28,70,69,      -1, 28,21,63,70,      -1, 21,10,52,63,-1, 10,9,51,52,      // #5
    15,22,33,34,35,16,-1, 57,64,75,76,77,58,-1, 15,22,64,57,-1, 22,33,75,64,-1,
    33,34,76,75,      -1, 34,35,77,76,      -1, 35,16,58,77,-1, 16,15,57,58,     // #6
    21,28,39,40,33,22,-1, 63,70,81,82,75,64,-1, 21,28,70,63,-1, 28,39,81,70,-1,
    39,40,82,81,      -1, 40,33,75,82,      -1, 33,22,64,75,-1, 22,21,63,64};    // #7

  pmesh->setConnectivity(MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA, polyHConn, polyHInd);

  int cpolygonsindex[nb_polyG+1] = {1, 7, 13, 19, 25, 31, 37, 43, 49, 55, 61, 67, 73, 79, 85};
  int connec[] = {
    1 , 2 , 3 , 4 , 5 , 6 ,
    7 , 8 , 9 , 10, 3 , 2 ,
    5 , 4 , 15, 16, 17, 18,
    3 , 10, 21, 22, 15, 4 ,
    9 , 26, 27, 28, 21, 10,
    15, 22, 33, 34, 35, 16,
    21, 28, 39, 40, 33, 22,
    43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 45, 44,
    47, 46, 57, 58, 59, 60,
    45, 52, 63, 64, 57, 46,
    51, 68, 69, 70, 63, 52,
    57, 64, 75, 76, 77, 58,
    63, 70, 81, 82, 75, 64};

  pmesh->setConnectivity(MED_EN::MED_FACE, MED_EN::MED_POLYGON,connec, cpolygonsindex);

  int bottom[] = {1, 2, 3, 4, 5, 6, 7};
  addMedFacesGroup(*pmesh, nb_hexagone, bottom, "BottomFaces");

  int id = pmesh->addDriver(MED_DRIVER, "cronoshexa.med", pmesh->getName());
  pmesh->write(id);
  pmesh->removeReference();

  if ( getenv("srcdir") )
    /* we are in 'make check' */
    remove( "cronoshexa.med" );

  return 0;
}
