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

#ifndef MEDMEM_WRAPPER_NODES_HXX
#define MEDMEM_WRAPPER_NODES_HXX

#ifndef NULL
#define NULL 0
#endif

#include <math.h>

//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*                Classe Wrapper_Noeud                   */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Wrapper_Noeud
{
protected :
        double * coordonnees;
public :
        Wrapper_Noeud():coordonnees(NULL) 
                {
                }
        Wrapper_Noeud(double * coord):coordonnees(coord) 
                {
                }
        ~Wrapper_Noeud() 
                {
                }
        void positionne(double *place) 
                {
                coordonnees=place;
                }
        const double & operator[] (int i) const
                {
                return coordonnees[i];
                }
        double operator[] (int i)
                {
                return coordonnees[i];
                }
        friend double DistanceInf(const Wrapper_Noeud<DIMENSION> &A,const Wrapper_Noeud<DIMENSION> &B) 
                {
                double max=0;
                double tmp;
                for (int i=0;i<DIMENSION;i++)
                        {
                        tmp=fabs(A[i]-B[i]);
                        if (tmp>max) max=tmp;
                        }
                return max;
                }
        friend double DistanceL2(const Wrapper_Noeud<DIMENSION> &A,const Wrapper_Noeud<DIMENSION> &B)  
                {
                double tmp,somme=0;
                for (int i=0;i<DIMENSION;i++)
                        {
                        tmp=(A[i]-B[i]);
                        somme+=tmp*tmp;
                        }
                return sqrt(somme);
                }
        friend int operator==(const Wrapper_Noeud<DIMENSION> &A,const Wrapper_Noeud<DIMENSION> &B)
                {
                for (int i=0;i<DIMENSION;i++) if (A[i]!=B[i]) return 1;
                return 0;
                }
        friend std::ostream & operator<<(std::ostream &os,const Wrapper_Noeud<DIMENSION> &A)
                {
                os<<"( "<<std::flush;
                for (int i=0;i<DIMENSION;i++) os<<A[i]<<" "<<std::flush;
                os<<")"<<std::flush;
                return os;
                }
        };

/*********************************************************/
/*                                                       */
/*             Classe Nuage_Wrapper_Noeud                */
/*                                                       */
/*********************************************************/

        
template <int DIMENSION> class Wrapper_Nuage_Noeud
        {
        protected : 
                int nbr_noeuds;
                double * noeuds;
                Wrapper_Noeud<DIMENSION> show;
        public :
                Wrapper_Nuage_Noeud():nbr_noeuds(0),noeuds(NULL) {}
                Wrapper_Nuage_Noeud(int nn, double *n):nbr_noeuds(nn),noeuds(n),show(noeuds) {}
                ~Wrapper_Nuage_Noeud() {}
                Wrapper_Noeud<DIMENSION> & operator [] (int i)
                        {
                        show.positionne((double *) &noeuds[DIMENSION*i]);
                        return show;
                        }
                int size() const {return nbr_noeuds;}
                int SIZE() const {return nbr_noeuds;}
                void affiche()
                        {
                        int i,j;
                        for (i=0;i<nbr_noeuds;i++)
                                {
                                cout<<"Noeud "<<i<<" : "<<flush;
                                for (j=0;j<DIMENSION;j++) cout<<noeuds[i*DIMENSION+j]<<" "<<flush;
                                cout<<endl;
                                }
                        }
        };


#endif
