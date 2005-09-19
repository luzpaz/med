#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace std;
using namespace MEDMEM;
int main (int argc, char ** argv)
{
    /* process the arguments */
    if (argc != 2) 
    {
	cerr << "Usage : " << argv[0] 
	<< "Gibifilename" << endl << endl
	<< "-> lit le fichier gibi filename ,crée 2 fichiers : MED et VTK" << endl;
	exit(-1);
    }

    string gibifilename  = argv[1];

    // Construction des noms de fichier
    const string ext=".sauve";
    string::size_type pos=gibifilename.find(ext,0);
    string basename (gibifilename, 0, pos); // nom sans extension
    string medfile=basename+".med"; // nom fichier med à creer
    string vtkfile=basename+".vtk"; // nom fichier vtk à creer
    string::size_type pos1=gibifilename.rfind('/');
    string meshName (gibifilename,pos1+1,pos-pos1-1); // get rid of directory & extension
    cout << meshName << endl;

    // lecture du fichier gibi
    // MESH * myMesh= new MESH(GIBI_DRIVER,gibifilename);
    MESH * myMesh= new MESH() ; 
    GIBI_MESH_RDONLY_DRIVER myGibiMeshDriver(gibifilename, myMesh) ;
    myGibiMeshDriver.open() ;
    myGibiMeshDriver.read() ;
    myGibiMeshDriver.close() ;

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
    delete myMesh;

    // remontée mémoire du fichier med
    myMesh= new MESH(MED_DRIVER,medfile,meshName);
    //myMesh= new MESH() ;
    //myMesh->setName(meshName);
    //MED_MESH_RDONLY_DRIVER myMeshDriver(medfile,myMesh) ;
    //myMeshDriver.setMeshName(meshName);
    //myMeshDriver.open() ;
    //myMeshDriver.read() ;
    //myMeshDriver.close() ;
    //cout << "Impression 2 de MESH : " << endl;
    //cout << *myMesh;
    delete myMesh;


}
