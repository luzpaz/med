using namespace std;
/* Programme de test du constructeur de copies de la classe CONNECTIVITY de MEDMEM
   jroy - 19/12/2002 */

#include <string>

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

void affiche_connectivity(CONNECTIVITY * myConnectivity, MESH * myMesh)
{
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int MeshDimension  = myMesh->getMeshDimension() ;
  int NumberOfNodes  = myMesh->getNumberOfNodes() ;

  int NumberOfTypes = myMesh->getNumberOfTypes(MED_CELL) ;
  medGeometryElement  * Types = myMesh->getTypes(MED_CELL) ;

  cout << "Show Connectivity (Nodal) :" << endl ;
  for (int i=0; i<NumberOfTypes; i++) {
    cout << "For type " << Types[i] << " : " << endl ;
    int NumberOfElements = myMesh->getNumberOfElements(MED_CELL,Types[i]);
    int * connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_CELL,Types[i]);
    int NomberOfNodesPerCell = Types[i]%100 ;
    for (int j=0;j<NumberOfElements;j++){
      cout << "Element "<< j+1 <<" : " ;
      for (int k=0;k<NomberOfNodesPerCell;k++)
	cout << connectivity[j*NomberOfNodesPerCell+k]<<" ";
      cout << endl ;
    }
  }

  cout << "Show Reverse Nodal Connectivity :" << endl ;
  int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
  int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    cout << "Node "<<i+1<<" : " ;
    for (int j=ReverseNodalConnectivityIndex[i];j<ReverseNodalConnectivityIndex[i+1];j++)
      cout << ReverseNodalConnectivity[j-1] << " " ;
    cout << endl ;
  }

  cout << "Show Connectivity (Descending) :" << endl ;
  int NumberOfElements ;
  int * connectivity ;
  int * connectivity_index ;
  myMesh->calculateConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL);
  try {
    NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
    connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
    connectivity_index =  myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  }
  catch (MEDEXCEPTION m) {
    cout << m.what() << endl ;
    exit (-1) ;
  }
  for (int j=0;j<NumberOfElements;j++) {
    cout << "Element "<<j+1<<" : " ;
    for (int k=connectivity_index[j];k<connectivity_index[j+1];k++)
      cout << connectivity[k-1]<<" ";
    cout << endl ;
  }

  cout << "Show Reverse Descending Connectivity :" << endl ;
  int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
  int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;

  int NumberOfConstituents  = 0;
  string constituent ;
  medEntityMesh constituentEntity ;

  if (MeshDimension==3) {
    constituent = "Face" ;
    constituentEntity = MED_FACE ;
  }

  if (MeshDimension==2) {
    constituent = "Edge" ;
    constituentEntity = MED_EDGE ;
  }

  if (MeshDimension==1) {
    MESSAGE("ERROR : MeshDimension = 1 !");
    MESSAGE("We could not see Reverse Descending Connectivity.") ;
  } else {
    NumberOfConstituents = myMesh->getNumberOfElements (constituentEntity,MED_ALL_ELEMENTS);
    for (int i=0; i<NumberOfConstituents; i++) {
      cout << constituent <<i+1<<" : " ;
      for (int j=ReverseDescendingConnectivityIndex[i];j<ReverseDescendingConnectivityIndex[i+1];j++)
	cout << ReverseDescendingConnectivity[j-1] << " " ;
      cout << endl ;
    }
  }
  cout << "Show "<<constituent<<" Connectivity (Nodal) :" << endl ;
  int * face_connectivity =  myMesh->getConnectivity(MED_FULL_INTERLACE,MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
  int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    cout << constituent <<i+1<<" : " ;
    for (int j=face_connectivity_index[i];j<face_connectivity_index[i+1];j++)
      cout << face_connectivity[j-1]<<" ";
    cout << endl ;
  }
}


int main (int argc, char ** argv) {


  if (argc !=3) {
    cerr << "Usage : " << argv[0] 
	 << " filename meshname" << endl << endl;
    exit(-1);
  }
  string filename = argv[1] ;
  string meshname = argv[2] ;

  //Construction d'un maillage
  MESH * myMesh= new MESH() ;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ; //A partir d'ici la connectivité est construite
  myMeshDriver.close() ;

  CONNECTIVITY * myConnectivity = myMesh->getConnectivityptr();
  affiche_connectivity(myConnectivity, myMesh);
  CONNECTIVITY * myConnectivity2 = new CONNECTIVITY(* myConnectivity);
  affiche_connectivity(myConnectivity2, myMesh);
  delete myConnectivity;//myConnectivity utile pour afficher myConnectivity2 via myMesh!

  return 0;
}
