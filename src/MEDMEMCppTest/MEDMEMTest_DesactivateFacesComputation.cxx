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

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;
using namespace MEDMEM;

static void addMedFacesGroup2( MESHING& meshing, int nFaces, const int *groupValue,
                        string groupName, const MED_EN::medGeometryElement *mytypes,  const int *index, const int *myNumberOfElements, int nbOfGeomTypes)
  {
    GROUP *faces=new GROUP;
    faces->setName(groupName) ;
    faces->setMesh(&meshing) ;
    faces->setEntity(MED_EN::MED_FACE) ;
    faces->setNumberOfGeometricType(nbOfGeomTypes) ;
    faces->setGeometricType(mytypes);
    faces->setNumberOfElements(myNumberOfElements) ;
    faces->setNumber(index, groupValue) ;
    meshing.addGroup(*faces) ;
    faces->removeReference();
  }

void MEDMEMTest::testDesactivateFacesComputation()
{
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

  int bottom[2]={1,7};
  MED_EN::medGeometryElement bottomTypes[1]={MED_EN::MED_QUAD4};
  int bottomIndex[2]={1,3};
  int bottomNbOfElts[1]={2};
  int top[4]={8,9,10,11};
  MED_EN::medGeometryElement topTypes[1]={MED_EN::MED_QUAD4};
  int topIndex[3]={1,5};
  int topNbOfElts[1]={4};
  int side[5]={ 3, 6, 10, 11, 13};
  MED_EN::medGeometryElement sideTypes[1]={MED_EN::MED_QUAD4};
  int sideIndex[2]={1,6};
  int sideNbOfElts[1]={5};
  //
  string tmp_dir=getTmpDirectory();
  string tmpfile=tmp_dir+"/toto3434535.med";
  string tmpGibiFile=tmp_dir+"/TET4.sauv";
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(tmpfile);
  aRemover.Register(tmpGibiFile);
  //

  MESHING* meshing = new MESHING;
  meshing->setName( "TESTMESH" );
  const int nNodes=36;
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
  //Face connectivity info for classical elts
  const MED_EN::medGeometryElement classicalTypesFace[1]={MED_EN::MED_QUAD4};
  const int nbOfFaceElts[1]={14};
  meshing->setNumberOfTypes(1,MED_EN::MED_FACE);
  meshing->setTypes(classicalTypesFace,MED_EN::MED_FACE);
  meshing->setNumberOfElements(nbOfFaceElts,MED_EN::MED_FACE);
  //All cell conn
  meshing->setConnectivity(MED_EN::MED_CELL,MED_EN::MED_HEXA8,connNodalCellClassical);
  //All face conn
  meshing->setConnectivity(MED_EN::MED_FACE,MED_EN::MED_QUAD4,connNodalFaceClassical);
  int nbOfTypes=meshing->getNumberOfTypes(MED_EN::MED_EDGE);
  addMedFacesGroup2( *meshing, 2,  bottom, "Bottom",bottomTypes,bottomIndex,bottomNbOfElts,1) ;
  addMedFacesGroup2( *meshing, 4,  top,    "TopFace",topTypes,topIndex,topNbOfElts,1) ;
  addMedFacesGroup2( *meshing, 5, side,   "SideFace",sideTypes,sideIndex,sideNbOfElts,1) ;
  //
  int id=meshing->addDriver(MED_DRIVER,tmpfile,meshing->getName());
  meshing->write(id);
  //
  MESH *mesh=new MESH;
  mesh->setName(meshing->getName());
  MEDMEM::MED_MESH_RDONLY_DRIVER *driver=new MEDMEM::MED_MESH_RDONLY_DRIVER(tmpfile,mesh);
  driver->desactivateFacesComputation();
  id=mesh->addDriver(*driver);
  mesh->read(id);
  CPPUNIT_ASSERT_NO_THROW(nbOfTypes=mesh->getNumberOfTypes(MED_EN::MED_EDGE));
  CPPUNIT_ASSERT_EQUAL(0,nbOfTypes);
  MEDMEM::GIBI_MESH_WRONLY_DRIVER *gibidriver=new MEDMEM::GIBI_MESH_WRONLY_DRIVER(tmpGibiFile,mesh );
  id=mesh->addDriver(*gibidriver);
  mesh->write(id);
#ifdef WIN32
  CPPUNIT_ASSERT( GetFileAttributes(tmpGibiFile.c_str()) != INVALID_FILE_ATTRIBUTES );
#else
  CPPUNIT_ASSERT( access(tmpGibiFile.c_str(), F_OK) == 0 );
#endif

  delete gibidriver;
  delete driver;
  //
  meshing->removeReference();
  mesh->removeReference();
}
