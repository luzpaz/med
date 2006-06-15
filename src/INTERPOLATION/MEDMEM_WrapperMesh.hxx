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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef MEDMEM_WRAPPER_MESH_HXX
#define MEDMEM_WRAPPER_MESH_HXX

#include "MEDMEM_WrapperCells.hxx"

#include "stdio.h"
#include "stdlib.h"

#include <vector>

#ifndef UNDEFINED
#define UNDEFINED -1
#endif

#ifndef FAUX
#define FAUX 0
#endif

#ifndef VRAI
#define VRAI 1
#endif

//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*                   Wrapper_Maillage                    */
/*                                                       */
/*********************************************************/

// cette classe est � la fois un wrapper sur un nuage de maille et une classe d'algorithme
// elle s'occupe de construire les liens de connexit�s minimums du nuage de maille
// pour le transformer en maillage suffisament riche pour etre utilis� par les algorithmes de connexit�s
// et autres m�thodes n�cessitant des informations de connexit� sur un maillage

// la version utilis�e dans MEDMEMOIRE est d�-templatifi�e dans MEDMEM_InterpolationHighLevelObject.hxx

template <class NUAGEMAILLE> class Wrapper_Maillage
{
protected : 
	// r�f�rence vers le nuage de maille, 
	// voir la classe Wrapper_Nuage_Maille dans MEDMEM_WrapperCells.hxx pour la politique
	NUAGEMAILLE * mailles;
	
	int nbr_noeuds;
	
	// liste des num�ros globaux de faces contenues dans une maille
	vector< vector<int> > faces_contenues;
	// liste des num�ros globaux de mailles qui contiennent un noeud
	vector< vector<int> > mailles_contenant_noeud;
	// liste des num�ros globaux de mailles voisines d'une maille donn�e via une face
	// l'ordre du voisin dans la liste implique par quelle face dans le tableau faces_contenues il est voisin
	vector< vector<int> > voisins_de_maille;
	
	// liste des num�ros globaux de faces qui sont au bord du maillage
	// Ce sont les faces qui n'ont qu'une seule maille de rattachement
	vector<int> face_au_bord;
	// liste des num�ros globaux de mailles qui sont au bord
	// ce sont les mailles qui ont une face sans voisin
	vector<int> maille_au_bord;

	// M�thode priv�e
	// construit le tableau mailles_contenant_noeud
	void Construit_Contenant_Noeud();
	
public :

	Wrapper_Maillage():mailles(NULL) {}
	// Construit les Connectivit�s du maillage � la construction
	Wrapper_Maillage(NUAGEMAILLE * fs, int nn);
	~Wrapper_Maillage() {}
	
	// M�thodes de la politique
	inline int DONNE_NBR_FACES_MAILLE(int num_maille); 
	inline int DONNE_VOISIN_DE_MAILLE(int num_maille,int num_face) const;
	inline int EST_AU_BORD_FACE_DE_MAILLE(int num_maille,int num_face) const; 
	inline int DONNE_NBR_FACES(int num_maille) const;
	inline int DONNE_PREMIERE_MAILLE_CONTENANT(int num_noeud) const;
	inline NUAGEMAILLE * DONNE_POINTEUR_NUAGEMAILLE();

};

//////////////////////////////////////////////////////////////////
///                                                            ///
///                            CODE                            ///
///                                                            ///
//////////////////////////////////////////////////////////////////

template <class NUAGEMAILLE> int Wrapper_Maillage<NUAGEMAILLE>::DONNE_PREMIERE_MAILLE_CONTENANT(int num_noeud) const
	{
	return mailles_contenant_noeud[num_noeud][0];
	}
template <class NUAGEMAILLE> int Wrapper_Maillage<NUAGEMAILLE>::DONNE_VOISIN_DE_MAILLE(int num_maille,int num_face) const
	{
	return voisins_de_maille[num_maille][num_face];
	}
template <class NUAGEMAILLE> int Wrapper_Maillage<NUAGEMAILLE>::EST_AU_BORD_FACE_DE_MAILLE(int num_maille,int num_face) const
	{
	return face_au_bord[faces_contenues[num_maille][num_face]];
	}
template <class NUAGEMAILLE> int Wrapper_Maillage<NUAGEMAILLE>::DONNE_NBR_FACES_MAILLE(int num_maille)
	{
	return (*mailles)[num_maille].DONNE_NBR_FACES();
	}
