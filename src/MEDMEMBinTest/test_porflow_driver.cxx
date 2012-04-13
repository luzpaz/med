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
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

int main (int argc, char ** argv)
{
    /* process the arguments */
    if (argc != 2) 
    {
        cerr << "Usage : " << argv[0] 
        << " Porflowfilename" << endl << endl
        << "-> lit le fichier Porflowfilename ,cr√©e 2 fichiers : MED et VTK" << endl;
        exit(-1);
    }

    string porflowfilename  = argv[1];

    // Construction des noms de fichier
    const string ext=".inp";
    string::size_type pos=porflowfilename.find(ext,0);
    string basename (porflowfilename, 0, pos); // nom sans extension
    string medfile=basename+".med"; // nom fichier med ‡ creer
    string vtkfile=basename+".vtk"; // nom fichier vtk ‡ creer
    string::size_type pos1=porflowfilename.rfind('/');
    string meshName (porflowfilename,pos1+1,pos-pos1-1); //get rid of directory & extension
    cout << meshName << endl;

    // lecture du fichier porflow
    MESH * myMesh= new MESH; 
    PORFLOW_MESH_RDONLY_DRIVER myPorflowMeshDriver(porflowfilename, myMesh) ;
    myPorflowMeshDriver.open() ;
    myPorflowMeshDriver.read() ;
    myPorflowMeshDriver.close() ;
    
    cout << "Impression de MESH : " << endl;
    cout << *myMesh;

    // creation d'un fichier med
    cout << "creation d'un fichier med : " << endl;
    int idMed = myMesh->addDriver(MED_DRIVER, medfile, meshName);
    myMesh->write(idMed) ;

    // creation d'un fichier vtk
    cout << "creation d'un fichier vtk : " << endl;
    int idVtk = myMesh->addDriver(VTK_DRIVER, vtkfile, meshName);
    myMesh->write(idVtk) ;


    int SpaceDimension = myMesh->getSpaceDimension() ;
    int MeshDimension  = myMesh->getMeshDimension() ;
    int NumberOfNodes  = myMesh->getNumberOfNodes() ;

    cout << "Space Dimension : " << SpaceDimension << endl << endl ; 

    cout << "Mesh Dimension : " << MeshDimension << endl << endl ; 

    const double * Coordinates = myMesh->getCoordinates(MED_FULL_INTERLACE) ;

    cout << "Show Nodes Coordinates : " << endl ;

    cout << "Name :" << endl ;
    const string * CoordinatesNames = myMesh->getCoordinatesNames() ;
    for(int i=0; i<SpaceDimension ; i++) {
      cout << " - " << CoordinatesNames[i] << endl ;
    }
    cout << "Unit :" << endl ;
    const string * CoordinatesUnits = myMesh->getCoordinatesUnits() ;
    for(int i=0; i<SpaceDimension ; i++) {
      cout << " - " << CoordinatesUnits[i] << endl ;
    }
    for(int i=0; i<NumberOfNodes ; i++) {
      cout << "Nodes " << i+1 << " : " ;
      for (int j=0; j<SpaceDimension ; j++)
        cout << Coordinates[i*SpaceDimension+j] << " " ;
      cout << endl ;
    }

    cout << "The END" << endl;
    myMesh->removeReference();

    // remontÈe du fichier med en mÈmoire
    myMesh= new MESH(MED_DRIVER,medfile,meshName);
    myMesh->removeReference();

}
