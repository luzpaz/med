#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"

int main (int argc, char ** argv)
{
    /* process the arguments */
    if (argc != 2) 
    {
	cerr << "Usage : " << argv[0] 
	<< "Gibifilename" << endl << endl
	<< "-> lit le fichier gibi filename ,crée un fichier MED" << endl;
	exit(-1);
    }
    string gibifilename  = argv[1];

    // Construction des noms de fichier
    const string ext=".sauve";
    string::size_type pos=gibifilename.find(ext,0);
    string basename (gibifilename, 0, pos);
    string medfile=basename+".med";
    string cast2medfile="cast_"+basename+".med";

    // lecture du fichier gibi
    MESH * myMesh= new MESH() ; 
    GIBI_MESH_RDONLY_DRIVER myGibiMeshDriver(gibifilename, myMesh) ;
    myGibiMeshDriver.open() ;
    myGibiMeshDriver.read() ;
    myGibiMeshDriver.close() ;

    cout << "Impression de MESH : " << endl;
    cout << *myMesh;

    // creation d'un fichier med
    int idMed = myMesh->addDriver(MED_DRIVER, medfile, basename);
    myMesh->write(idMed) ;
    delete myMesh;

}
