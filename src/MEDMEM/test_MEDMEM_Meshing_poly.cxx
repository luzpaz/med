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
// See http://www.salome-platform.org/
//
#include "MEDMEM_Meshing.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main (int argc, char ** argv)
{
    // Traitement arguments
    if (argc != 2) 
    {
	cerr << "Usage : " << argv[0] 
	<< " medfilename" << endl << endl
	<< "-> cré un maillage et le sauve dans le fichier medfilename." << endl;
	exit(-1);
    }
    string medfilename  = argv[1];

    // Creation maillage
    //***********************************************************************************

      MESHING myMeshing;
      myMeshing.setName("myMeshing");

      //   define coordinates

      int SpaceDimension = 2;
      int MeshDimension = SpaceDimension ;
      int NumberOfNodes = 11;
      double Coordinates[2*11] = {
	0.0, 0.0,
	0.5, 0.0,
	1.0, 0.0,
	0.25, 0.5,
	0.5, 0.5,
	0.75, 0.5,
	0.0, 1.0,
	0.5, 1.0,
	1.0, 1.0,
	1.5, 0.0,
	1.5, 1.0,
      };

      myMeshing.setMeshDimension(MeshDimension) ;

      myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);

      string Names[3] = { "X","Y","Z" };
      myMeshing.setCoordinatesNames(Names);

      string Units[3] = { "cm","cm","cm" };
      myMeshing.setCoordinatesUnits(Units);

      //   define conectivities

      //      cell part

      const int NumberOfTypes = 3;
      medGeometryElement Types[NumberOfTypes] = {MED_TRIA3,MED_QUAD4,MED_POLYGON};
      const int NumberOfElements[NumberOfTypes] = {1,4,1};

      myMeshing.setNumberOfTypes(NumberOfTypes,MED_CELL);
      myMeshing.setTypes(Types,MED_CELL);

      int ConnectivityTria[1*3]=
	{
	  7,4,1
	};


      int ConnectivityQuad[4*4]=
	{
	  4,5,2,1,
	  5,6,3,2,
	  7,8,5,4,
	  8,9,6,5
	};
  
      int ConnectivityPolygon[1*5]=
	{
	  9,11,10,3,6
	};
      int ConnectivityPolygonIndex[2]=
	{
	  1,6
	};

      myMeshing.setNumberOfElements(NumberOfElements,MED_CELL,ConnectivityPolygonIndex,1);
      myMeshing.setConnectivity(ConnectivityTria,MED_CELL,MED_TRIA3);
      myMeshing.setConnectivity(ConnectivityQuad,MED_CELL,MED_QUAD4);
      myMeshing.setConnectivity(ConnectivityPolygon,MED_CELL,MED_POLYGON,ConnectivityPolygonIndex,1);

      /*
      //      face part

      const int NumberOfFacesTypes = 2 ;
      medGeometryElement FacesTypes[NumberOfFacesTypes] = {MED_TRIA3,MED_QUAD4} ;
      const int NumberOfFacesElements[NumberOfFacesTypes] = {4,4} ;

      myMeshing.setNumberOfTypes(NumberOfFacesTypes,MED_FACE);
      myMeshing.setTypes(FacesTypes,MED_FACE);
      myMeshing.setNumberOfElements(NumberOfFacesElements,MED_FACE);

      const int sizeTria = 3*4 ;
      int ConnectivityTria[sizeTria]=
      {
      1,4,3,
      1,5,4,
      1,6,5,
      1,3,6
      };
 
      myMeshing.setConnectivity(ConnectivityTria,MED_FACE,MED_TRIA3);

      int ConnectivityQua[4*4]=
      {
      7,8,9,10,
      11,12,13,14,
      11,7,8,12,
      12,8,9,13
      };

      myMeshing.setConnectivity(ConnectivityQua,MED_FACE,MED_QUAD4);
      */

      /*
      //      edge part

      // not yet implemented : if set, results are unpredictable.
      */


  // Ecriture fichier

      int idMed = myMeshing.addDriver(MED_DRIVER,medfilename,myMeshing.getName());
      myMeshing.write(idMed) ;

      //      int idVtk = myMeshing.addDriver(VTK_DRIVER,"toto.vtk",myMeshing.getName());
      //      myMeshing.write(idVtk) ;

  //************************************************************************************

  // impression de controle
  cout << endl;
  cout << "Impression de MESHING : " << endl;
  cout << myMeshing;

}
