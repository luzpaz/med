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
int i;

const int DIMENSION = 3;

const int isConvexFromMesh = 1;

const char * fromFileName  = "fromMesh.med";
const char * fromMeshName  = "fromMesh";

MESH * fromMesh = new MESH(MED_DRIVER,fromFileName,fromMeshName);

const char * toFileName    = "toMesh.med";
const char * toMeshName    = "toMesh";

MESH * toMesh   = new MESH(MED_DRIVER,toFileName,toMeshName);

Meta_Wrapper<DIMENSION> * fromWrapper = new Meta_Wrapper<DIMENSION>
					(
					fromMesh->getNumberOfNodes(),
					const_cast<double *> (fromMesh->getCoordinates(MED_FULL_INTERLACE)),
					const_cast<CONNECTIVITY *> (fromMesh->getConnectivityptr())
					);

Meta_Wrapper<DIMENSION> * toWrapper   = new Meta_Wrapper<DIMENSION>
					(
					toMesh->getNumberOfNodes(),
					const_cast<double *> (toMesh->getCoordinates(MED_FULL_INTERLACE))
					);

Meta_Mapping<DIMENSION> * mapping     = new  Meta_Mapping<DIMENSION> (fromWrapper,toWrapper);

mapping->Cree_Mapping(isConvexFromMesh);
					        
vector<int> vectormapping = mapping->Get_Mapping();

for (i=0;i<vectormapping.size();i++)
	{
	cout<<"Le noeud "<<i<<" de "<<toMeshName<<" est contenu dans la maille "<<vectormapping[i]<<" de "<<fromMeshName<<endl;	
	}

}
