#ifndef MEDMEM_WRAPPER_MESH_HXX
#define MEDMEM_WRAPPER_MESH_HXX

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

template <class NUAGEMAILLE> class Wrapper_Maillage
{
protected : 
	NUAGEMAILLE * mailles;
	
	int nbr_noeuds;
	
	vector< vector<int> > faces_contenues;
	vector< vector<int> > mailles_contenant_noeud;
	vector< vector<int> > voisins_de_maille;
	
	vector<int> face_au_bord;
	vector<int> maille_au_bord;

	// Méthode privée
	void Construit_Contenant_Noeud();
	
public :

	Wrapper_Maillage():mailles(NULL) {}
	// Construit les Connectivités du maillage à la construction
	Wrapper_Maillage(NUAGEMAILLE * fs, int nn);
	~Wrapper_Maillage() {}
	
	// Méthodes de la politique
	inline int DONNE_NBR_FACES_MAILLE(int num_maille); 
	inline int DONNE_VOISIN_DE_MAILLE(int num_maille,int num_face) const;
	inline int EST_AU_BORD_FACE_DE_MAILLE(int num_maille,int num_face) const; 
	inline int DONNE_NBR_FACES(int num_maille) const;
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_maille,int num_face) const;
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
	return mailles->DONNE_NBR_FACES(num_maille);
	}
template <class NUAGEMAILLE> vector<int> Wrapper_Maillage<NUAGEMAILLE>::DONNE_SIMPLEXE_BASE(int num_maille,int num_face) const
	{
	return mailles->DONNE_SIMPLEXE_BASE(num_maille,num_face);
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
		
	for (num_maille=0;num_maille<mailles->SIZE();num_maille++)
		{
		nbr_noeuds_maille=mailles->DONNE_NBR_NOEUDS(num_maille);
		for (num_noeud=0;num_noeud<nbr_noeuds_maille;num_noeud++) 
			{
			num=mailles->DONNE_SOMMET_MAILLE(num_maille,num_noeud);
// 			if ((num<0)||(num>=mailles_contenant_noeud.size()))
// 				{
// 				cerr<<endl;
// 				cerr<<"Outbound call dans Construit Contenant Noeud"<<endl;
// 				cerr<<"Inférior bound = 0"<<endl;
// 				cerr<<"Supérior bound = "<<mailles_contenant_noeud.size()-1<<endl;
// 				cerr<<"Call = "<<num<<endl;
// 				exit(-1);
// 				}
			mailles_contenant_noeud[num].push_back(num_maille);
			}
		}	

	}
template <class NUAGEMAILLE> Wrapper_Maillage<NUAGEMAILLE>::Wrapper_Maillage(NUAGEMAILLE * fs,int nn)
	{
	
	cout<<"Calcul de la Connectivité du Maillage"<<endl;
	
	if (fs) mailles=fs;
	else
		{
		cerr<<"Wrapper_Maillage : Nuage mailles vide passé en argument"<<endl;
		exit(-1);
		}
		
	int i,j,k;
	int num_local_face;
	int ind_num_noeud,num_noeud;
	int num_maille;
	int ind_num_maille_sec,num_maille_sec;
	int num_local_sec;
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
	
	vector<int> sommets_face;
	
	cout<<"Calcul du lien Sommet->Maille"<<endl;
	
	Construit_Contenant_Noeud();
	
	for (num_maille=0;num_maille<nbr_mailles;num_maille++)
		{
		tmp=mailles->DONNE_NBR_FACES(num_maille);
		voisins_de_maille[num_maille]=vector<int>(tmp,UNDEFINED);
		faces_contenues[num_maille]=vector<int>(tmp,UNDEFINED);
		approx_nbr_formants+=tmp;
		}
		
	face_au_bord.reserve(approx_nbr_formants/10);
	
	cout<<"Calcul des voisins"<<endl;
	
	int flag_interm;
	for (num_maille=0;num_maille<nbr_mailles;num_maille++)
		{
		for (num_local_face=0;num_local_face<mailles->DONNE_NBR_FACES(num_maille);num_local_face++)
			{
			num_noeud=mailles->DONNE_PREMIER_NOEUD_DE_FACE(num_maille,num_local_face);
			flag_existence=0;
			sommets_face=mailles->DONNE_FACE(num_maille,num_local_face);
			flag_interm=0;
			for (ind_num_maille_sec=0;(flag_existence==0)&&(ind_num_maille_sec<mailles_contenant_noeud[num_noeud].size());ind_num_maille_sec++)
				{
				num_maille_sec=mailles_contenant_noeud[num_noeud][ind_num_maille_sec];
				if (num_maille_sec>num_maille)
					{
					flag_interm=1;
					num_loc=mailles->DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(num_maille_sec,sommets_face);
					if (num_loc>UNDEFINED)
						{
						cout<<"La maille "<<num_maille<<" a pour voisin la maille "<<num_maille_sec<<" via la face "<<nbr_formants<<endl;
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
		
	// Construction de la connexité des mailles de bord
	
	cout<<"Calcul des bords"<<endl;

	int ind_num_cont,test_bord,nbr_faces_bord=0;
	for (num_maille=0;num_maille<nbr_mailles;num_maille++) 
		{
		test_bord=0;
		for (ind_num_cont=0;ind_num_cont<faces_contenues[num_maille].size();ind_num_cont++)
			{
			if (faces_contenues[num_maille][ind_num_cont]==UNDEFINED)
				{
				cout<<"La maille "<<num_maille<<" est au bord via sa face "<<ind_num_cont<<endl;
				test_bord=1;
				faces_contenues[num_maille][ind_num_cont]=nbr_formants;
				maille_au_bord[num_maille]=VRAI;
				face_au_bord.push_back(VRAI);
				nbr_faces_bord++;
				nbr_formants++;
				}
			}
		if (test_bord==0)
			{
			maille_au_bord[num_maille]=FAUX;
			}
		}
	
	// Vérification
	
	cout<<"Vérification de la connectivité"<<endl;
	
	int verif=0;
	int nf,nbf=0;
	for (i=0;i<nbr_mailles;i++) 
		{
		nf=0;
		for (j=0;j<faces_contenues[i].size();j++) 
			{
			if (faces_contenues[i][j]==UNDEFINED) verif++;
			if (voisins_de_maille[i][j]==UNDEFINED) nf++;
			}
		if (maille_au_bord[i]==UNDEFINED) cout<<"Maille "<<i<<" non completement construite"<<endl;
		if (nf==faces_contenues[i].size()) nbf++;
		}
		
	
	cout<<" IL Y A "<<verif<<" PROBLEMES A LA SUITE DE LA CONSTRUCTION DE CONNEXITE"<<endl;
	cout<<"Nombre de mailles : "<<nbr_mailles<<endl;
	cout<<"Approximation du nombre de faces : "<<approx_nbr_formants<<endl;
	cout<<"Nombre réel de faces de bord : "<<nbr_faces_bord<<endl;
	cout<<"Nombre réel total de faces : "<<nbr_formants<<endl;
	cout<<"Nombre de Mailles Isolées : "<<nbf<<endl;	
	}

#endif
