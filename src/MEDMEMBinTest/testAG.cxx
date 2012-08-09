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

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_MedMeshDriver22.hxx"

using namespace MED_EN;
using namespace std;
using namespace MEDMEM;

void addMedFacesGroup( MESHING& meshing, int nFaces, int *groupValue,string groupName)
  {
    GROUP faces ;
    int myNumberOfElements[1];
    medGeometryElement mytypes[1];
    int index[2];
    int nbOfGeomTypes;
    faces.setName(groupName) ;
    faces.setMesh(&meshing) ;
    faces.setEntity(MED_EN::MED_FACE) ;
    faces.setNumberOfGeometricType(1) ;
    mytypes[0]=MED_EN::MED_POLYGON;
    faces.setGeometricType(mytypes);
    myNumberOfElements[0]=nFaces;
    faces.setNumberOfElements(myNumberOfElements) ;
    index[0]=1; index[1]=nFaces;
    faces.setNumber(index, groupValue) ;
    meshing.addGroup(faces) ;
  }

const int NB_OF_HEXA=1;
const int NB_OF_CELL_AXIAL=1;
char NomBottom[]="Bottom";

int main()
{
  MESHING *pmesh=new MESHING;
  pmesh->setName( "RJHMESH" ) ;

  double coord3DTab[36]={0.024155000000000003, 0.041837687256826218, -0.30499999999999999, 0.048310000000000013, -1.015761910347357e-17, -0.30499999999999999, 0.096620000000000011, -1.8329792978583059e-18,
  -0.30499999999999999, 0.12077499999999999, 0.041837687256826225, -0.30499999999999999, 0.096620000000000011, 0.083675374513652451, -0.30499999999999999, 0.048310000000000013, 0.083675374513652465,
  -0.30499999999999999, 0.024155000000000003, 0.041837687256826218, -0.2863, 0.048310000000000013, -1.015761910347357e-17, -0.2863, 0.096620000000000011, -1.8329792978583059e-18, -0.2863,
  0.12077499999999999, 0.041837687256826225, -0.2863, 0.096620000000000011, 0.083675374513652451, -0.2863, 0.048310000000000013, 0.083675374513652465, -0.2863};
  vector<double> coords3D(coord3DTab,coord3DTab+36);
  unsigned nNodes=coords3D.size()/3;
  pmesh->setCoordinates(3, nNodes, &(coords3D[0]), "CARTESIAN",MED_EN::MED_FULL_INTERLACE);
  string coordname[3] = { "x", "y", "z" } ;
  pmesh->setCoordinatesNames(coordname) ;
  // Set coordinates units
  string coordunit[3] = { "cm", "cm", "cm" } ;
  pmesh->setCoordinatesUnits(coordunit) ;

  medGeometryElement mytypes3D[1]={MED_POLYHEDRA};
  medGeometryElement mytypes2D[2]={MED_QUAD4,MED_POLYGON};
  pmesh->setNumberOfTypes(1, MED_EN::MED_CELL);
  pmesh->setNumberOfTypes(2,MED_EN::MED_FACE);
  pmesh->setTypes(mytypes3D,MED_EN::MED_CELL);
  pmesh->setTypes(mytypes2D,MED_EN::MED_FACE);
  //
  const int nbPolyH = 1;
  int polyHInd[2]={1, 9};
  int polyHFaceConn[43]={8, 9, 10, 11, 12, 7, -1,
                         2, 1, 6,  5,  4,  3, -1,
                         2, 3, 9,  8,         -1,
                         3, 4, 10, 9,         -1,
                         4, 5, 11, 10,        -1,
                         5, 6, 12, 11,        -1,
                         6, 1, 7, 12,         -1,
                         1, 2, 8, 7};
  //
  int conn2DClassicalTab[24]={2, 3, 9, 8, 3, 4, 10, 9, 4, 5, 11, 10, 5, 6, 12, 11, 6, 1, 7, 12, 1, 2, 8, 7};
  vector<int> conn2DClassical(conn2DClassicalTab,conn2DClassicalTab+24);
  //
  int conn2DPolyIndTab[3]={1, 7, 13};
  vector<int > conn2DPolyInd(conn2DPolyIndTab,conn2DPolyIndTab+3);
  int conn2DPolyTab[12]={1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  //
  int nbOfElemsFace[] = { conn2DClassical.size()/4, conn2DPolyInd.size()-1 };
  pmesh->setNumberOfElements(&nbPolyH,MED_EN::MED_CELL);
  pmesh->setNumberOfElements(nbOfElemsFace,MED_EN::MED_FACE);
  //
  //
  pmesh->setConnectivity(MED_EN::MED_CELL, MED_EN::MED_POLYHEDRA, polyHFaceConn,polyHInd);
  pmesh->setConnectivity(MED_EN::MED_FACE, MED_EN::MED_QUAD4,&conn2DClassical[0]);
  pmesh->setConnectivity(MED_EN::MED_FACE, MED_EN::MED_POLYGON,&conn2DPolyInd[0],conn2DPolyIndTab);
  int bottom=7;
  addMedFacesGroup(*pmesh,1,&bottom,NomBottom);
  int id = pmesh->addDriver(MED_DRIVER, "coeur.med", pmesh->getName());
  pmesh->write(id) ;
  pmesh->rmDriver(id);
  delete pmesh;
  //
  
  MESH mesh;//(MED_DRIVER,,"RJHMESH");//"/home/geay/test_FGUI.med","FluidMesh_1");//../STEP3/coeur.med","RJHMESH");
  mesh.setName("RJHMESH");
  MED_MESH_RDONLY_DRIVER22 *drv=new MED_MESH_RDONLY_DRIVER22("coeur.med",&mesh);
  drv->desactivateFacesComputation();
  int newDrv=mesh.addDriver(*drv);
  delete drv;
  mesh.read(newDrv);
  cout << " *** " << mesh.getNumberOfTypes(MED_EN::MED_FACE) << endl;
  cout << "Expected 8 - having " << mesh.getNumberOfElements(MED_FACE,MED_ALL_ELEMENTS) << endl;
  cout << "Expected 6 - having " << mesh.getNumberOfElements(MED_FACE,MED_QUAD4) << endl;
  cout << "Expected 1 - having " << mesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) << endl;
  return 0;
}
