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
#include "MEDMEM_DriverFactory.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;
using namespace DRIVERFACTORY;

int main (int argc, char ** argv)
{
    // Traitement arguments
    if (argc != 2) 
    {
	cerr << "Usage : " << argv[0] 
	<< " filenameRoot" << endl << endl
	<< "-> creer un maillage et le sauve dans le fichier filenameRoot22.med sous le format Med Fichier V2.2 car il contient un polygon." << endl;
	exit(-1);
    }

    string filenameRoot = argv[1] ;

    string medfilename  = filenameRoot + "22.med";

    // Creation maillage
    //***********************************************************************************

      MESHING myMeshing;
      myMeshing.setName("myMeshing");

      //   define coordinates

      int SpaceDimension = 2;
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

      myMeshing.setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);

      string Names[3] = { "X","Y","Z" };
      myMeshing.setCoordinatesNames(Names);

      string Units[3] = { "cm","cm","cm" };
      myMeshing.setCoordinatesUnits(Units);

      //   define conectivities of classic types

      //      cell part

      const int NumberOfTypes = 2;
      medGeometryElement Types[NumberOfTypes] = {MED_TRIA3,MED_QUAD4};
      const int NumberOfElements[NumberOfTypes] = {1,4};

      myMeshing.setNumberOfTypes(NumberOfTypes,MED_CELL);
      myMeshing.setTypes(Types,MED_CELL);
      myMeshing.setNumberOfElements(NumberOfElements,MED_CELL);

      MED_EN::med_int /*int*/ ConnectivityTria[1*3]=
	{
	  7,4,1
	};

      myMeshing.setConnectivity(ConnectivityTria,MED_CELL,MED_TRIA3);

      MED_EN::med_int /*int*/ ConnectivityQuad[4*4]=
	{
	  4,5,2,1,
	  5,6,3,2,
	  7,8,5,4,
	  8,9,6,5
	};
  
      myMeshing.setConnectivity(ConnectivityQuad,MED_CELL,MED_QUAD4);

      int MeshDimension = SpaceDimension ;
      // because there are 2D cells in the mesh

      myMeshing.setMeshDimension(MeshDimension) ;

      // then define eventuel polygonal cells

      MED_EN::med_int /*int*/ ConnectivityPolygon[1*5]=
	{
	  9,11,10,3,6
	};
      MED_EN::med_int /*int*/ ConnectivityPolygonIndex[2]=
	{
	  1,6
	};

      myMeshing.setPolygonsConnectivity(ConnectivityPolygonIndex,ConnectivityPolygon,1,MED_CELL);

      // Ecriture fichier

      medFileVersion version = getMedFileVersionForWriting();
      if (version == V21)
	setMedFileVersionForWriting(V22);

      int idMed22 = myMeshing.addDriver(MED_DRIVER,medfilename,myMeshing.getName());
      myMeshing.write(idMed22) ;

      //      int idVtk = myMeshing.addDriver(VTK_DRIVER,"toto.vtk",myMeshing.getName());
      //      myMeshing.write(idVtk) ;

  //************************************************************************************

  // impression de controle
  cout << endl;
  cout << "Impression de MESHING : " << endl;
  cout << myMeshing;

}
