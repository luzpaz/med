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
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_DriverFactory.hxx"
#ifdef WIN32
#include <io.h>
#define access _access
#define W_OK 02
#endif

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;
using namespace DRIVERFACTORY;

int main (int argc, char ** argv)
{
    // Traitement arguments
    if (argc > 2) 
    {
        cerr << "Usage : " << argv[0] 
        << " filenameRoot" << endl << endl
        << "-> creer un maillage et le sauve dans le fichier filenameRoot22.med sous le format Med Fichier V2.2 car il contient un polygon." << endl;
        exit(-1);
    }

    string filenameRoot;
    if (argc==2) 
      filenameRoot= argv[1] ;
    else {
      if ( getenv("TMP") && access(getenv("TMP"),W_OK)==0 )
        filenameRoot=getenv("TMP");
      else if (getenv("TMPDIR") && access(getenv("TMPDIR"),W_OK)==0 )
        filenameRoot=getenv("TMPDIR");
      else
        filenameRoot="/tmp";
      filenameRoot+="/testMeshingPoly";
    }

    string medfilename  = filenameRoot + "22.med";

    // Creation maillage
    //***********************************************************************************

      MESHING *myMeshing=new MESHING;
      myMeshing->setName("myMeshing");

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

      myMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);

      string Names[3] = { "X","Y","Z" };
      myMeshing->setCoordinatesNames(Names);

      string Units[3] = { "cm","cm","cm" };
      myMeshing->setCoordinatesUnits(Units);

      //   define conectivities of classic types

      //      cell part

      const int NumberOfTypes = 3;
      medGeometryElement Types[NumberOfTypes] = {MED_TRIA3,MED_QUAD4,MED_POLYGON};
      const int NumberOfElements[NumberOfTypes] = {1,4,1};

      myMeshing->setNumberOfTypes(NumberOfTypes,MED_CELL);
      myMeshing->setTypes(Types,MED_CELL);
      myMeshing->setNumberOfElements(NumberOfElements,MED_CELL);

      int ConnectivityTria[1*3]=
        {
          7,4,1
        };

      myMeshing->setConnectivity(MED_CELL,MED_TRIA3,ConnectivityTria);

      int ConnectivityQuad[4*4]=
        {
          4,5,2,1,
          5,6,3,2,
          7,8,5,4,
          8,9,6,5
        };
  
      myMeshing->setConnectivity(MED_CELL,MED_QUAD4,ConnectivityQuad);

      // then define eventuel polygonal cells

      int ConnectivityPolygon[1*5]=
        {
          9,11,10,3,6
        };
      int ConnectivityPolygonIndex[2]=
        {
          1,6
        };

      myMeshing->setConnectivity(MED_CELL,MED_POLYGON,ConnectivityPolygon,ConnectivityPolygonIndex);

      // Ecriture fichier

      int idMed22 = myMeshing->addDriver(MED_DRIVER,medfilename,myMeshing->getName());
      myMeshing->write(idMed22) ;

      //      int idVtk = myMeshing->addDriver(VTK_DRIVER,"toto.vtk",myMeshing->getName());
      //      myMeshing->write(idVtk) ;
      myMeshing->removeReference();

  //************************************************************************************

  // impression de controle
  cout << endl;
  cout << "Impression de MESHING : " << endl;
  cout << myMeshing;

  if (argc==1) {
    cout << "Remove generated file" << endl;
    remove( medfilename.c_str() );
  }
}
