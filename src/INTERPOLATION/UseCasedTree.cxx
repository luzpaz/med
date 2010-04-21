//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "stdio.h"
#include "stdlib.h"

#include <iostream>
#include <list>
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
        std::cout<<"Noeud["<<i<<"] = ";
        tmp1=&noeuds[DIMENSION*i];
        affiche(tmp1);
        std::cout<<std::endl;
        }

Meta_dTree<DIMENSION> Octree(NBR_NOEUDS,noeuds);

for (i=0;i<NBR_INC;i++)
        {
        std::cout<<"Noeud le plus proche de ";
        tmp1=&noeuds_inconnus[DIMENSION*i];
        affiche(tmp1);
        std::cout<<" : ";
        tmp2=&noeuds[DIMENSION*Octree.trouve_plus_proche_point(tmp1)];
        affiche(tmp2);
        std::cout<<" ; Vérification par méthode bourrin : ";
        tmp2=&noeuds[DIMENSION*Octree.trouve_plus_proche_point_bourrin(tmp1)];
        affiche(tmp2);
        std::cout<<std::endl;
        }

 for (i=0;i<NBR_INC;i++)
 {
   std::cout<<"****************"<<std::endl <<"Nodes close to ";
   tmp1=&noeuds_inconnus[DIMENSION*i];
   affiche(tmp1);
   std::cout << std::endl;
   for ( double toler = 0.3; toler < 3.5; toler+=0.5 )
   {
     std::list<int> close;
     int nb = Octree.get_all_close( tmp1, toler, close );
     std::cout << "With tolerance " << toler << " - " << nb << std::endl;
     std::list<int>::iterator n = close.begin();
     while ( n != close.end() ) {
       int node = *n++;
       tmp2=&noeuds[DIMENSION*node];
       std::cout<<"\t"<< node << " : ";
        affiche(tmp2);
        double dist = 0;
        for ( int dim = 0; dim < DIMENSION; ++dim )
          dist += ( tmp1[dim] - tmp2[dim] ) * ( tmp1[dim] - tmp2[dim] );
        std::cout << "\t distance: " << sqrt( dist ) << std::endl;
     }
     if ( nb == NBR_NOEUDS )
       break; // no sens in increase toler as all nodes found
   }
 }
}

