//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef MEDMEM_DTREE_HXX
#define MEDMEM_DTREE_HXX

#include "MEDMEM_dTreeSommet.hxx"
#include <list>

enum { 
  DTREE_FANTOME  = -1,
  DTREE_RACINE   =  0,
  DTREE_TERMINAL =  1,
  DTREE_COURANT  =  2
};

// etat_descendance
enum {
  DTREE_NON_CALCULE = -1,
  DTREE_AUCUNE      =  0,
  DTREE_VALIDE      =  1
};

// val min nbr noeud
const int DTREE_NBR_MIN_NOEUDS = 2;
const int DTREE_NBR_MAX_DESC   = 8;
// pour meilleu re lecture
#define _TEMPLATE_ \
template <class NOEUD,class NUAGENOEUD,int DIMENSION,int NBR_NOEUDS_PAR_CASE, int MAX_DEPTH> 
#define _DTREE_ dTree<NOEUD,NUAGENOEUD,DIMENSION,NBR_NOEUDS_PAR_CASE,MAX_DEPTH>

//////////////////////////////////////////////////////////////////
///                                                            ///
///                       DECLARATIONS                         ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*             Calcul Statique de Puissance              */
/*                                                       */
/*********************************************************/

// Permet de Calculer 2^n de façon statique

template < int DIMENSION > struct DeuxPuissance
        {
        enum { valeur = 2 * DeuxPuissance<DIMENSION-1>::valeur } ;
        };

template <> struct DeuxPuissance<0>
        {
        enum { valeur = 1  } ;
        };      

/*********************************************************/
/*                                                       */
/*                         DTREE                         */
/*                                                       */
/*********************************************************/

// Construit un d-Tree sur un nuage de noeud et pour un noeud donné donne lequel des noeuds du nuage est le plus proche

// # Bugs connus : 
//      -  Problemes avec points sur les faces des hypercubes ?
//      -  Plantage sauvage si le point est plus loin de l'hypercube père que le diametre Linf de celui-ci.
// # Politique de classe :
//      - NOEUD : voir dans MEDMEM_WrapperNodes.hxx la classe Wrapper_Noeud<..>
//      - NUAGENOEUD : typiquement, c'est vector<NOEUD> en rédefinissant NUAGENOEUD<...>::SIZE()=vector<...>::size()
// Remarques :
//      - NBR_NOEUDS_PAR_CASE ne doit pas être modifié sauf peut-être dans le cas où l'utilisateur veut utiliser des d-Tree parallèles 
//        ou utilise des nuages de noeud trop grands

template <class NOEUD,class NUAGENOEUD,int DIMENSION,int NBR_NOEUDS_PAR_CASE=DTREE_NBR_MIN_NOEUDS, int MAX_DEPTH=DTREE_NBR_MAX_DESC>
class dTree
{
protected :
        // types
        typedef _DTREE_ * Ptr_dTree;
        // Static Const
        static const int nbr_descendants=DeuxPuissance<DIMENSION>::valeur;
        // champs
        NUAGENOEUD * nuage;
        Ptr_dTree descendant[nbr_descendants];
        // numéro des noeuds contenus
        std::vector<int> * noeud_contenu;
        int etat;
        int niveau;
        dTree * pere;
        // extrémités de l'hypercube du dTree
        Sommet_dTree<DIMENSION> coord_max;
        Sommet_dTree<DIMENSION> coord_min;
public :

