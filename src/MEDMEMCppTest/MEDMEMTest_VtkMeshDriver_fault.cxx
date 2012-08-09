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

#include <MEDMEM_VtkMeshDriver.hxx>
#include <MEDMEM_Mesh.hxx>

using namespace std;
using namespace MEDMEM;

int main (int argc, char** argv)
{
  string meshname = "MyMesh";
  string filename = makeTmpFile( "my_pointe.vtk" );

  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filename);

  //Create an empty Mesh
  MESH * aMesh = new MESH();

  //Creation corect driver
  VTK_MESH_DRIVER *aVtkDriver = new VTK_MESH_DRIVER(filename, aMesh);
  aVtkDriver->setMeshName(meshname);
  aVtkDriver->open();

  // ? (BUG) ? In VTK_MESH_DRIVER::write() => Segmentation fault on attempt to write an empty mesh
  try
  {
    //Trying write an empty mesh
    aVtkDriver->write();
  }
  catch (MEDEXCEPTION & ex)
  {
    // good behaviour
  }
  catch (...)
  {
  }

  aVtkDriver->close();

  // (BUG) In copy constructor of VTK_MESH_DRIVER: Segmentation fault or Infinite loop
  VTK_MESH_DRIVER aVtkDriverCpy_2 (*aVtkDriver);

  delete aVtkDriver;
  delete aMesh;
}
