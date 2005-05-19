#include "MEDMEM_Grid.hxx"

#ifdef _DEBUG_
#include "LocalTraceCollector.hxx"
#endif /* ifdef _DEBUG_*/

using namespace MEDMEM;
using namespace MED_EN;

static void usage(const char * test)
{
    cerr << "Usage : " << test
	<< " SpaceDimension  nbMaille" << endl << endl
	<< "-> Crée une grille cartesienne en dimension SpaceDimension avec nbMaille suivant chaque direction" << endl;
    exit(-1);
}

int main (int argc, char ** argv) {

    /* process the arguments */
    if (argc != 3)
	usage(argv[0]);

#ifdef _DEBUG_
  LocalTraceCollector::instance();
#endif /* ifdef _DEBUG_*/

    const int SpaceDimension=atoi(argv[1]);
    const int nbMaille=atoi(argv[2]);
    if(SpaceDimension>3 || SpaceDimension<1 || nbMaille<1)
	usage(argv[0]);

    // Creation des tableaux necessaires à la construction de GRID
    std::vector<int> nMaille(SpaceDimension,nbMaille);
    std::vector<double> Origine(SpaceDimension,0.0);
    std::vector<double> pas(SpaceDimension,10.0);
    std::vector<std::vector<double> > XYZ_Array(SpaceDimension);
    for(int i=0;i!=SpaceDimension;++i)
    {
	XYZ_Array[i].resize(nMaille[i]+1); // nbre de noeuds = nbre de mailles +1
	XYZ_Array[i][0]=Origine[i];
	for(int j=1;j!=XYZ_Array[i].size();++j)
	    XYZ_Array[i][j]=XYZ_Array[i][j-1] + pas[j-1];
    }
    
    std::vector<std::string> coord_name(SpaceDimension,"X");
    if(SpaceDimension>=2)
	coord_name[1]="Y";
    if(SpaceDimension>=3)
	coord_name[2]="Z";
    std::vector<std::string> coord_unit(SpaceDimension,"cm");

    // creation du pointeur MESH à partir d'un GRID, test affichage
    std::auto_ptr<MEDMEM::MESH> Mesh (new MEDMEM::GRID( XYZ_Array, coord_name, coord_unit, MED_CARTESIAN) );
    const MEDMEM::CONNECTIVITY* conn = Mesh->getConnectivityptr();
    std::cout << "Affichage du maillage : " << endl << *Mesh << endl;
}
