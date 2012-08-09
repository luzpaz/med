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
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;
using namespace DRIVERFACTORY;

int main (int argc, char ** argv)
{
    /* process the arguments */
    if (argc != 2) 
    {
        cerr << "Usage : " << argv[0] 
        << " Gibifilename" << endl << endl
        << "-> lit le fichier gibi filename ,crée 2 fichiers : MED et VTK" << endl;
        exit(-1);
    }

    string gibifilename  = argv[1];

    // Construction des noms de fichier
    const string ext=".sauve";
    string::size_type pos=gibifilename.find(ext,0);
    string basename (gibifilename, 0, pos); // nom sans extension
    string medfile21=basename+"_V21.med"; // nom fichier med 2.1 à creer
    string medfile22=basename+"_V22.med"; // nom fichier med 2.2 à creer
    string vtkfile=basename+".vtk"; // nom fichier vtk à creer
    string::size_type pos1=gibifilename.rfind('/');
    string meshName (gibifilename,pos1+1,pos-pos1-1); // get rid of directory & extension
    cout << meshName << endl;

    // lecture du fichier gibi
    // MESH * myMesh= new MESH(GIBI_DRIVER,gibifilename);
    MESH * myMesh= new MESH; 
    GIBI_MESH_RDONLY_DRIVER myGibiMeshDriver(gibifilename, myMesh) ;
    myGibiMeshDriver.open() ;
    myGibiMeshDriver.read() ;
    myGibiMeshDriver.close() ;

    cout << "Impression de MESH : " << endl;
    cout << *myMesh;

    cout << "creation du fichier med 22 : " << endl;
    int idMed22 = myMesh->addDriver(MED_DRIVER, medfile22, meshName);
    myMesh->write(idMed22) ;

    // creation d'un fichier vtk
    cout << "creation d'un fichier vtk : " << endl;
    int idVtk = myMesh->addDriver(VTK_DRIVER, vtkfile, meshName);
    myMesh->write(idVtk) ;
    myMesh->removeReference();

    // remontée mémoire du fichier med 22
    myMesh= new MESH(MED_DRIVER,medfile22,meshName);

    myMesh->removeReference();
}