        void init();
        dTree();
        // Ce constructeur est le seul constructeur utilisateur, il construit le père puis tous les descendants
        dTree(NUAGENOEUD *n);
        // Ce Constructeur est utilisé par le précédent, pour un pere donné, avec deux extrémités données, 
        // il instantie un dTree sans en calculer les noeuds contenus
        dTree(const Sommet_dTree<DIMENSION> &A,const Sommet_dTree<DIMENSION> &B,dTree *mypere);
        dTree(const dTree &F);
        ~dTree();
        // Renvoie les numéros de noeuds contenus dans le dTree
        void Get_Noeuds_Filtre(std::vector<int> &tmp);
        // renvoie les extrémités
        Sommet_dTree<DIMENSION> Get_Max() const;
        Sommet_dTree<DIMENSION> Get_Min() const;
        // renvoie vrai si P est dans le dTree
        int is_in_dTree(NOEUD P) const;
        // calcule la distance topologique locale de P au dTree
        double calcule_distance(NOEUD P) const;
        dTree & operator = (const dTree & F);
        // retourne le sommet du dTree codé par l'entier selecteur (voir explications dans le code)
        Sommet_dTree<DIMENSION> donne_sommet(int selecteur) const;
        int a_des_fils() const;
        // renvoi une reference sur le dTree terminal descendant de this contenant P
        dTree * trouve_dTree_contenant(NOEUD P) const;
        // renvoie le point le plus proche de P dans this par la méthode exhaustive brutale
        int trouve_plus_proche_point_bourrin(NOEUD P) const;
        // renvoie le point le plus proche de p dans this par la méthode dtree
        int trouve_plus_proche_point(NOEUD P) const;
        // renvoie un numéro de point contenu dans this
        int trouve_un_point() const;
        // méthode récursive utilisée par trouve_plus_proche_point
        int tppp_rec(NOEUD P,double &delta,int &flag) const;
        // dit si P est d-proche de l'hypercube de this
        int Localise_Point(NOEUD P,double d) const;
        // utilisé par le constructeur pour créer tout la filiation du dTree
        void cree_filiation();
        // méthodes de mesure
        int Get_Nbr_Descendants_Non_Vides() const;
        int Get_Nbr_Descendants_Vides() const;
        int Get_Profondeur_Max() const;

        // return numbers of nodes close to P within tolerance d
        int get_all_close(NOEUD P, double d, std::list<int> & closeNumbers) const;
};


//////////////////////////////////////////////////////////////////
///                                                            ///
///                           CODE                             ///
///                                                            ///
//////////////////////////////////////////////////////////////////

_TEMPLATE_ void _DTREE_::init()
        {
        int i;
        nuage=NULL;
        noeud_contenu=NULL;
        etat=DTREE_FANTOME;
        for (i=0;i<nbr_descendants;i++) descendant[i]=NULL;
        niveau=-1;
        pere=NULL;
        }
_TEMPLATE_ _DTREE_::dTree()
        {
        init();
        coord_max=0.0;
        coord_min=0.0;
        }
_TEMPLATE_ _DTREE_::dTree(NUAGENOEUD *n)
        {
        int i,j;
        double tmp;
        
        if (n==NULL) 
                {
                cerr<<"DTREE : Nuage vide !"<<endl;
                exit(-1);
                }
        
        init();
        nuage=n;
        etat=DTREE_RACINE;
        noeud_contenu=new std::vector<int>(nuage->size());
        niveau=0;
        
        // calcule les extrémités du dTree pere
        for (i=0;i<DIMENSION;i++)
                {
                coord_max[i]=(*nuage)[0][i];
                coord_min[i]=(*nuage)[0][i];
                }
        (*noeud_contenu)[0]=0;
        for (i=1;i<nuage->size();i++)
                {
                (*noeud_contenu)[i]=i;
                for (j=0;j<DIMENSION;j++) 
                        {
                        if ((*nuage)[i][j]>coord_max[j]) coord_max[j]=(*nuage)[i][j];
                        if ((*nuage)[i][j]<coord_min[j]) coord_min[j]=(*nuage)[i][j];
                        }
                }
        for (j=0;j<DIMENSION;j++) 
                {
                tmp=(coord_max[j]-coord_min[j])*0.01;
                coord_max[j]+=tmp;
                coord_min[j]-=tmp;
                }
        
        // méthode récursive qui construit la filiation
        cree_filiation();
        
        }
