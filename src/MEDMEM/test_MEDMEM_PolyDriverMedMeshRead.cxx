// This program test the implementation of the class MED_MESH_RDONLY_DRIVER for Polygons and Polyhedron. //

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

  MESH * myMesh = new MESH();
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshReadDriver(filename,myMesh);
  myMeshReadDriver.setMeshName(meshname);
  myMeshReadDriver.open();
  myMeshReadDriver.read();
  myMeshReadDriver.close();

  cout << * myMesh << endl;

  delete myMesh;
  return 0;
}
