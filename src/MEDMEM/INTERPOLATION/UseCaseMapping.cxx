#include "stdio.h"
#include "stdlib.h"

#include <iostream.h>

#include "MEDMEM_InterpolationHighLevelObjects.hxx"

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
