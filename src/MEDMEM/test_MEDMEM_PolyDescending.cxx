// This program test the transformation of a nodal connectivity into a descending one. This is done by the calculateDescendingConnectivity method. It must be valid with polygons and polyhedron. //

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
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

  // Lecture fichier med
  MESH * myMesh = new MESH();
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshReadDriver(filename,myMesh);
  myMeshReadDriver.setMeshName(meshname);
  myMeshReadDriver.open();
  myMeshReadDriver.read();
  myMeshReadDriver.close();

  // Calcul connectivite descendante
  myMesh->calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL);

  // Affichage connectivite descendante
  cout << * myMesh << endl;

  delete myMesh;
  return 0;
}
