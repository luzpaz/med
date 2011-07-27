//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
#ifdef WNT
#include <io.h>
#define access _access
#define W_OK 02
#endif

double coords[108]=
  {0, 0.21504, 0.21504, 0, 0.43008, 
   0.43008, 0.21504, 0, 0.43008, 0, 
   0.21504, 0.21504, 0, 0.43008, 
   0.43008, 0.21504, 0, 0.43008, 0, 
   0.21504, 0.21504, 0, 0.43008, 
   0.43008, 0.21504, 0, 0.43008, 0, 
   0.21504, 0.21504, 0, 0.43008, 
   0.43008, 0.21504, 0, 0.43008,
   //coordY
   0, 0, 0.21504, 0.21504, 0, 0.21504, 
   0.43008, 0.43008, 0.43008, 0, 0, 
   0.21504, 0.21504, 0, 0.21504, 
   0.43008, 0.43008, 0.43008, 0, 0, 
   0.21504, 0.21504, 0, 0.21504, 
   0.43008, 0.43008, 0.43008, 0, 0, 
   0.21504, 0.21504, 0, 0.21504, 
   0.43008, 0.43008, 0.43008,
   //coordZ
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0.15677, 0.15677, 
   0.15677, 0.15677, 0.15677, 
   0.15677, 0.15677, 0.15677, 
   0.15677, 0.1934, 0.1934, 
   0.1934, 0.1934, 0.1934, 
   0.1934, 0.1934, 0.1934, 
   0.1934, 0.3485, 0.3485, 
   0.3485, 0.3485, 0.3485, 
   0.3485, 0.3485, 0.3485, 
   0.3485};

int connNodalCellClassical[48]=
  { 1, 2, 3, 4, 10, 11, 12, 13, 
    4, 3, 7, 8, 13, 12, 16, 17, 
    10, 11, 12, 13, 19, 20, 21, 22, 
    13, 12, 16, 17, 22, 21, 25, 26, 
    19, 20, 21, 22, 28, 29, 30, 31,
    22, 21, 25, 26, 31, 30, 34, 35 };

int polyHedraInd[7]={1, 7, 13, 19, 25, 31, 37};
int polyHedraFacesInd[37]={ 
  1, 5, 9, 13, 17, 21, 
  25, 29, 33, 37, 41, 45, 
  49, 53, 57, 61, 65, 69, 
  73, 77, 81, 85, 89, 93, 
  97, 101, 105, 109, 113, 117, 
  121, 125, 129, 133, 137, 141, 
  145 };

int polyHedraConn[144]={ 
  2, 3, 6, 5, 11, 14, 15, 12, 2, 5, 14, 11, 5, 6, 15, 14, 6, 3, 12, 15, 3, 2, 11, 12, 
  3, 7, 9, 6, 12, 15, 18, 16, 3, 6, 15, 12, 6, 9, 18, 15, 9, 7, 16, 18, 7, 3, 12, 16, 
  11, 12, 15, 14, 20, 23, 24, 21, 11, 14, 23, 20, 14, 15, 24, 23, 15, 12, 21, 24, 12, 11, 20, 21, 
  12, 16, 18, 15, 21, 24, 27, 25, 12, 15, 24, 21, 15, 18, 27, 24, 18, 16, 25, 27, 16, 12, 21, 25, 
  20, 21, 24, 23, 29, 32, 33, 30, 20, 23, 32, 29, 23, 24, 33, 32, 24, 21, 30, 33, 21, 20, 29, 30, 
  21, 25, 27, 24, 30, 33, 36, 34, 21, 24, 33, 30, 24, 27, 36, 33, 27, 25, 34, 36, 25, 21, 30, 34 };

int connNodalFaceClassical[116]=
  { 1, 2, 3, 4, 
    10, 13, 12, 11, 
    1, 2, 11, 10, 
    2, 3, 12, 11, 
    3, 4, 13, 12, 
    4, 1, 10, 13, 
    4, 3, 7, 8, 
    13, 17, 16, 12, 
    3, 7, 16, 12, 
    7, 8, 17, 16, 
    8, 4, 13, 17, 
    19, 22, 21, 20, 
    10, 11, 20, 19,
    11, 12, 21, 20, 
    12, 13, 22, 21, 
    13, 10, 19, 22, 
    22, 26, 25, 21, 
    12, 16, 25, 21, 
    16, 17, 26, 25, 
    17, 13, 22, 26, 
    28, 31, 30, 29, 
    19, 20, 29, 28, 
    20, 21, 30, 29, 
    21, 22, 31, 30, 
    22, 19, 28, 31, 
    31, 35, 34, 30, 
    21, 25, 34, 30, 
    25, 26, 35, 34, 
    26, 22, 31, 35};

int cpolygonsIndex[24]=
  { 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 
    73, 77, 81, 85, 89, 93};

