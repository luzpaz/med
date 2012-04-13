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

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

static void affiche_connectivity(const CONNECTIVITY * myConnectivity, MESH * myMesh)
{
  int MeshDimension  = myMesh->getMeshDimension() ;
  int NumberOfNodes  = myMesh->getNumberOfNodes() ;

  int NumberOfTypes                 = myMesh->getNumberOfTypes(MED_CELL) ;
  const medGeometryElement  * Types = myMesh->getTypes(MED_CELL) ;

  cout << "Show Connectivity (Nodal) :" << endl ;
  for (int i=0; i<NumberOfTypes; i++) {
    cout << "For type " << Types[i] << " : " << endl ;
    int NumberOfElements = myMesh->getNumberOfElements(MED_CELL,Types[i]);
    const int * connectivity =  myMesh->getConnectivity(MED_NODAL,MED_CELL,Types[i]);
    int NomberOfNodesPerCell = Types[i]%100 ;
    for (int j=0;j<NumberOfElements;j++){
      cout << "Element "<< j+1 <<" : " ;
      for (int k=0;k<NomberOfNodesPerCell;k++)
        cout << connectivity[j*NomberOfNodesPerCell+k]<<" ";
      cout << endl ;
    }
  }

  cout << "Show Reverse Nodal Connectivity :" << endl ;
  const int * ReverseNodalConnectivity = myMesh->getReverseConnectivity(MED_NODAL) ;
  const int * ReverseNodalConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    cout << "Node "<<i+1<<" : " ;
    for (int j=ReverseNodalConnectivityIndex[i];j<ReverseNodalConnectivityIndex[i+1];j++)
      cout << ReverseNodalConnectivity[j-1] << " " ;
    cout << endl ;
  }

  cout << "Show Connectivity (Descending) :" << endl ;
  int NumberOfElements ;
  const int * connectivity ;
  const int * connectivity_index ;
  myMesh->calculateConnectivity(MED_DESCENDING,MED_CELL);
  try {
    NumberOfElements = myMesh->getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
    connectivity =  myMesh->getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
    connectivity_index =  myMesh->getConnectivityIndex(MED_DESCENDING,MED_CELL);
  }
  catch (MEDEXCEPTION& m) {
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
  const int * ReverseDescendingConnectivity = myMesh->getReverseConnectivity(MED_DESCENDING) ;
  const int * ReverseDescendingConnectivityIndex = myMesh->getReverseConnectivityIndex(MED_DESCENDING) ;

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
    MESSAGE_MED("ERROR : MeshDimension = 1 !");
    MESSAGE_MED("We could not see Reverse Descending Connectivity.") ;
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
  const int * face_connectivity =  myMesh->getConnectivity(MED_NODAL,constituentEntity,MED_ALL_ELEMENTS);
  const int * face_connectivity_index =  myMesh->getConnectivityIndex(MED_NODAL,constituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    cout << constituent <<i+1<<" : " ;
    for (int j=face_connectivity_index[i];j<face_connectivity_index[i+1];j++)
      cout << face_connectivity[j-1]<<" ";
    cout << endl ;
  }
}


int main (int argc, char ** argv) {

  if (argc <3) { // after 3, ignored !
    cerr << "Usage : " << argv[0] 
         << " filename meshname" << endl << endl;
    exit(-1);
  }

  string filename = argv[1] ;
  string meshname = argv[2] ;

  //Construction d'un maillage
  MESH * myMesh= new MESH;
  myMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER myMeshDriver(filename,myMesh) ;
  myMeshDriver.setMeshName(meshname);
  myMeshDriver.open() ;
  myMeshDriver.read() ; //A partir d'ici la connectivité est construite
  myMeshDriver.close() ;

  const CONNECTIVITY * myConnectivity = myMesh->getConnectivityptr();
  affiche_connectivity(myConnectivity, myMesh);

  CONNECTIVITY * myConnectivity2 = new CONNECTIVITY(* myConnectivity);
  affiche_connectivity(myConnectivity2, myMesh);

  CONNECTIVITY * myConnectivity3 = new CONNECTIVITY(* myConnectivity2);
  delete myConnectivity2;
  affiche_connectivity(myConnectivity3, myMesh);
  delete myConnectivity3;

  myMesh->removeReference();

  return 0;
}
