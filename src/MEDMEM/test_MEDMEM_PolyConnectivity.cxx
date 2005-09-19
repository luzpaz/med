// This program test the implementation of the class CONNECTIVITY for Polygons and Polyhedron. //

#include "MEDMEM_Connectivity.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main(void)
{
  CONNECTIVITY myNodalConnectivity;
  CONNECTIVITY myDescendingConnectivity;

  //POLYGONS
  const med_int NumberOfPolygons = 2;
  const med_int ConnectivitySize = 12;
  med_int PolygonsConnectivityIndex[NumberOfPolygons+1] = {1,7,13};

  //Nodal
  med_int PolygonsNodalConnectivity[ConnectivitySize] = {1,2,3,4,5,10,10,5,6,7,8,9};

  myNodalConnectivity.setPolygonsConnectivity(MED_NODAL,MED_CELL,PolygonsNodalConnectivity,PolygonsConnectivityIndex,ConnectivitySize,NumberOfPolygons);

  //Descending
  med_int PolygonsDescendingConnectivity[ConnectivitySize] = {1,2,3,4,11,10,11,5,6,7,8,9};

  myDescendingConnectivity.setPolygonsConnectivity(MED_DESCENDING,MED_CELL,PolygonsDescendingConnectivity,PolygonsConnectivityIndex,ConnectivitySize,NumberOfPolygons);

  //POLYHEDRON
  const med_int NumberOfPolyhedron = 2;
  med_int PolyhedronIndex[NumberOfPolyhedron+1] = {1,10,20};

  //Nodal
  const med_int NumberOfFaces = 19;
  const med_int NumberOfNodes = 74;
  med_int PolyhedronFacesIndex[NumberOfFaces+1] = {1,7,11,15,19,23,27,31,34,39,44,48,52,55,58,61,64,68,72,75};
  med_int PolyhedronNodalConnectivity[NumberOfNodes] = {1,2,3,4,5,6,1,7,8,2,2,8,9,3,4,3,9,10,5,4,10,11,6,5,11,12,1,6,12,7,7,12,8,10,9,8,12,11,13,14,15,3,2,13,2,8,16,14,13,16,17,15,14,17,15,17,18,15,18,9,3,15,9,2,3,9,8,8,9,17,16,9,18,17};

  myNodalConnectivity.setPolyhedronConnectivity(MED_NODAL,PolyhedronNodalConnectivity,PolyhedronIndex,NumberOfNodes,NumberOfPolyhedron,PolyhedronFacesIndex,NumberOfFaces);

  //Descending
  const med_int DescendingConnectivitySize = 19;
  med_int PolyhedronDescendingConnectivity[DescendingConnectivitySize] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,3,17,18};

  myDescendingConnectivity.setPolyhedronConnectivity(MED_DESCENDING,PolyhedronDescendingConnectivity,PolyhedronIndex,DescendingConnectivitySize,NumberOfPolyhedron);

  cout << myNodalConnectivity;
  cout << myDescendingConnectivity;

  return 0;
}
