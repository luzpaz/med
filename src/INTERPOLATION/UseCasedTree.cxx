#include "stdio.h"
#include "stdlib.h"

#include <iostream.h>

#include "MEDMEM_InterpolationHighLevelObjects.hxx"

#define affiche(NOEUD) cout<<flush;for (int iii=0;iii<DIMENSION;iii++) cout<<NOEUD[iii]<<" "<<flush;

int main (void) 
{
int i;

const int DIMENSION  = 3;
const int NBR_NOEUDS = 8;
const int NBR_INC    = 3;

double noeuds[DIMENSION*NBR_NOEUDS] = { 0,0,0,
					1,0,0,
					0,1,0,
					0,0,1,
					1,1,0,
					1,0,1,
					0,1,1,
					1,1,1 };
					
double noeuds_inconnus[DIMENSION*NBR_INC] =   { 2  ,2  ,2  ,
						0.2,0.2,0.2,
						0  ,0.9,0.9 };

double * tmp1, * tmp2;

for (i=0;i<NBR_NOEUDS;i++)
	{
	cout<<"Noeud["<<i<<"] = ";
	tmp1=&noeuds[DIMENSION*i];
	affiche(tmp1);
	cout<<endl;
	}

Meta_dTree<DIMENSION> Octree(NBR_NOEUDS,noeuds);

for (i=0;i<NBR_INC;i++)
	{
	cout<<"Noeud le plus proche de ";
	tmp1=&noeuds_inconnus[DIMENSION*i];
	affiche(tmp1);
	cout<<" : ";
	tmp2=&noeuds[DIMENSION*Octree.trouve_plus_proche_point(tmp1)];
	affiche(tmp2);
	cout<<" ; Vérification par méthode bourrin : ";
	tmp2=&noeuds[DIMENSION*Octree.trouve_plus_proche_point_bourrin(tmp1)];
	affiche(tmp2);
	cout<<endl;
	}

}

