//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#ifndef MEDMEM_MAPPING_HXX
#define MEDMEM_MAPPING_HXX

#include "MEDMEM_MappingTools.hxx"
#include "MEDMEM_dTree.hxx"

#define NBR_MAX_MAILLES_EXAMINEES 100

#ifndef  NBR_FACES_MAX
#define NBR_FACES_MAX 6
#endif

#define _TEMPLATE_ template <class MAILLAGE, class NUAGEMAILLE, class NUAGENOEUD, class NOEUD, int DIMENSION>
#define _MAPPING_ Mapping<MAILLAGE,NUAGEMAILLE,NUAGENOEUD,NOEUD,DIMENSION>


//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*                   Classe  Mapping                     */
/*                                                       */
/*********************************************************/

// ATTENTION LE NUAGE DE NOEUD EST SUPPOSE NON REDONDANT ET AUCUNE VERIFICATION N'EST FAITE !

template <class MAILLAGE, class NUAGEMAILLE, class NUAGENOEUD, class NOEUD, int DIMENSION> class Mapping
{
protected :
	MAILLAGE * maillage_back;
	NUAGEMAILLE * mailles_back;
	NUAGENOEUD * noeuds_back;
	NUAGENOEUD * noeuds_front;
	Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,DIMENSION> * CB;
	dTree<NOEUD,NUAGENOEUD,DIMENSION> * my_dTree;
	vector<int> resultat_mapping;
	vector<int> point_le_plus_proche;
	
public :
	Mapping():maillage_back(NULL),mailles_back(NULL),noeuds_back(NULL),noeuds_front(NULL),CB(NULL),my_dTree(NULL) {}
	Mapping(MAILLAGE * mb,NUAGENOEUD * nb,NUAGENOEUD * nf); // le dTree est crée à l'initialisation, par contre, le mapping lui meme doit etre invoqué
	~Mapping() {if (CB) delete CB;if (my_dTree) delete my_dTree;}
        dTree<NOEUD,NUAGENOEUD,DIMENSION> * Donne_dTree() {return my_dTree;}
	enum { INTERIEUR = 1, EXTERIEUR_AU_MILIEU = -1, EXTERIEUR_AU_BORD = -2 };
	int Donne_Directions(int num_maille,const NOEUD &n,int etat_face[NBR_FACES_MAX]);
	// Méthode interne de localisation
	int Trouve_Maille_Contenant_Point_Mth_Co(const NOEUD &n,int num_maille,int num_maille_interdit,int max_loop,int &nbr_mailles_examinees,int flag_convexe);
	void Cree_Mapping(int flag_convexe=0);                                             // SUPPOSE NON CONVEXE PAR DEFAUT
	void Cree_Mapping(NUAGENOEUD * nf, int flag_convexe=0);                            // SUPPOSE NON CONVEXE PAR DEFAUT
	inline int operator[](int i) const {return resultat_mapping[i];}                   // Renvoie la valeur mappé, si le mapping a été fait, sinon, n'importe quoi
	inline vector<int> & Get_Mapping() {return resultat_mapping;}                        // Renvoie le vector contenant le mapping
	inline int Get_Noeud_Le_Plus_Proche(int i) const {return point_le_plus_proche[i];} // Invoque la méthode de d-Tree qui donne le noeud le plus proche
	inline int Exist_dTree() const {return (my_dTree);}                                // Teste si le dTree existe
	void affiche()
		{
		for (int i=0;i<resultat_mapping.size();i++) cout<<"Noeud "<<i<<" de noeud plus proche "<<point_le_plus_proche[i]<<" mappé dans maille "<<resultat_mapping[i]<<endl;		
		}
};

//////////////////////////////////////////////////////////////////
///                                                            ///
///                            CODE                            ///
///                                                            ///
//////////////////////////////////////////////////////////////////

_TEMPLATE_ void _MAPPING_::Cree_Mapping(NUAGENOEUD * nf, int flag_convexe)
	{
	noeuds_front=nf;
	Cree_Mapping(flag_convexe);
	}