template <class NUAGEMAILLE> NUAGEMAILLE * Wrapper_Maillage<NUAGEMAILLE>::DONNE_POINTEUR_NUAGEMAILLE()
	{
	return mailles;
	}
template <class NUAGEMAILLE> void Wrapper_Maillage<NUAGEMAILLE>::Construit_Contenant_Noeud()
	{
	int nbr_noeuds_maille;
	int num,num_noeud,num_maille;
	
	mailles_contenant_noeud.resize(nbr_noeuds);
	
	// parcours le tableau des mailles, puis les sommets de chaque maille
	// et utilise un push_back pour renseigner mailles_contenant_noeud
		
	for (num_maille=0;num_maille<mailles->SIZE();num_maille++)
		{
		nbr_noeuds_maille=(*mailles)[num_maille].DONNE_NBR_NOEUDS();
		for (num_noeud=0;num_noeud<nbr_noeuds_maille;num_noeud++) 
			{
			num=(*mailles)[num_maille][num_noeud];
			mailles_contenant_noeud[num].push_back(num_maille);
			}
		}	

	}
template <class NUAGEMAILLE> Wrapper_Maillage<NUAGEMAILLE>::Wrapper_Maillage(NUAGEMAILLE * fs,int nn)
	{
	
	if (fs) mailles=fs;
	else
		{
		cerr<<"Wrapper_Maillage : Nuage mailles vide pass� en argument"<<endl;
		exit(-1);
		}
		
	int i,j;
	int num_local_face;
	int num_noeud;
	int num_maille;
	int ind_num_maille_sec,num_maille_sec;
	int flag_existence;
	int nbr_mailles=mailles->SIZE();
	int nbr_formants=0;
	int approx_nbr_formants=0;
	int tmp;
	int num_loc;
	
	nbr_noeuds=nn;
	
	voisins_de_maille.resize(nbr_mailles);
	faces_contenues.resize(nbr_mailles);
	maille_au_bord.resize(nbr_mailles,UNDEFINED);
	
	type_retour sommets_face;
	
	Construit_Contenant_Noeud();
	
	// mise a taille des tableaux et calcul d'une approximation du nombre de faces
	// on postule que le nombre de faces r�el est le dixieme de la somme du nombre de faces par maille sur toutes les mailles
	// on calcule cette approximation pour �viter les allocations fr�quentes dues aux push_back pour des petits tableaux
	
	for (num_maille=0;num_maille<nbr_mailles;num_maille++)
		{
		tmp=(*mailles)[num_maille].DONNE_NBR_FACES();
		voisins_de_maille[num_maille]=vector<int>(tmp,UNDEFINED);
		faces_contenues[num_maille]=vector<int>(tmp,UNDEFINED);
		approx_nbr_formants+=tmp;
		}
		
	face_au_bord.reserve(approx_nbr_formants/10);
	
	// algorithme principal
	
	// REMARQUE : les faces sont num�rot�es mais ne sont pas construites ni stock�es
	
	int flag_interm;
	
	// on parcourt en premier lieu le nuage de maille (lin�aire, en Nombre de Maille)
	
	for (num_maille=0;num_maille<nbr_mailles;num_maille++)
		{
		
	// pour chaque maille, dite primaire, on parcourt ensuite ses faces (born�, par 8)
	
		for (num_local_face=0;num_local_face<(*mailles)[num_maille].DONNE_NBR_FACES();num_local_face++)
			{
			num_noeud=(*mailles)[num_maille].DONNE_PREMIER_NOEUD_DE_FACE(num_local_face);
			flag_existence=0;
			(*mailles)[num_maille].DONNE_FACE(num_local_face,sommets_face);
			flag_interm=0;
			
	// pour chaque face, dite primaire, on prend le premier noeud, et on parcourt les mailles qui contiennent ce noeud tant qu'on n'a pas trouv� de voisin
	// (born�, par un coefficient qui d�pend de l'anisotropie du maillage, le nombre maximum de maille qui contient un sommet)
			
			for (ind_num_maille_sec=0;(flag_existence==0)&&(ind_num_maille_sec<(int)mailles_contenant_noeud[num_noeud].size());ind_num_maille_sec++)
				{
				num_maille_sec=mailles_contenant_noeud[num_noeud][ind_num_maille_sec];
				
	// on teste ensuite si cette maille secondaire a un num�ro plus �lev� que la maille primaire, dans le cas contraire, 
	// �a veut dire qu'elle a d�ja �t� trait�e ou sera trait�e ult�rieurement
				
				if (num_maille_sec>num_maille)
					{
					flag_interm=1;
					
	// pour cette maille secondaire on regarde si elle contient la face primaire
	// (born�, par 8*4=32)			
					
					num_loc=(*mailles)[num_maille_sec].DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(sommets_face);					
					if (num_loc>UNDEFINED)
						{
						
	// et dans ce cas, la maille secondaire est voisine de la maille primaire, on met � jour les tableaux
	// si on voulait construire le tableau des faces, c'est ici qu'il faudrait le faire -1-
						
                                                  // MESSAGE("La maille "<<num_maille<<" a pour voisin la maille "<<num_maille_sec<<" via la face "<<nbr_formants);
						face_au_bord.push_back(FAUX);
						faces_contenues[num_maille][num_local_face]=nbr_formants;
						voisins_de_maille[num_maille][num_local_face]=num_maille_sec;
						faces_contenues[num_maille_sec][num_loc]=nbr_formants;
						voisins_de_maille[num_maille_sec][num_loc]=num_maille;
						flag_existence=1;
						nbr_formants++;
						}
					}
				}
			}
		}
		
	// Construction de la connexit� des mailles de bord
	// A ce stade, on n'a que la connexit� du voisinage des mailles, et les faces de bord n'ont pas �t� num�rot�es
	
	int ind_num_cont,test_bord,nbr_faces_bord=0;
	
	// on parcourt les mailles
	
	for (num_maille=0;num_maille<nbr_mailles;num_maille++) 
		{
		test_bord=0;
		
	// on examine les faces de cette maille dans la num�rotation globale faces_contenues
		
		for (ind_num_cont=0;ind_num_cont<(int)faces_contenues[num_maille].size();ind_num_cont++)
			{
			
	// On regarde si tous les num�ros globaux des faces sont d�finis
			
			if (faces_contenues[num_maille][ind_num_cont]==UNDEFINED)
				{
				
	// si un seul num�ro n'est pas d�fini, la maille est au bord
	// si on voulait construire le tableau des faces, c'est ici qu'il faudrait le faire -2-
				
                                  // MESSAGE("La maille "<<num_maille<<" est au bord via sa face "<<ind_num_cont);
				test_bord=1;
				faces_contenues[num_maille][ind_num_cont]=nbr_formants;
				maille_au_bord[num_maille]=VRAI;
				face_au_bord.push_back(VRAI);
				nbr_faces_bord++;
				nbr_formants++;
				}
			}
			
	// dans le cas contraire, tous les num�ros sont d�finis, la maille n'est pas au bord
			
		if (test_bord==0)
			{
			maille_au_bord[num_maille]=FAUX;
			}
		}
	
	// V�rification de la connectivit�
	// on regarde si tous les num�ros globaux sont d�finis
	// si ce n'est pas le cas, c'est que le nuage de maille est mal d�fini

	
	int verif=0;
	int nf,nbf=0;
	for (i=0;i<nbr_mailles;i++) 
		{
		nf=0;
		for (j=0;j<(int)faces_contenues[i].size();j++) 
			{
			if (faces_contenues[i][j]==UNDEFINED) verif++;
			if (voisins_de_maille[i][j]==UNDEFINED) nf++;
			}
		if (maille_au_bord[i]==UNDEFINED) cerr<<"Maille "<<i<<" non completement construite"<<endl;
		if (nf==faces_contenues[i].size()) nbf++;
		}
		
	
	MESSAGE("IL Y A "<<verif<<" PROBLEMES A LA SUITE DE LA CONSTRUCTION DE CONNEXITE");
	MESSAGE("Nombre de mailles : "<<nbr_mailles);
	MESSAGE("Approximation du nombre de faces : "<<approx_nbr_formants);
	MESSAGE("Nombre r�el de faces de bord : "<<nbr_faces_bord);
	MESSAGE("Nombre r�el total de faces : "<<nbr_formants);
	MESSAGE("Nombre de Mailles Isol�es : "<<nbf);	
	}

#endif
