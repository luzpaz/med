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
#include "MEDMEM_Grid.hxx"
#include <memory>

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
        for(int j=1;j!=(int)XYZ_Array[i].size();++j)
            XYZ_Array[i][j]=XYZ_Array[i][j-1] + pas[i];
    }
    
    std::vector<std::string> coord_name(SpaceDimension,"X");
    if(SpaceDimension>=2)
        coord_name[1]="Y";
    if(SpaceDimension>=3)
        coord_name[2]="Z";
    std::vector<std::string> coord_unit(SpaceDimension,"cm");

    // creation du pointeur MESH à partir d'un GRID, test affichage
    MEDMEM::GMESH *grid=(new MEDMEM::GRID( XYZ_Array, coord_name, coord_unit, MED_CARTESIAN) );
    const MEDMEM::MESH* Mesh = grid->convertInMESH(); 
    Mesh->getConnectivityptr();
    std::cout << "Affichage du maillage : " << endl << *Mesh << endl;
    Mesh->removeReference();
    grid->removeReference();
}
