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
#ifndef WRAPPERS_CELLS_HXX
#define WRAPPERS_CELLS_HXX

#include "stdio.h"
#include "stdlib.h"

#include <typeinfo>

#include <vector>
#include <map>

#ifndef FAUX
#define FAUX 0
#endif

#ifndef VRAI
#define VRAI 1
#endif

#ifndef UNDEFINED
#define UNDEFINED -1
#endif

#include "MEDMEM_define.hxx"

#define MAXNBR 10

//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*      Equivalence num modele local => MED              */
/*                                                       */
/*********************************************************/

#define NBR_MODELES_MAILLES_DEFINIS 15
static int Equivalence_Local_MED[NBR_MODELES_MAILLES_DEFINIS] = { MED_EN::MED_POINT1 ,
                                                           MED_EN::MED_SEG2   , MED_EN::MED_SEG3,
							   MED_EN::MED_TRIA3  , MED_EN::MED_TRIA6   , MED_EN::MED_QUAD4 , MED_EN::MED_QUAD8  ,
							   MED_EN::MED_TETRA4 , MED_EN::MED_TETRA10 , MED_EN::MED_HEXA8 , MED_EN::MED_HEXA20 , MED_EN::MED_PYRA5 , MED_EN::MED_PYRA13 , MED_EN::MED_PENTA6 , MED_EN::MED_PENTA15 };

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Base          */
/*                                                       */
/*********************************************************/

// classe mere des connectivités cannoniques
// N'a aucune méthode virtuelle pour éviter les pertes de temps
// Ce sont les constructeurs des classes dérivées qui renseignent les attributs spécifiques
// un simplexe de base est constitué par un sommet de la maille qui n'est pas contenu dans la face démandée et par trois points de cette face
// Ce simplexe est utilisé dans le calcul des fonctions barycentriques
// LES MAILLES SONT DONC ET PAR CONSEQUENT DES MAILLES A FACES PLANES
// Une face est une face P1, c'est a dire contenant uniquement les sommets, par les noeuds milieux
// Ces faces sont utilisées pour le calcul de connexité


class Connectivite_Canonique_Base
{
protected :
	int                   type          ;
	int                   nbr_noeuds    ;
	int                   nbr_faces     ;
	vector<int>           premier_noeud ;
	vector< vector<int> > simplexe_base ;
	vector< vector<int> > face          ;
public :
	inline int                 DONNE_NBR_NOEUDS()                        const { return nbr_noeuds;              }
	inline int                 DONNE_NBR_FACES()                         const { return nbr_faces;               }
	inline const vector<int> & DONNE_SIMPLEXE_BASE(int num_face)         const { return simplexe_base[num_face]; }
	inline const vector<int> & DONNE_FACE(int num_face)                  const { return face[num_face];          }
	inline int                 DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const { return premier_noeud[num_face]; }
	friend class Wrapper_Maille;
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_*             */
/*                                                       */
/*********************************************************/

// définies dans la partie CODE
// les constructeurs construisent tous les tableaux de connectivités nécessaires en fonction du MODELE MED

//class Connectivite_Canonique_Point1  : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Seg2    : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Seg3    : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Tria3   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Tria6   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Quad4   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Quad8   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Tetra4  : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Tetra10 : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Hexa8   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Hexa20  : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Pyra5   : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Pyra13  : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Penta6  : public Connectivite_Canonique_Base; 
//class Connectivite_Canonique_Penta15 : public Connectivite_Canonique_Base; 

/*********************************************************/
/*                                                       */
/*               Classe Connectivite_Generale            */
/*                                                       */
/*********************************************************/

// Cette classe contient toutes les connectivités canoniques, elle est utilisée dans Wrapper_Nuage_Maille

class Connectivite_Generale
{
protected :
	vector<Connectivite_Canonique_Base *> AllConn;
public :
	Connectivite_Generale();
	~Connectivite_Generale();
	// Renvoie la connectivite locale de la maille de numero local de modele i, dont l'equivalent MED est Equivalence_Local_MED[i]
	Connectivite_Canonique_Base * operator[](int i) const {return AllConn[i];}
};

/*********************************************************/
/*                                                       */
/*                  Structure type_retour                */
/*                                                       */
/*********************************************************/

// c'est une structure de type petit tableau statique, pour accelerer les acces et eviter les allocations dynamiques

struct type_retour
	{
	int quoi[MAXNBR];
	int combien;
	};
	
/*********************************************************/
/*                                                       */
/*           Fonction Comparaison_Informe(...)           */
/*                                                       */
/*********************************************************/

// renvoie vrai si v1 et v2 contiennent la meme chose et sont de meme tailles, faux sinon

inline int Comparaison_Informe(const type_retour &v1,const type_retour &v2);

/*********************************************************/
/*                                                       */
/*               Classe Wrapper_Maille                   */
/*                                                       */
/*********************************************************/

// c'est le wrapper maille sur int[]
// cette classe n'est pas dérivée, tous les types de mailles sont stockées sous cette forme
// la variable qui définit les type est la Connectivité_Canonique_Base, qui est toujours polymorphée en un type spécifique


class Wrapper_Maille
{
protected :

