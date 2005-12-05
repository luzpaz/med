// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/
//
// This program test the implementation of the class MED_MESH_WRONLY_DRIVER for Polygons and Polyhedron. //

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

using namespace std;
using namespace MEDMEM;


int main (int argc, char ** argv)
{
  if (argc <3) // after 3, ignored !
    {
      cerr << "Usage : " << argv[0] 
	   << " filename meshname" << endl << endl;
      exit(-1);
    }

  string filename = argv[1];
  string meshname = argv[2];

  // Lecture d'un fichier MED
  MESH * myMesh = new MESH();
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshReadDriver(filename,myMesh);
  myMeshReadDriver.setMeshName(meshname);
  myMeshReadDriver.open();
  myMeshReadDriver.read();
  myMeshReadDriver.close();

  // Ecriture dans un fichier MED
  string filename2 = filename + "_copy";
  MED_MESH_WRONLY_DRIVER myMeshWriteDriver(filename2,myMesh);
  myMeshWriteDriver.setMeshName(meshname);
  myMeshWriteDriver.open();
  myMeshWriteDriver.write();
  myMeshWriteDriver.close();


  delete myMesh;
  return 0;
}
