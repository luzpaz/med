#include "stdio.h"
#include "stdlib.h"

#include <iostream.h>

#include "MEDMEM_InterpolationHighLevelObjects.hxx"

#ifdef _DEBUG_
#include "LocalTraceCollector.hxx"
#endif /* ifdef _DEBUG_*/

using namespace MEDMEM;
using namespace MED_EN;

int main (void) 
{
#ifdef _DEBUG_
  LocalTraceCollector::instance();
#endif /* ifdef _DEBUG_*/

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