int cpolygonsValue[92]=
  {2, 5, 6, 3, 
   11, 12, 15, 14, 
   2, 5, 14, 11, 
   5, 6, 15, 14, 
   6, 3, 12, 15, 
   3, 6, 9, 7, 
   12, 16, 18, 15, 
   6, 9, 18, 15, 
   9, 7, 16, 18, 
   20, 21, 24, 23, 
   11, 14, 23, 20, 
   14, 15, 24, 23, 
   15, 12, 21, 24, 
   21, 25, 27, 24, 
   15, 18, 27, 24, 
   18, 16, 25, 27, 
   29, 30, 33, 32, 
   20, 23, 32, 29, 
   23, 24, 33, 32, 
   24, 21, 30, 33, 
   30, 34, 36, 33, 
   24, 27, 36, 33, 
   27, 25, 34, 36};

int bottom[4]={1,7,30,35}; MED_EN::medGeometryElement bottomTypes[2]={MED_EN::MED_QUAD4, MED_EN::MED_POLYGON }; int bottomIndex[3]={1,3,5}; int bottomNbOfElts[2]={2,2};
int top[4]={21,26,46,50}; MED_EN::medGeometryElement topTypes[2]={MED_EN::MED_QUAD4, MED_EN::MED_POLYGON }; int topIndex[3]={1,3,5}; int topNbOfElts[2]={2,2};
int side[24]={ 3, 6, 10, 11, 13, 16, 19, 20, 22, 25, 28, 29, 32, 33, 37, 38, 40, 41, 44, 45, 
               47, 48, 51, 52}; MED_EN::medGeometryElement sideTypes[2]={MED_EN::MED_QUAD4, MED_EN::MED_POLYGON }; int sideIndex[3]={1,13,25}; int sideNbOfElts[2]={12,12};

using namespace MEDMEM;

void addMedFacesGroup( MESHING& meshing, int nFaces, const int *groupValue,
                       string groupName, const MED_EN::medGeometryElement *mytypes,  const int *index, const int *myNumberOfElements, int nbOfGeomTypes)
  {
    GROUP faces ;
    faces.setName(groupName) ;
    faces.setMesh(&meshing) ;
    faces.setEntity(MED_EN::MED_FACE) ;
    faces.setNumberOfGeometricType(nbOfGeomTypes) ;
    faces.setGeometricType(mytypes);
    faces.setNumberOfElements(myNumberOfElements) ;
    faces.setNumber(index, groupValue) ;
    meshing.addGroup(faces) ;
  }

int main()
{
  MESHING* meshing = new MESHING();
  meshing->setName( "TESTMESH" );
  meshing->setSpaceDimension(3);
  const int nNodes=36;
  meshing->setNumberOfNodes(nNodes);
  meshing->setCoordinates(3, nNodes, coords, "CARTESIAN",
                          MED_EN::MED_NO_INTERLACE);
  string coordname[3] = { "x", "y", "z" };
  meshing->setCoordinatesNames(coordname);
  string coordunit[3] = { "m", "m", "m" };
  meshing->setCoordinatesUnits(coordunit);
  //Cell connectivity info for classical elts
  const MED_EN::medGeometryElement classicalTypesCell[1]={MED_EN::MED_HEXA8};
  const int nbOfCellElts[1]={6};
  meshing->setNumberOfTypes(1,MED_EN::MED_CELL);
  meshing->setTypes(classicalTypesCell,MED_EN::MED_CELL);
  meshing->setNumberOfElements(nbOfCellElts,MED_EN::MED_CELL);
  meshing->setMeshDimension(3);
  //Face connectivity info for classical elts
  const MED_EN::medGeometryElement classicalTypesFace[1]={MED_EN::MED_QUAD4};
  const int nbOfFaceElts[1]={29};
  meshing->setNumberOfTypes(1,MED_EN::MED_FACE);
  meshing->setTypes(classicalTypesFace,MED_EN::MED_FACE);
  meshing->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
  //All cell conn
  meshing->setConnectivity(connNodalCellClassical,MED_EN::MED_CELL,MED_EN::MED_HEXA8);
  meshing->setPolyhedraConnectivity(polyHedraInd,polyHedraFacesInd,polyHedraConn,6,MED_EN::MED_CELL);
  //All face conn
  meshing->setConnectivity(connNodalFaceClassical,MED_EN::MED_FACE,MED_EN::MED_QUAD4);
  meshing->setPolygonsConnectivity(cpolygonsIndex,cpolygonsValue,23,MED_EN::MED_FACE);
  //Adding some groups on faces
  addMedFacesGroup( *meshing, 4,  bottom, "BottomFaces",bottomTypes,bottomIndex,bottomNbOfElts,2) ;
  addMedFacesGroup( *meshing, 4,  top,    "TopFaces",topTypes,topIndex,topNbOfElts,2) ;
  addMedFacesGroup( *meshing, 24, side,   "SideFaces",sideTypes,sideIndex,sideNbOfElts,2) ;
  //writing...
  string medfile = "/tmp";
  if ( getenv("TMP") && access(getenv("TMP"),W_OK)==0 )
    medfile=getenv("TMP");
  else if (getenv("TMPDIR") && access(getenv("TMPDIR"),W_OK)==0 )
    medfile=getenv("TMPDIR");
  medfile += "/totoFlica_V22.med";
  
  int id=meshing->addDriver(MED_DRIVER,medfile.c_str());
  meshing->write(id);
  delete meshing;

  if ( getenv("srcdir") )
    remove(medfile.c_str());

  return 0;
}
