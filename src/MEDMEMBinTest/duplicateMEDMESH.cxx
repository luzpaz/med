//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include<string>
#include<deque>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_Med.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Support.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_MedMedDriver.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

using namespace std;
using namespace MEDMEM;
int main (int argc, char ** argv) {

  if ( argc < 3 ) {
    cout << "Usage: " << argv[0] << "<input med file> <output med file>" << endl;
    return -1;
  }
  string filenameIN = argv[1] ;
    string filenameOUT = argv[2] ;
    
    MED * myMed = new MED() ;
    MED_MED_RDONLY_DRIVER myMedDriver(filenameIN,myMed) ;

  // we read all meshes in filenameIN 
  try {
    
    //int read ; !! UNUSED VARIABLE !!
    myMedDriver.open();
    myMedDriver.readFileStruct();
    myMedDriver.close();
    
    // read all mesh
    MESSAGE_MED("Read all meshes :") ;
    int NumberOfMeshes = myMed->getNumberOfMeshes() ;
    MESSAGE_MED("Number of meshes : "<<NumberOfMeshes) ;
    deque<string> MeshName = myMed->getMeshNames() ;
    map<string,MESH*> _meshes ;
    for (int i=0; i<NumberOfMeshes; i++) {
      _meshes[MeshName[i]]=myMed->getMesh(MeshName[i]) ;
      _meshes[MeshName[i]]->read();
      MESSAGE_MED("  - Mesh "<<i+1<<", named "<<MeshName[i]<<" read !");
      MED_MESH_WRONLY_DRIVER myMeshDriver(filenameOUT,_meshes[MeshName[i]]);
      MESSAGE_MED("After declaration of MED_MESH_DRIVER");
      myMeshDriver.setMeshName(MeshName[i]);
      MESSAGE_MED("After setMeshName");
      myMeshDriver.open() ;
      MESSAGE_MED("After open");
      myMeshDriver.write() ;
      MESSAGE_MED("After write");
      myMeshDriver.close() ;
      MESSAGE_MED("After close");
    }

    // set support : support must be calculated with mesh information !!!
    myMed->updateSupport() ;

  } catch (MEDEXCEPTION& ex){
    MESSAGE_MED(ex.what()) ;
    return -1;
  }
  return 0;
}
