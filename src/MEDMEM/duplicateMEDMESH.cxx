using namespace std;
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

int main (int argc, char ** argv) {
  
    string filenameIN = argv[1] ;
    string filenameOUT = argv[2] ;
    
    MED * myMed = new MED() ;
    MED_MED_DRIVER myMedDriver(filenameIN,myMed) ;

  // we read all meshes in filenameIN 
  try {
    
    int read ;
    myMedDriver.open();
    myMedDriver.readFileStruct();
    myMedDriver.close();
    
    // read all mesh
    MESSAGE("Read all meshes :") ;
    int NumberOfMeshes = myMed->getNumberOfMeshes() ;
    MESSAGE("Number of meshes : "<<NumberOfMeshes) ;
    deque<string> MeshName = myMed->getMeshNames() ;
    map<string,MESH*> _meshes ;
    for (int i=0; i<NumberOfMeshes; i++) {
      _meshes[MeshName[i]]=myMed->getMesh(MeshName[i]) ;
      _meshes[MeshName[i]]->read();
      MESSAGE("  - Mesh "<<i+1<<", named "<<MeshName[i]<<" read !");
      MED_MESH_WRONLY_DRIVER myMeshDriver(filenameOUT,_meshes[MeshName[i]]);
      MESSAGE("After declaration of MED_MESH_DRIVER");
      myMeshDriver.setMeshName(MeshName[i]);
      MESSAGE("After setMeshName");
      myMeshDriver.open() ;
      MESSAGE("After open");
      myMeshDriver.write() ;
      MESSAGE("After write");
      myMeshDriver.close() ;
      MESSAGE("After close");
    }

    // set support : support must be calculated with mesh information !!!
    myMed->updateSupport() ;

  } catch (MEDEXCEPTION& ex){
    MESSAGE(ex.what()) ;
  }
}
