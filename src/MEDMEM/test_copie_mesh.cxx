using namespace std;
#include<string>

#include <math.h>
#include <stdlib.h>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_define.hxx"
using namespace MEDMEM;


int main (int argc, char ** argv) {

  int read;

  if (argc <3) { // after 3, ignored !
    cerr << "Usage : " << argv[0] 
	 << " filename meshname" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  MESH * myMesh= new MESH() ;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ;
  myMeshDriver.close() ;

  cout << * myMesh << endl;
  MESH * myMesh2 = new MESH(* myMesh);
  delete myMesh;
  cout << * myMesh2 << endl;
  delete myMesh2;

  return 0;
}