_TEMPLATE_ _DTREE_::dTree(const Sommet_dTree<DIMENSION> &A,const Sommet_dTree<DIMENSION> &B,dTree *mypere)
        {
        if (mypere!=NULL)
                {
                
                int i;
                
                init();
                
                pere=mypere;
                nuage=pere->nuage;
                niveau=pere->niveau+1;
                
                etat=DTREE_COURANT;
                
                noeud_contenu=new std::vector<int>;
                noeud_contenu->reserve((pere->noeud_contenu->size())/nbr_descendants);
                
                for (i=0;i<DIMENSION;i++)
                        {
                        coord_max[i]=(*nuage)[0][i];
                        coord_min[i]=(*nuage)[0][i];
                        }
                
                for (i=0;i<DIMENSION;i++)
                        {
                        if (A[i]>B[i]) 
                                {
                                coord_max[i]=A[i];
                                coord_min[i]=B[i];
                                }
                        else
                                {
                                coord_min[i]=A[i];
                                coord_max[i]=B[i];
                                }
                        }
                }
        else
                {
                cerr<<"DTREE : Construction de descendance sans père !"<<endl;
                exit(-1);
                }
        }       
_TEMPLATE_ _DTREE_::dTree(const _DTREE_ &F)
        {
        // Pas Super Top Moumoute ... Recopie de pointeurs et pas de contenus, merdique
        int i,j;
        init();
        for (i=0;i<nbr_descendants;i++) descendant[i]=F.descendant[i];
        noeud_contenu=F.noeud_contenu;
        etat=F.etat;
        niveau=F.niveau;
        pere=F.pere;
        coord_max=F.coord_max;
        coord_min=F.coord_min;
        }
_TEMPLATE_ _DTREE_::~dTree()
        {
        int i;
        nuage=NULL;
        pere=NULL;
        // cout<<*this<<endl;
        if (noeud_contenu) 
                {
                delete noeud_contenu;
                }
        for (i=0;i<nbr_descendants;i++) if (descendant[i]) 
                {
                delete descendant[i];
                }
        }
_TEMPLATE_ void _DTREE_::Get_Noeuds_Filtre(std::vector<int> &tmp)
        {
        int i;
        switch (etat)
                {
                case DTREE_RACINE : int pourlefunlecompilolesttropcon;
                case DTREE_COURANT : 
                        for (i=0;i<nbr_descendants;i++) descendant[i]->Get_Noeuds_Filtre(tmp);
                case DTREE_TERMINAL : 
                        if (noeud_contenu->size()>0) 
                                {
                                for (i=0;i<NBR_NOEUDS_PAR_CASE;i++) tmp.push_back((*noeud_contenu)[i]);
                                }
                default : 
                        cerr<<"DTREE : dTree Non Valide dans Rempli_Noeuds_Filtre"<<endl;
                        exit(-1);
                }
        }
_TEMPLATE_ Sommet_dTree<DIMENSION> _DTREE_::Get_Max() const
        {
        return coord_max;
        }
_TEMPLATE_ Sommet_dTree<DIMENSION> _DTREE_::Get_Min() const
        {
        return coord_min;
        }
_TEMPLATE_ int _DTREE_::is_in_dTree(NOEUD P) const
        {
        int test;
        for (int i=0;i<DIMENSION;i++)
                {
                test=((coord_min[i]<=P[i])&&(P[i]<=coord_max[i])); 
                if (!test) return 0;
                }
        return 1;

        }
// calcule la distance Linf d'un point exterieur, si le point est interieur, le resultat sera erroné
_TEMPLATE_ double _DTREE_::calcule_distance(NOEUD P) const
        {
        double min=fabs(coord_max[0]-P[0]);
        double tmp;
        int i;
        for (i=0;i<DIMENSION;i++)
                {
                tmp=fabs(coord_max[i]-P[i]);
                if (min>tmp) min=tmp;
                tmp=fabs(coord_min[i]-P[i]);
                if (min>tmp) min=tmp;
                }
        return min;     
        }
