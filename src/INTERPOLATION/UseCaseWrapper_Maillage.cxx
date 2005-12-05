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
#include "stdio.h"
#include "stdlib.h"

#include <iostream.h>

#include "MEDMEM_InterpolationHighLevelObjects.hxx"

using namespace MEDMEM;
using namespace MED_EN;

int main (void) 
{
int i,j;

const char * fromFileName  = "fromMesh.med";
const char * fromMeshName  = "fromMesh";

MESH fromMesh(MED_DRIVER,fromFileName,fromMeshName);

Meta_Nuage_Maille * nuagemailles = new Meta_Nuage_Maille(const_cast<CONNECTIVITY *> (fromMesh.getConnectivityptr()));

int nbr_noeuds = fromMesh.getNumberOfNodes();

Meta_Maillage * maillage = new Meta_Maillage(nuagemailles,nbr_noeuds);

int nbr_mailles = maillage->DONNE_POINTEUR_NUAGEMAILLE()->SIZE();

int nbr_faces;

for (i=0;i<nbr_mailles;i++)
	{
	cout<<"Voisins de la maille "<<i<<" : "<<flush;
	nbr_faces=maillage->DONNE_NBR_FACES_MAILLE(i); 
	for (j=0;j<nbr_faces;j++) cout<<"["<<maillage->DONNE_VOISIN_DE_MAILLE(i,j)<<"]-("<<maillage->EST_AU_BORD_FACE_DE_MAILLE(i,j)<<") "<<flush;
	cout<<endl;
	}

}