_TEMPLATE_ void _MAPPING_::Cree_Mapping(int flag_convexe)
	{
	
	if (resultat_mapping.size()==0) 
		{
		if (!noeuds_front) 
			 {
			 cerr<<"Mapping : Pas de noeuds à mapper !"<<endl;
			 exit(-1);
			 }
		 
		int i;
		int nbr_noeuds=noeuds_front->SIZE();
		int num_maille_depart;
		int nma=0;
		resultat_mapping     = vector<int>(nbr_noeuds,MED_UNDEFINED);
		point_le_plus_proche = vector<int>(nbr_noeuds,MED_UNDEFINED);
	
		// noeuds_back->affiche();
		
		for (i=0;i<nbr_noeuds;i++)
			{
			point_le_plus_proche[i]=my_dTree->trouve_plus_proche_point((*noeuds_front)[i]);
			num_maille_depart=maillage_back->DONNE_PREMIERE_MAILLE_CONTENANT(point_le_plus_proche[i]);
			resultat_mapping[i]=Trouve_Maille_Contenant_Point_Mth_Co((*noeuds_front)[i],num_maille_depart,num_maille_depart,NBR_MAX_MAILLES_EXAMINEES,nma,flag_convexe);
			}
		}
		
	else
		{
		cout<<"Le mapping semble déja existé, interrogation sur l'existant"<<endl;
		}
		
	}
_TEMPLATE_ _MAPPING_::Mapping(MAILLAGE * mb,NUAGENOEUD * nb,NUAGENOEUD * nf):maillage_back(mb),noeuds_back(nb),noeuds_front(nf),my_dTree(NULL)
	{
	
	if (!maillage_back)
		{
		cerr<<"Mapping : constructeur appelé avec Maillage Vide"<<endl;
		exit(-1);
		}
		
	if (!noeuds_back)
		{
		cerr<<"Mapping : constructeur appelé avec Nuage Noeuds Vide"<<endl;
		exit(-1);
		}
		
	mailles_back=maillage_back->DONNE_POINTEUR_NUAGEMAILLE();
	
	CB=new Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,DIMENSION>(mailles_back,noeuds_back);

	// TEST REDONDANCE
	/*
	int nnb=noeuds_back->SIZE();
	if (nnb<20000) 
		{
		cout<<"MAPPING : VERIFICATION REDONDANCES DANS NUAGE NOEUD BACK"<<endl;
		noeuds_back->affiche();
		int i,j;		
		vector<int> redondance(nnb,0);
		for (i=0;i<nnb;i++) 
			{
			for (j=i+1;j<nnb;j++) if ((*noeuds_back)[i]==(*noeuds_back)[j]) 
				{
				cerr<<"Le Noeud "<<j<<" est identique au Noeud "<<i<<endl;
				exit(-1);
				}
			}
		cout<<"FIN TEST VERIFICATION REDONDANCES"<<endl;
		}
	// FIN TEST */
	
	my_dTree=new dTree<NOEUD,NUAGENOEUD,DIMENSION>(noeuds_back);

	}