	// la référence du premier sommet
	int * sommets;	
	// la connectivité canonique, toujours polymorphée
	Connectivite_Canonique_Base * modele;
	
public :
	Wrapper_Maille():sommets(NULL) {}
	~Wrapper_Maille() {}
	
	// sorte de transtypeur, cette méthode prend une référence vers un premier sommet et un modele canonique et renvoie *this en tant que wrapper sur ces données	
	inline const Wrapper_Maille & positionne(int * pos, Connectivite_Canonique_Base * mod){sommets=pos;modele=mod;return *this;}
	// méthodes de la politique (les numéros renvoyés sont des numéros GLOBAUX, calculés à partir de l'indirection fournie par la connectivité canonique
	// par contre les numéros de face fournis sont des numéros locaux
	// renvoie le numéro global du sommet de numéro local i
	inline int operator[](int i) const {return sommets[i];} 
	inline int  DONNE_NBR_NOEUDS()                               const;
	inline int  DONNE_NBR_FACES()                                const;
	inline void DONNE_SIMPLEXE_BASE(int num_face,type_retour & simplexe) const;
	inline void DONNE_FACE(int num_face,type_retour & face)              const;
	inline int  DONNE_PREMIER_NOEUD_DE_FACE(int num_face)        const;
	// pour une face, donne, s'il existe, le numéro local de face équivalente, -1 sinon
	inline int  DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(const type_retour & sommets_face) const;
	// donne le numéro local de modele
	inline int  DONNE_TYPE_MAILLE()     const; 
	// donne le numéro MED de modele
	inline int  DONNE_TYPE_MED_MAILLE() const; 
};


/*********************************************************/
/*                                                       */
/*            Classe Wrapper_Nuage_Maille                */
/*                                                       */
/*********************************************************/

// Classe de Wrapping sur un nuage de maille donné sous forme SKYLINE 
// voir la classe Wrapper_Med_Connectivity dans MEDMEM_Wrapper_Connectivity.hxx pour la politique de classe

template <class FORME_SKYLINE> class Wrapper_Nuage_Maille
{
protected :
	// pointeur sur une forme skyline
	FORME_SKYLINE * mailles;
	// toutes les connectivités canoniques
	Connectivite_Generale ConnGen;
	int nbr_mailles;
	// ATTENTION, c'est le type en numero local de modele, pour éviter une map, le numéro de modele MED correponsdant est donné par Equivalence_Local_MED
	vector<int> types;
	// pointeur dans mailles du premier sommet de chaque maille (évite les calculs du au nombres éventuellement différents de sommets par maille)
	vector< int * > premier_pointeur;
	// maille_courante, est un Wrapper_Maille déja instancié utilisé par l'opérateur [] pour accelerer les acces
	Wrapper_Maille maille_courante;
public :
	Wrapper_Nuage_Maille():mailles(NULL) {}
	// le constructeur renseigne types et premier_pointeur, instantie ConnGenn et positionne maille_courante sur la premiere maille
	Wrapper_Nuage_Maille(FORME_SKYLINE * fs);
	~Wrapper_Nuage_Maille() {}
	// Méthodes de la politique
	// positionne maille_courante sur la maille de numéro global i et renvoie maille_courante
	inline const Wrapper_Maille & operator[](int i);
	inline int         SIZE() {return nbr_mailles;}
	void affiche();
};

//////////////////////////////////////////////////////////////////
///                                                            ///
///                            CODE                            ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*           Fonction Comparaison_Informe(...)           */
/*                                                       */
/*********************************************************/

// effectue le test (v1 et v2 ont meme taille)&&(chaque élément de v1 est dans v2)
// c'est une égalité forte si on est sur que v1 et v2 n'ont pas de doublets, 
// ce qui est le cas pour les mailles et les simplexes

int Comparaison_Informe(const type_retour &v1,const type_retour &v2)
	{
	int t1=v1.combien;
	int t2=v2.combien;
	if (t1!=t2) return FAUX;
	int i1,i2;
	int test;
	for (i1=0;i1<t1;i1++)
		{
		test=1;
		for (i2=0;(i2<t2)&&(test);i2++)
			{
			if (v1.quoi[i1]==v2.quoi[i2]) test=0;
			}
		if (test) return FAUX;
		}
	return VRAI;
	}

/*********************************************************/
/*                                                       */
/*               Classe Wrapper_Maille                   */
/*                                                       */
/*********************************************************/

inline int  Wrapper_Maille::DONNE_NBR_NOEUDS()                                       const
	{
	return modele->DONNE_NBR_NOEUDS();
	}
inline int  Wrapper_Maille::DONNE_NBR_FACES()                                        const
	{
	return modele->DONNE_NBR_FACES();
	}
inline void Wrapper_Maille::DONNE_SIMPLEXE_BASE(int num_face,type_retour & simplexe) const
	{
	const vector<int> & simplexelocal=modele->DONNE_SIMPLEXE_BASE(num_face);
	simplexe.combien=simplexelocal.size();
	for (int i=0;i<simplexe.combien;i++) simplexe.quoi[i]=sommets[simplexelocal[i]];
	}
inline void Wrapper_Maille::DONNE_FACE(int num_face,type_retour & face)               const
	{
	const vector<int> & facelocal=modele->DONNE_FACE(num_face);
	face.combien=facelocal.size();
	for (int i=0;i<face.combien;i++) face.quoi[i]=sommets[facelocal[i]];
	}
inline int  Wrapper_Maille::DONNE_PREMIER_NOEUD_DE_FACE(int num_face)                const
	{
	return sommets[modele->DONNE_PREMIER_NOEUD_DE_FACE(num_face)];
	}
inline int  Wrapper_Maille::DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(const type_retour & sommets_face) const
	{
	type_retour face_loc;
	int num_face;
	for (num_face=0;num_face<DONNE_NBR_FACES();num_face++)
		{
		DONNE_FACE(num_face,face_loc);
		if (Comparaison_Informe(face_loc,sommets_face)) return num_face;
		}
	return UNDEFINED;
	}
inline int  Wrapper_Maille::DONNE_TYPE_MAILLE()     const
	{
	return modele->type;
	}
inline int  Wrapper_Maille::DONNE_TYPE_MED_MAILLE() const 
	{
	return Equivalence_Local_MED[modele->type];
	}

	
/*********************************************************/
/*                                                       */
/*            Classe Wrapper_Nuage_Maille                */
/*                                                       */
/*********************************************************/	
	
template <class FORME_SKYLINE>	Wrapper_Nuage_Maille<FORME_SKYLINE>::Wrapper_Nuage_Maille(FORME_SKYLINE * fs):mailles(fs)
		{
		int i;
		map<int,int> Equivalence_MED_Local;
		// calcule la map de convertion des types med en numéro local, pour accelerer l'acces
		for (i=0;i<NBR_MODELES_MAILLES_DEFINIS;i++) Equivalence_MED_Local[Equivalence_Local_MED[i]]=i;
		nbr_mailles=mailles->SIZE();
		types.resize(nbr_mailles);
		premier_pointeur.resize(nbr_mailles);
		for (i=0;i<nbr_mailles;i++) 
			{
			types[i]=Equivalence_MED_Local[mailles->DONNE_TYPE_MAILLE(i)];
			premier_pointeur[i]=mailles->DONNE_PREMIER_POINTEUR(i);
			}
		maille_courante.positionne(premier_pointeur[0],ConnGen[types[0]]);
		}
template <class FORME_SKYLINE> const Wrapper_Maille & Wrapper_Nuage_Maille<FORME_SKYLINE>::operator[](int i)
		{
		return maille_courante.positionne(premier_pointeur[i],ConnGen[types[i]]);
		}
template <class FORME_SKYLINE> void Wrapper_Nuage_Maille<FORME_SKYLINE>::affiche()
		{
		int i,j;
		for (i=0;i<nbr_mailles;i++)
			{
			cout<<"Maille "<<i<<" MED "<<Equivalence_Local_MED[types[i]]<<" : "<<flush;
			for (j=0;j<(*this)[i].DONNE_NBR_NOEUDS();j++) cout<<(*this)[i][j]<<" "<<flush;
			cout<<endl;
			}		
		}

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Point1        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Point1  : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Point1()
		{
		
		type=0;
		
		nbr_noeuds = 1;
		nbr_faces = 0;		
		
		premier_noeud = vector<int>(0);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		}
	~Connectivite_Canonique_Point1() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Seg2          */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Seg2    : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Seg2()
		{
		
		type=1;
		
		int pn[2] = {0,1};
		int sb[4] = {1,0,0,1};
		int fa[2] = {0,1};
		int po[3] = {0,1,2};
		
		nbr_noeuds = 2;
		nbr_faces = 2;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[2*i],&sb[2*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Seg2() {}
};
/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Seg3          */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Seg3    : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Seg3()
		{
		
		type=2;
		
		int pn[2] = {0,1};
		int sb[4] = {1,0,0,1};
		int fa[2] = {0,1};
		int po[3] = {0,1,2};
		
		nbr_noeuds = 3;
		nbr_faces = 2;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[2*i],&sb[2*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Seg3() {}
};


/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Tria3         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Tria3 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Tria3()
		{
		
		type=3;
		
		int pn[3] = {0,1,2};
		int sb[9] = {2,0,1,0,1,2,1,2,0};
		int fa[6] = {0,1,1,2,2,0};
		int po[4] = {0,2,4,6};
		nbr_noeuds = 3;
		nbr_faces = 3;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[3*i],&sb[3*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Tria3() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Tria6         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Tria6 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Tria6()
		{
		
		type=4;
		
		int pn[3] = {0,1,2};
		int sb[9] = {2,0,1,0,1,2,1,2,0};
		int fa[6] = {0,1,1,2,2,0};
		int po[4] = {0,2,4,6};
		nbr_noeuds = 6;
		nbr_faces = 3;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[3*i],&sb[3*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Tria6() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Quad4         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Quad4 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Quad4()
		{
		
		type=5;
		
		int pn[4] = {0,1,2,3};
		int sb[12] = {2,0,1,3,1,2,0,2,3,1,0,2};
		int fa[8] = {0,1,1,2,2,3,3,0};
		int po[5] = {0,2,4,6,8};
		nbr_noeuds = 4;
		nbr_faces = 4;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[3*i],&sb[3*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Quad4() {}
};


/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Quad8         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Quad8 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Quad8()
		{
		
		type=6;
		
		int pn[4] = {0,1,2,3};
		int sb[12] = {2,0,1,3,1,2,0,2,3,1,0,2};
		int fa[8] = {0,1,1,2,2,3,3,0};
		int po[5] = {0,2,4,6,8};
		nbr_noeuds = 8;
		nbr_faces = 4;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[3*i],&sb[3*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Quad8() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Tetra4        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Tetra4 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Tetra4()
		{
		
		type=7;
		
		int pn[4] = {0,0,1,2};
		int sb[16] = {3,0,1,2,2,0,3,1,0,1,3,2,1,2,3,0};
		int fa[12] = {0,1,2,0,3,1,1,3,2,2,3,0};
		int po[5] = {0,3,6,9,12};
		nbr_noeuds = 4;
		nbr_faces = 4;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Tetra4() {}
};

/*********************************************************/
/*                                                       */
/*          Classe Connectivite_Canonique_Tetra10        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Tetra10 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Tetra10()
		{
		
		type=8;
		
		int pn[4] = {0,0,1,2};
		int sb[16] = {3,0,1,2,2,0,3,1,0,1,3,2,1,2,3,0};
		int fa[12] = {  0,1,2,  0,3,1,  1,3,2,  2,3,0};
		int po[5] = {0,3,6,9,12};
		
		nbr_noeuds = 10;
		nbr_faces = 4;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Tetra10() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Hexa8         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Hexa8 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Hexa8()
		{
		
		type=9;
		
		int pn[6] = {0,4,0,1,2,3};
		int sb[24] = {5,0,1,2,0,7,6,5,3,0,4,5,4,1,5,6,1,2,6,3,2,3,7,0};
		int fa[24] = {0,1,2,3,4,5,6,7,0,4,5,1,1,5,6,2,2,6,7,3,3,7,4,0};
		int po[7] = {0,4,8,12,16,20,24};
		nbr_noeuds = 8;
		nbr_faces = 6;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Hexa8() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Hexa20        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Hexa20 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Hexa20()
		{
		
		type=10;
		
		int pn[6] = {0,4,0,1,2,3};
		int sb[24] = {5,0,1,2,0,7,6,5,3,0,4,5,4,1,5,6,1,2,6,3,2,3,7,0};
		int fa[24] = {0,1,2,3,4,5,6,7,0,4,5,1,1,5,6,2,2,6,7,3,3,7,4,0};
		int po[7] = {0,4,8,12,16,20,24};
		nbr_noeuds = 20;
		nbr_faces = 6;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Hexa20() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Pyra5         */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Pyra5 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Pyra5()
		{
		
		type=11;
		
		int pn[5] = {0,0,1,2,3};
		int sb[20] = {4,0,1,2,3,0,4,1,0,1,4,2,1,2,4,3,2,0,3,4};
		int fa[16] = {0,1,2,3,0,4,1,1,4,2,2,4,3,3,4,0};
		int po[6] = {0,4,7,10,13,16};
		nbr_noeuds = 5;
		nbr_faces = 5;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Pyra5() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Pyra13        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Pyra13 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Pyra13()
		{
		
		type=12;
		
		int pn[5] = {0,0,1,2,3};
		int sb[20] = {4,0,1,2,3,0,4,1,0,1,4,2,1,2,4,3,2,0,3,4};
		int fa[16] = {0,1,2,3,0,4,1,1,4,2,2,4,3,3,4,0};
		int po[6] = {0,4,7,10,13,16};
		nbr_noeuds = 13;
		nbr_faces = 5;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Pyra13() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Penta6        */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Penta6 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Penta6()
		{
		
		type=13;
		
		int pn[5] = {0,3,0,1,2};
		int sb[20] = {3,0,1,2,2,3,4,5,5,0,3,1,0,1,4,2,1,2,5,0};
		int fa[18] = {0,1,2,3,4,5,0,3,4,1,1,4,5,2,2,5,3,1};
		int po[6] = {0,3,6,10,14,18};
		nbr_noeuds = 6;
		nbr_faces = 5;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Penta6() {}
};

/*********************************************************/
/*                                                       */
/*           Classe Connectivite_Canonique_Penta15       */
/*                                                       */
/*********************************************************/

class Connectivite_Canonique_Penta15 : public Connectivite_Canonique_Base
{
public :
	Connectivite_Canonique_Penta15()
		{
		
		type=14;
		
		int pn[5] = {0,3,0,1,2};
		int sb[20] = {3,0,1,2,2,3,4,5,5,0,3,1,0,1,4,2,1,2,5,0};
		int fa[18] = {0,1,2,3,4,5,0,3,4,1,1,4,5,2,2,5,3,1};
		int po[6] = {0,3,6,10,14,18};
		nbr_noeuds = 15;
		nbr_faces = 5;
		
		
		premier_noeud = vector<int>(&pn[0],&pn[nbr_faces]);
		
		simplexe_base = vector< vector<int> >(nbr_faces);
		face	      = vector< vector<int> >(nbr_faces);
		
		int i;
		
		for (i=0;i<nbr_faces;i++) simplexe_base[i]=vector<int>(&sb[4*i],&sb[4*(i+1)]);
		for (i=0;i<nbr_faces;i++) face[i]=vector<int>(&fa[po[i]],&fa[po[i+1]]);
		
		}
	~Connectivite_Canonique_Penta15() {}
};

/*********************************************************/
/*                                                       */
/*               Classe Connectivite_Generale            */
/*                                                       */
/*********************************************************/

inline Connectivite_Generale::Connectivite_Generale():AllConn(NBR_MODELES_MAILLES_DEFINIS)
	{
	// l'ordre est important, il dépend de la relation entre numéro local et modele et numéro MED
	AllConn[ 0]=new Connectivite_Canonique_Point1  ();
	AllConn[ 1]=new Connectivite_Canonique_Seg2    ();
	AllConn[ 2]=new Connectivite_Canonique_Seg3    ();
	AllConn[ 3]=new Connectivite_Canonique_Tria3   ();
	AllConn[ 4]=new Connectivite_Canonique_Tria6   ();
	AllConn[ 5]=new Connectivite_Canonique_Quad4   ();	    
	AllConn[ 6]=new Connectivite_Canonique_Quad8   ();	    
	AllConn[ 7]=new Connectivite_Canonique_Tetra4  ();	    
	AllConn[ 8]=new Connectivite_Canonique_Tetra10 ();	    
	AllConn[ 9]=new Connectivite_Canonique_Hexa8   ();
	AllConn[10]=new Connectivite_Canonique_Hexa20  ();
	AllConn[11]=new Connectivite_Canonique_Pyra5   ();	    
	AllConn[12]=new Connectivite_Canonique_Pyra13  ();	    
	AllConn[13]=new Connectivite_Canonique_Penta6  ();	    
	AllConn[14]=new Connectivite_Canonique_Penta15 ();	    
	}
inline Connectivite_Generale::~Connectivite_Generale() 
	{
	for (int i=0;i<AllConn.size();i++) delete AllConn[i];
	}


#endif
