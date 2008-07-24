// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Field.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main (int argc, char ** argv) {
  double Coordinates[57] = {
    2.0, 3.0, 2.0,
    3.0, 2.0, 2.0,
    4.0, 1.0, 2.0,
    2.0, 0.0, 2.0,
    0.0, 1.0, 2.0,
    1.0, 2.0, 2.0,
    2.0, 3.0, 1.0,
    3.0, 2.0, 0.0,
    4.0, 1.0, 0.0,
    2.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    1.0, 2.0, 0.0,
    5.0, 3.0, 2.0,
    7.0, 2.0, 2.0,
    6.0, 0.0, 2.0,
    6.0, 3.0, 0.0,
    7.0, 2.0, 0.0,
    6.0, 0.0, -1.0,
    5.0, 1.0, -3.0};

  const int REFnodalConnOfFaces[74] = {
    1, 2, 3, 4, 5, 6, // Polyhedron 1
    1, 7, 8, 2, 
    2, 8, 9, 3, 
    4, 3, 9, 10, 
    5, 4, 10, 11, 
    6, 5, 11, 12, 
    1, 6, 12, 7, 
    7, 12, 8, 
    10, 9, 8, 12, 11,
				   
    13, 14, 15, 3, 2, // Polyhedron 2
    13, 2, 8, 16, 
    14, 13, 16, 17, 
    15, 14, 17, 
    15, 17, 18, 
    15, 18, 9, 
    3, 15, 9, 
    2, 3, 9, 8, 
    8, 9, 17, 16, 
    9, 18, 17};

  const int REFfacesIndex[20] = {
    1, 7, 11, 15, 19, 23, 27, 31, 34, 
    39, 44, 48, 52, 55, 58, 61, 64, 68, 72, 75};

  const int REFpolyIndex[3] = {1, 10, 20};

  double PolygonCoordinates[27] = {
    2.0, 3.0, 12.0,
    3.0, 2.0, 12.0,
    4.0, 1.0, 12.0,
    2.0, 0.0, 12.0,
    0.0, 1.0, 12.0,
    1.0, 2.0, 12.0,
    5.0, 3.0, 12.0,
    7.0, 2.0, 12.0,
    6.0, 0.0, 12.0};

  const int REFpolygonFaces[11] = {
    1, 2, 3, 4, 5, 6, // Polygon 1   
    7, 8, 9, 3, 2}; // Polygon 2

  const int REFpolygonIndex[3] = {1, 7, 12};

  MESHING myMeshing;
  myMeshing.setName("meshing");

  int NumberOfNodes = 19;
  int SpaceDimension = 3;
  int MeshDimension = 3;

  const int NumberOfTypes = 1;
  medGeometryElement Types[NumberOfTypes] = {MED_TETRA4};
  const int NumberOfElements[NumberOfTypes] = {1};

  myMeshing.setNumberOfTypes(NumberOfTypes, MED_CELL);

  myMeshing.setCoordinates(SpaceDimension, NumberOfNodes, Coordinates,
			   "CARTESIAN", MED_FULL_INTERLACE);
  myMeshing.setSpaceDimension(SpaceDimension);
  myMeshing.setMeshDimension(MeshDimension);
  myMeshing.setTypes(Types, MED_CELL);
  myMeshing.setNumberOfElements(NumberOfElements, MED_CELL);

  string Names[3] = {"X","Y","Z"};
  myMeshing.setCoordinatesNames(Names);

  string Units[3] = {"cm","cm","cm"};
  myMeshing.setCoordinatesUnits(Units);

  const int sizeTetra = 4*1 ;
  int ConnectivityTetra[sizeTetra]=
  {
    17, 9, 18, 19
  };
  
  myMeshing.setConnectivity(ConnectivityTetra, MED_CELL, MED_TETRA4);

  myMeshing.setPolyhedraConnectivity(REFpolyIndex, REFfacesIndex, 
				     REFnodalConnOfFaces, 2, MED_CELL);


  MESHING myPolygonMeshing;
  myPolygonMeshing.setName("PolygonMeshing");

  NumberOfNodes = 9;
  SpaceDimension = 3;
  MeshDimension = 2;
  medGeometryElement PolygonTypes[NumberOfTypes] = {MED_TRIA3};
  const int PolygonNumberOfElements[NumberOfTypes] = {2};

  myPolygonMeshing.setNumberOfTypes(NumberOfTypes, MED_CELL);

  myPolygonMeshing.setCoordinates(SpaceDimension, NumberOfNodes, PolygonCoordinates,
			   "CARTESIAN", MED_FULL_INTERLACE);
  myPolygonMeshing.setSpaceDimension(SpaceDimension);
  myPolygonMeshing.setMeshDimension(MeshDimension);
  myPolygonMeshing.setTypes(PolygonTypes, MED_CELL);
  myPolygonMeshing.setNumberOfElements(PolygonNumberOfElements, MED_CELL);

  myPolygonMeshing.setCoordinatesNames(Names);
  myPolygonMeshing.setCoordinatesUnits(Units);

  const int sizeTri = 3*2 ;
  int ConnectivityTri[sizeTri]=
  {
    1, 7, 2, 3, 9, 4
  };

  myPolygonMeshing.setConnectivity(ConnectivityTri, MED_CELL, MED_TRIA3);
  myPolygonMeshing.setPolygonsConnectivity(REFpolygonIndex, REFpolygonFaces, 2, MED_CELL);
  SUPPORT *sup2 = new SUPPORT(&myPolygonMeshing);
  FIELD<double> *areas = myPolygonMeshing.getArea(sup2);
  const double *vals2 = areas->getValue();
  const double REFAreaOfPolyg[4] ={1.5, 2, 6, 6.5};
  int nbPts=0;
  int i;
  for(i=0;i<4;i++)
    if(fabs(REFAreaOfPolyg[i]-vals2[i])<1e-12)
      nbPts++;
  delete areas;
  delete sup2;

  SUPPORT *sup = new SUPPORT(&myMeshing);
  FIELD<double> *vols = myMeshing.getVolume(sup);
  const double *vals = vols->getValue();
  const double REFVolOfPolyHedron[3] = {2.333333333333333, 11.66666666666666, 13.83224131414673};
  for(i=0;i<3;i++)
    if(fabs(REFVolOfPolyHedron[i]-vals[i])<1e-12)
      nbPts++;
  
  delete vols;
  delete sup;
  if(nbPts==7)
    {
      cout << "ALL TESTS OK !!!" << endl;
    }
  else
    {
      cout << "TEST FAILS !!!" << endl;
      return -1;
    }
  cout << "Writing test " << endl;
  int idMed22 = myMeshing.addDriver(MED_DRIVER,"totoPoly_V22.med",myMeshing.getName());
  myMeshing.write(idMed22);

}