_TEMPLATE_ _DTREE_ & _DTREE_::operator = (const _DTREE_ & F)
        {
        // Pas Super Top Moumoute ... Recopie de pointeurs et pas de contenus, merdique
        int i,j;
        init();
        for (i=0;i<_DTREE_::nbr_descendans;i++) descendant[i]=F.descendant[i];
        noeud_contenu=F.noeud_contenu;
        etat=F.etat;
        niveau=F.niveau;
        pere=F.pere;
        coord_max=F.coord_max;
        coord_min=F.coord_min;
        }
// selecteur doit etre lu comme un nombre en base 2
// il specifie les coordonnes de reference du sommet dont on veut les coordonnees reelles
// ex : en dim 3 : coord_min=0+0*2+0*4=0
//                 coord_max=1+1*2*1*4=7
// donc les unites pour x, les 2aines pour y, les 4aines pour z
_TEMPLATE_ Sommet_dTree<DIMENSION> _DTREE_::donne_sommet(int selecteur) const
        {
        Sommet_dTree<DIMENSION> p;
        int i;
        int residu=selecteur;
        int reste;
        for (i=0;i<DIMENSION;i++) 
                {
                reste=residu%2;
                if (reste==0) p[i]=coord_min[i]; else p[i]=coord_max[i];
                residu=(residu-reste)/2;
                }
        return p;
        }
_TEMPLATE_ int _DTREE_::a_des_fils() const
        {
        if ((etat==DTREE_COURANT)||(etat=DTREE_RACINE)) return 1;
        else return 0;
        }
_TEMPLATE_ _DTREE_ * _DTREE_::trouve_dTree_contenant(NOEUD P) const
        {
        Sommet_dTree<DIMENSION> B(coord_min,coord_max);
        int i;
        if ((etat==DTREE_RACINE)&&(!is_in_dTree(P))) return NULL; // Le noeud est extérieur a l'dTree
        else if (etat==DTREE_TERMINAL) return (dTree *) this;     // Le noeud est dans *this qui est terminal
        
        for (i=0;i<nbr_descendants;i++)
                {
                Sommet_dTree<DIMENSION> A=donne_sommet(i);
                int test,j;
                for (j=0;j<DIMENSION;j++)
                        {
                        if (A[j]<=B[j]) test=((A[j]<=P[j])&&(P[j]<=B[j])); 
                        else test=((A[j]>=P[j])&&(P[j]>=B[j]));
                        if (!test) break;
                        }

                if (test)
                  return descendant[i]->trouve_dTree_contenant(P); // Propagation
                }
        return NULL;
        }
// si de le dTree n'est pas TERMINAL, scanne tous les points du nuage du pere pour trouver le point le plus proche
// sinon scanne uniquement les points contenus dans le dTree
_TEMPLATE_ int _DTREE_::trouve_plus_proche_point_bourrin(NOEUD P) const
        {
        int i;
        int num_sol=0;
        double min;
        double tmp;
        if (etat!=DTREE_TERMINAL)
                {
                min=DistanceL2(P,(*nuage)[0]);
                for (i=1;i<nuage->size();i++)
                        {
                        tmp=DistanceL2(P,(*nuage)[i]);
                        if (tmp<min)
                                {
                                num_sol=i;
                                min=tmp;
                                }
                        }
                return num_sol;
                }
        else
                {
                if (noeud_contenu->size()!=0)
                        {
                        num_sol=(*noeud_contenu)[0];
                        min=DistanceL2(P,(*nuage)[num_sol]);
                        for (i=1;i<(int)noeud_contenu->size();i++)
                                {
                                tmp=DistanceL2(P,(*nuage)[(*noeud_contenu)[i]]);
                                if (tmp<min)
                                        {
                                        num_sol=(*noeud_contenu)[i];
                                        min=tmp;
                                        }
                                }
                        return num_sol;
                        }
                else return DTREE_FANTOME;
                }
        }