// Renvoie :
//     1 si le point est intérieur
//    -1 si le point est extérieur à la maille via uniquement des faces qui ne sont pas au bord
//    -2 si le point est extérieur à la maille par au moins une face de bord
// Et modifie etat_face de telle sorte que :
// etat_face[i] = -1 s'il n'existe pas de voisin via la face i
// etat_face[i] =  0 si le point est intérieur via la face i et que le voisin i existe
// etat_face[i] =  1 si le point est extérieur via la face i et que le voisin i existe
_TEMPLATE_ int _MAPPING_::Donne_Directions(int num_maille,const NOEUD &n,int etat_face[NBR_FACES_MAX])
	{
	vector<double> ef=CB->Donne_Pseudo_Coord_Baryc(num_maille,n);
	int etat_int=VRAI;
	int etat_ext_bord=FAUX;
	int tf,tv,tb;
	int nbr_faces=(*mailles_back)[num_maille].DONNE_NBR_FACES();
	for (int i=0;i<nbr_faces;i++)
		{
		tf=(ef[i]<0);
		tv=(maillage_back->DONNE_VOISIN_DE_MAILLE(num_maille,i)==MED_UNDEFINED);
		tb=(maillage_back->EST_AU_BORD_FACE_DE_MAILLE(num_maille,i));
		if (tf) // extérieur
			{
			etat_int=FAUX;
			if (tb) etat_ext_bord=VRAI;
			}
		if (tv) etat_face[i]=-1; // ya pas de voisin
		else
			{
			if (tf) etat_face[i]=1;
			else etat_face[i]=0;
			}
		}
	if (etat_int) return INTERIEUR;
	if (etat_ext_bord) return EXTERIEUR_AU_BORD;
	return EXTERIEUR_AU_MILIEU;
	}
_TEMPLATE_ int _MAPPING_::Trouve_Maille_Contenant_Point_Mth_Co(const NOEUD &n,int num_maille,int num_maille_interdit,int max_loop,int &nbr_mailles_examinees,int flag_convexe)
	{

	int etat_face[NBR_FACES_MAX];
	int i,tmp,nbr_rnd;
	int indirection[NBR_FACES_MAX];
	int ind_reel;
	int num_reel;
	int new_num=MED_UNDEFINED;
	
	int test=Donne_Directions(num_maille,n,etat_face);
	
	int nbr_faces=maillage_back->DONNE_NBR_FACES_MAILLE(num_maille);
	
	if ( test != INTERIEUR ) { // EAP, for PAL11458
          // check neighbors
          int etat_face_for_check[NBR_FACES_MAX];
          for (i=0;i<nbr_faces;i++) {
            int num_neighbor=maillage_back->DONNE_VOISIN_DE_MAILLE(num_maille,i);
            if ( num_neighbor != MED_UNDEFINED &&
                 Donne_Directions(num_neighbor,n,etat_face_for_check) == INTERIEUR )
              return num_neighbor;
            indirection[i]=i;
          }
	}
	
	nbr_mailles_examinees=0;
	
	while (nbr_mailles_examinees<max_loop)
		{
		if (test==INTERIEUR) 
			{
			return num_maille;
			}
		if ((test==EXTERIEUR_AU_BORD)&&(flag_convexe)) 
			{
			return 2*MED_UNDEFINED;
			}
		nbr_mailles_examinees++;
		for (i=0;i<nbr_faces;i++)
			{
			tmp=indirection[i];
			nbr_rnd=rand()%nbr_faces;
			indirection[i]=indirection[nbr_rnd];
			indirection[nbr_rnd]=tmp;
			}
		for (i=0;(i<nbr_faces)&&(new_num==MED_UNDEFINED);i++) 
			{
			ind_reel=indirection[i];
			num_reel=maillage_back->DONNE_VOISIN_DE_MAILLE(num_maille,ind_reel);
			if ((etat_face[ind_reel]==1)&&(num_reel!=num_maille_interdit)) 
				{
				new_num=num_reel;
				}
			}
		for (i=0;(i<nbr_faces)&&(new_num==MED_UNDEFINED);i++) 
			{
			ind_reel=indirection[i];
			num_reel=maillage_back->DONNE_VOISIN_DE_MAILLE(num_maille,ind_reel);
			if ((etat_face[ind_reel]==0)&&(num_reel!=num_maille_interdit)) 
				{
				new_num=num_reel;
				}
			}
		if (new_num==MED_UNDEFINED) 
			{
			new_num=num_maille_interdit;
			}
		num_maille_interdit=num_maille;
		num_maille=new_num;
		new_num=MED_UNDEFINED;
		test=Donne_Directions(num_maille,n,etat_face);
		}
	return MED_UNDEFINED;
	}

#undef _TEMPLATE_
#undef _MAPPING_

#endif
