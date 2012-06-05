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

#include<string>
#include<deque>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFileBrowser.hxx"

using namespace std;
using namespace MEDMEM;
int main (int argc, char ** argv)
{
  if ( argc < 3 ) {
    cout << "Usage: " << argv[0] << "<input med file> <output med file>" << endl;
    return -1;
  }
  string filenameIN = argv[1] ;
  string filenameOUT = argv[2] ;
    
  MEDFILEBROWSER myMed(filenameIN);

  // we read all meshes in filenameIN 
  try {
    
    // read all mesh
    MESSAGE_MED("Read all meshes :") ;
    int NumberOfMeshes = myMed.getNumberOfMeshes() ;
    MESSAGE_MED("Number of meshes : "<<NumberOfMeshes) ;
    vector<string> meshNames = myMed.getMeshNames() ;
    for (int i=0; i<NumberOfMeshes; i++)
    {
      GMESH* mesh = myMed.isStructuredMesh( meshNames[i] ) ? (GMESH*) new GRID : (GMESH*) new MESH;
      int drv = mesh->addDriver(MED_DRIVER, filenameIN, meshNames[i] );
      mesh->read(drv);
      MESSAGE_MED("  - Mesh "<<i+1<<", named "<<meshNames[i]<<" read !");
      MED_MESH_WRONLY_DRIVER myMeshDriver(filenameOUT,mesh);
      MESSAGE_MED("After declaration of MED_MESH_DRIVER");
      myMeshDriver.setMeshName(meshNames[i]);
      MESSAGE_MED("After setMeshName");
      myMeshDriver.open() ;
      MESSAGE_MED("After open");
      myMeshDriver.write() ;
      MESSAGE_MED("After write");
      myMeshDriver.close() ;
      MESSAGE_MED("After close");
      mesh->removeReference();
    }

  } catch (MEDEXCEPTION& ex){
    MESSAGE_MED(ex.what()) ;
    return -1;
  }
  return 0;
}