// Fonction de pilotage de la recursion, lance les tests preliminaires et invoque la recursion
_TEMPLATE_ int _DTREE_::trouve_plus_proche_point(NOEUD P) const
        {

        if (etat==DTREE_FANTOME) 
                {
                cerr<<"DTREE : TROUVE PLUS PROCHE POINT : L'octree n'est pas construit !"<<endl;
                exit(-1);
                }
        dTree *ref=trouve_dTree_contenant(P);   
        double delta;

        if (ref!=NULL)
                {
                // Le point est intérieur

                if (ref->pere!=NULL)    
                        {
                        delta=DistanceInf((ref->pere->coord_max),(ref->pere->coord_min));
                        }       
                else 
                        {
                        cerr<<"DTREE : TROUVE PLUS PROCHE POINT : l'octree contenant le noeud n'a pas de pere !"<<endl;
                        exit(-1);
                        }
                }
        else 
                {
                // Le point est extérieur

                delta=DistanceL2(P,(*nuage)[0]);
                }
                
        int flag=0;
        int retour;

        retour=tppp_rec(P,delta,flag);

        return retour;
        }
_TEMPLATE_ int _DTREE_::trouve_un_point() const
        {
        if (noeud_contenu!=NULL)
                {
                if (noeud_contenu->size()>0) return (*(noeud_contenu))[0];
                }
        else
                {
                int i;
                for (i=0;i<nbr_descendants;i++)
                        {
                        return descendant[i]->trouve_un_point();
                        }
                }
        }
// partie recursive de trouve_plus_proche_point
// change le flag en 1 si un point plus proche est trouvé
// adapte automatiquement la distance delta de filtrage
_TEMPLATE_ int _DTREE_::tppp_rec(NOEUD P,double &delta,int &flag) const
        {
        if (Localise_Point(P,delta)==0) 
                {
        
        // La distance du point à l'octree est plus grande que delta
        
                return DTREE_FANTOME;
                }
        int num_Ptmp;
        double dtmp;
        if (etat==DTREE_TERMINAL)
                {
                if (noeud_contenu->size()>0)
                        {
                        num_Ptmp=trouve_plus_proche_point_bourrin(P);
                        dtmp=DistanceL2((*nuage)[num_Ptmp],P);
                        if (dtmp<=delta) 
                                {
        
        // Le point le plus proche minimise delta, c'est un bon candidat, on l'envoie !
        
                                delta=dtmp;
                                flag=1;
                                return num_Ptmp;
                                }
        
        // Le point le plus proche ne minimise pas delta
        
                        // ===========> peut etre rajouter exit(-1); ??????????
                        return DTREE_FANTOME;
                        }
                else 
                        {
        
        // L'octree qui contient P ne contient aucun point
        
                        return DTREE_FANTOME;
                        }
                }
        int i;
        int num_sol=DTREE_FANTOME;
        for (i=0;i<nbr_descendants;i++)
                {
                num_Ptmp=descendant[i]->tppp_rec(P,delta,flag);
                if ((num_Ptmp!=DTREE_FANTOME)&&(flag==1)) 
                        {
        
        // On a trouvé un point qui minimise delta dans une branche
        
                        num_sol=num_Ptmp;
                        }
                }
        // A ce stade, on a trouvé un point qui minimise tous les deltas, c'est donc le point le plus proche
        // REMARQUE : cette affirmation est à la base de l'algorithme par dTree mais est loin d'étre évidente
        
        return num_sol;
        }
        
// renvoie 1 si la distance L inf du noeud a l'octree est plus petite que d, 0 sinon
_TEMPLATE_ int _DTREE_::Localise_Point(NOEUD P,double d) const
        {
        int i;
        for (i=0;i<DIMENSION;i++)
                {
                if (P[i]>coord_max[i]+d) return 0;
                if (P[i]<coord_min[i]-d) return 0;
                }
        return 1;
        }
        
// Méthode de construction du dTree par propagation
_TEMPLATE_ void _DTREE_::cree_filiation()
        {
        if (etat!=DTREE_RACINE) 
                {
                niveau=pere->niveau+1;
                }
        else 
                {
                niveau=0;
                }
        
        if (noeud_contenu->size()<=NBR_NOEUDS_PAR_CASE ||
            niveau > MAX_DEPTH) // badly needed for the case with coincident nodes
                {
                etat=DTREE_TERMINAL;
                }
        else
                {
                int i,num_loc,test;
                
                Sommet_dTree<DIMENSION> centre(coord_max,coord_min);
                
                for (i=0;i<nbr_descendants;i++)
                        {
                        descendant[i]=new dTree(centre,donne_sommet(i),this);
                        }
                
                for (num_loc=0;num_loc<(int)noeud_contenu->size();num_loc++)
                        {
                        int indice=(*noeud_contenu)[num_loc];
                        NOEUD & courant=(*nuage)[indice];
                        test=1;
                        for (i=0;(test)&&(i<nbr_descendants);i++)
                                {
                                if (descendant[i]->is_in_dTree(courant))
                                        {
                                        descendant[i]->noeud_contenu->push_back(indice);
                                        test=0;
                                        }
                                }
                        }
                
                delete noeud_contenu;
                noeud_contenu=NULL;
                
                for (i=0;i<nbr_descendants;i++) descendant[i]->cree_filiation();
                }
        }
_TEMPLATE_ int _DTREE_::Get_Nbr_Descendants_Non_Vides() const
        {
        int i;
        int ndnv=0;
        switch (etat)
                {
                case DTREE_RACINE : int pourlefunlecompilolesttropcon;
                case DTREE_COURANT : 
                        for (i=0;i<nbr_descendants;i++) ndnv+=descendant[i]->Get_Nbr_Descendants_Non_Vides();
                        return ndnv;
                case DTREE_TERMINAL : 
                        if (noeud_contenu->size()>0) return 1;
                        else return 0;
                default : 
                        cerr<<"dTree Non Valide dans Get_Nbr_Descendants_Non_Vides"<<endl;
                        return -1;
                }
        }
_TEMPLATE_ int _DTREE_::Get_Nbr_Descendants_Vides() const
        {
        int i;
        int ndnv=0;
        switch (etat)
                {
                case DTREE_RACINE : int pourlefunlecompilolesttropcon;
                case DTREE_COURANT : 
                        for (i=0;i<nbr_descendants;i++) ndnv+=descendant[i]->Get_Nbr_Descendants_Vides();
                        return ndnv;
                case DTREE_TERMINAL : 
                        if (noeud_contenu->size()==0) return 1;
                        else return 0;
                default : 
                        cerr<<"dTree Non Valide dans Get_Nbr_Descendants_Non_Vides"<<endl;
                        return -1;
                }
        }
_TEMPLATE_ int _DTREE_::Get_Profondeur_Max() const
        {
        int i;
        int prof=0;
        int tmp;
        switch (etat)
                {
                case DTREE_RACINE : int pourlefunlecompilolesttropcon;
                case DTREE_COURANT : 
                        for (i=0;i<nbr_descendants;i++) 
                                {
                                tmp=descendant[i]->Get_Profondeur_Max();
                                if (tmp>prof) prof=tmp;
                                }
                        return prof;
                case DTREE_TERMINAL : return niveau;
                default : 
                        cerr<<"dTree Non Valide dans Get_Nbr_Descendants_Non_Vides"<<endl;
                        return -1;
                }
        }

// return numbers of nodes close to P within tolerance d
_TEMPLATE_ int _DTREE_::get_all_close(NOEUD P, double d, std::list<int> & closeNumbers) const
{
  int i, nbAdded = 0;
  if (Localise_Point(P,d))
  {
    if (etat==DTREE_TERMINAL)
    {
      int nb = noeud_contenu->size();
      for (i=0;i<nb;i++)
      {
        double dist=DistanceL2(P,(*nuage)[(*noeud_contenu)[i]]);
        if (dist < d )
        {
          nbAdded++;
          closeNumbers.push_back( (*noeud_contenu)[i] );
        }
      }
    }
    else
    {
      for (i=0;i<nbr_descendants;i++)
      {
        nbAdded += descendant[i]->get_all_close(P,d,closeNumbers);
      }
    }
  }
  return nbAdded;
}

#undef _TEMPLATE_
#undef _DTREE_


#endif
