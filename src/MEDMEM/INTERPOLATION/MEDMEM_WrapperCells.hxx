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

//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*           Fonction Comparaison_Informe(...)           */
/*                                                       */
/*********************************************************/

int Comparaison_Informe(const vector<int> &v1,const vector<int> &v2);


/*********************************************************/
/*                                                       */
/*               Classe Wrapper_Maille                   */
/*                                                       */
/*********************************************************/

// REMARQUE : A REFAIRE COMPLETEMENT !

class Wrapper_Maille
{
protected :
	int * sommets;
public :
	Wrapper_Maille():sommets(NULL) {}
	~Wrapper_Maille() {}
	inline const Wrapper_Maille & positionne(int * pos){sommets=pos;return *this;}
	inline int operator[](int i) const {return sommets[i];} 
	virtual int	    DONNE_NBR_NOEUDS()  		      const {cerr<<"APPEL D'UNE FONCTION VIRTUELLE DE LA CLASSE MERE WRAPPER_MAILLE => EXIT"<<endl;exit(-1);}
	virtual int	    DONNE_NBR_FACES()			      const {cerr<<"APPEL D'UNE FONCTION VIRTUELLE DE LA CLASSE MERE WRAPPER_MAILLE => EXIT"<<endl;exit(-1);}
	virtual vector<int> DONNE_SIMPLEXE_BASE(int num_face)	      const {cerr<<"APPEL D'UNE FONCTION VIRTUELLE DE LA CLASSE MERE WRAPPER_MAILLE => EXIT"<<endl;exit(-1);}
	virtual vector<int> DONNE_FACE(int num_face)		      const {cerr<<"APPEL D'UNE FONCTION VIRTUELLE DE LA CLASSE MERE WRAPPER_MAILLE => EXIT"<<endl;exit(-1);}
	virtual int	    DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const {cerr<<"APPEL D'UNE FONCTION VIRTUELLE DE LA CLASSE MERE WRAPPER_MAILLE => EXIT"<<endl;exit(-1);}
};

/*********************************************************/
/*                                                       */
/*            Dérivées de Wrapper_Maille                 */
/*                                                       */
/*********************************************************/

// class Wrapper_Maille_Seg2	; // public Wrapper_Maille;
// class Wrapper_Maille_Seg3	; // public Wrapper_Maille;
class Wrapper_Maille_Tria3   ; // public Wrapper_Maille;
class Wrapper_Maille_Tria6   ; // public Wrapper_Maille;
class Wrapper_Maille_Quad4   ; // public Wrapper_Maille;
class Wrapper_Maille_Quad8   ; // public Wrapper_Maille;
class Wrapper_Maille_Tetra4  ; // public Wrapper_Maille;
class Wrapper_Maille_Tetra10 ; // public Wrapper_Maille;
class Wrapper_Maille_Hexa8   ; // public Wrapper_Maille;
class Wrapper_Maille_Hexa20  ; // public Wrapper_Maille;
class Wrapper_Maille_Pyra5   ; // public Wrapper_Maille;
class Wrapper_Maille_Pyra13  ; // public Wrapper_Maille;
class Wrapper_Maille_Penta6  ; // public Wrapper_Maille;
class Wrapper_Maille_Penta15 ; // public Wrapper_Maille;

/*********************************************************/
/*                                                       */
/*            Classe Type_Wrapper_Maille                 */
/*                                                       */
/*********************************************************/

// Permet de switcher en fonction du type de maille

class Type_Wrapper_Maille
{
protected :
	map<int,Wrapper_Maille *> contenu;
public :
	~Type_Wrapper_Maille();
	Type_Wrapper_Maille();
	inline Wrapper_Maille * operator[] (int i) {return contenu[i];}
};

/*********************************************************/
/*                                                       */
/*            Classe Wrapper_Nuage_Maille                */
/*                                                       */
/*********************************************************/

template <class FORME_SKYLINE> class Wrapper_Nuage_Maille
{
protected :
	FORME_SKYLINE * mailles;
	Type_Wrapper_Maille show;
	int nbr_mailles;
	vector<int> types;
	vector< int* > premier_pointeur;
public :
	Wrapper_Nuage_Maille():mailles(NULL) {}
	Wrapper_Nuage_Maille(FORME_SKYLINE * fs);
	~Wrapper_Nuage_Maille() {}
	// Méthodes de la politique
	inline const Wrapper_Maille & operator[](int i);
	inline int         SIZE() {return nbr_mailles;}
	inline int         DONNE_NBR_FACES(int num_maille);
	inline int         DONNE_NBR_NOEUDS(int num_maille);
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_maille,int num_face);
	inline int         DONNE_PREMIER_NOEUD_DE_FACE(int num_maille,int num_face_loc);
	inline vector<int> DONNE_FACE(int num_maille,int num_local_formant);
	inline int         DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(int num_maille_sec,const vector<int> &sommets_face);
	inline int	   DONNE_SOMMET_MAILLE(int num_maille,int num_som);
	inline int	   DONNE_TYPE_MAILLE(int num_maille) {return mailles->DONNE_TYPE_MAILLE(num_maille);}
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

int Comparaison_Informe(const vector<int> &v1,const vector<int> &v2)
	{
	int s1=v1.size();
	int s2=v2.size();
	if (s1!=s2) return FAUX;
	int i1,i2;
	int test;
	for (i1=0;i1<s1;i1++)
		{
		test=1;
		for (i2=0;(i2<s2)&&(test);i2++)
			{
			if (v1[i1]==v2[i2]) test=0;
			}
		if (test) return FAUX;
		}
	return VRAI;
	}
	
/*********************************************************/
/*                                                       */
/*            Classe Wrapper_Nuage_Maille                */
/*                                                       */
/*********************************************************/	
	
template <class FORME_SKYLINE>	Wrapper_Nuage_Maille<FORME_SKYLINE>::Wrapper_Nuage_Maille(FORME_SKYLINE * fs):mailles(fs)
		{
		int i;
		nbr_mailles=mailles->SIZE();
		types.resize(nbr_mailles);
		premier_pointeur.resize(nbr_mailles);
		for (i=0;i<nbr_mailles;i++) 
			{
			types[i]=mailles->DONNE_TYPE_MAILLE(i);
			premier_pointeur[i]=mailles->DONNE_PREMIER_POINTEUR(i);
			}
		show[types[0]]->positionne(premier_pointeur[0]);
		}
template <class FORME_SKYLINE> const Wrapper_Maille & Wrapper_Nuage_Maille<FORME_SKYLINE>::operator[](int i)
		{
		return show[types[i]]->positionne(premier_pointeur[i]);
		}
template <class FORME_SKYLINE>	inline int Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_SOMMET_MAILLE(int num_maille,int num_som)
		{
		return (*this)[num_maille][num_som];
		}

template <class FORME_SKYLINE>	inline int Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_NUM_LOC_FACE_EGALE_A_FORMANT(int num_maille,const vector<int> &sommets_face)
	{
	vector<int> face_loc;
	int num_face;
	int test=FAUX;
	for (num_face=0;num_face<DONNE_NBR_FACES(num_maille);num_face++)
		{
		face_loc=DONNE_FACE(num_maille,num_face);
		if (Comparaison_Informe(face_loc,sommets_face)) return num_face;
		}
	return UNDEFINED;
	}
template <class FORME_SKYLINE> int Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_NBR_NOEUDS(int num_maille)
		{
		return (*this)[num_maille].DONNE_NBR_NOEUDS();
		}
template <class FORME_SKYLINE> int Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_NBR_FACES(int num_maille)
		{
		return (*this)[num_maille].DONNE_NBR_FACES();
		}
template <class FORME_SKYLINE> vector<int> Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_SIMPLEXE_BASE(int num_maille,int num_face)
		{
		return (*this)[num_maille].DONNE_SIMPLEXE_BASE(num_face);
		}
template <class FORME_SKYLINE> vector<int> Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_FACE(int num_maille,int num_face)
		{
		return (*this)[num_maille].DONNE_FACE(num_face);
		}
template <class FORME_SKYLINE> int Wrapper_Nuage_Maille<FORME_SKYLINE>::DONNE_PREMIER_NOEUD_DE_FACE(int num_maille,int num_face)
		{
		return (*this)[num_maille].DONNE_PREMIER_NOEUD_DE_FACE(num_face);
		}

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Seg2                  */
/*                                                       */
/*********************************************************/
	
/*
class Wrapper_Maille_Seg2    : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 2;}
	inline int DONNE_NBR_NOEUDS() const {return 2;}
};
//*/

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Seg3                  */
/*                                                       */
/*********************************************************/

/*
class Wrapper_Maille_Seg3    : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 2;}
	inline int DONNE_NBR_NOEUDS() const {return 3;}
};
//*/

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Tria3                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Tria3 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 3;}
	inline int DONNE_NBR_NOEUDS() const {return 3;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[1];
			case 2 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(2);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];break;
			case 1 : retour[0]=sommets[1];retour[1]=sommets[2];break;
			case 2 : retour[0]=sommets[2];retour[1]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[1];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 2 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Tria6                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Tria6 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 3;}
	inline int DONNE_NBR_NOEUDS() const {return 6;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[1];
			case 2 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(2);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];break;
			case 1 : retour[0]=sommets[1];retour[1]=sommets[2];break;
			case 2 : retour[0]=sommets[2];retour[1]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[1];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 2 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Quad4                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Quad4 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 4;}
	inline int DONNE_NBR_NOEUDS() const {return 4;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[1];
			case 2 : return sommets[2];
			case 3 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(2);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];break;
			case 1 : retour[0]=sommets[1];retour[1]=sommets[2];break;
			case 2 : retour[0]=sommets[2];retour[1]=sommets[3];break;
			case 3 : retour[0]=sommets[3];retour[1]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[1];break;
			case 1 : retour[0]=sommets[3];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[2];retour[2]=sommets[3];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[0];retour[3]=sommets[0];break;
			}
		return retour;
		}
};


/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Quad8                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Quad8 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 4;}
	inline int DONNE_NBR_NOEUDS() const {return 8;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[1];
			case 2 : return sommets[2];
			case 3 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(2);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];break;
			case 1 : retour[0]=sommets[1];retour[1]=sommets[2];break;
			case 2 : retour[0]=sommets[2];retour[1]=sommets[3];break;
			case 3 : retour[0]=sommets[3];retour[1]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[1];break;
			case 1 : retour[0]=sommets[3];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[2];retour[2]=sommets[3];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[0];retour[3]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Tetra4                */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Tetra4 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 4;}
	inline int DONNE_NBR_NOEUDS() const {return 4;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[0];
			case 2 : return sommets[1];
			case 3 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[3];retour[2]=sommets[1];break;
			case 2 : retour[0]=sommets[1];retour[1]=sommets[3];retour[2]=sommets[2];break;
			case 3 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[1];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[3];retour[3]=sommets[2];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[3];retour[3]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*          Classe Wrapper_Maille_Tetra10                */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Tetra10 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 4;}
	inline int DONNE_NBR_NOEUDS() const {return 10;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[0];
			case 2 : return sommets[1];
			case 3 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(3);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[3];retour[2]=sommets[1];break;
			case 2 : retour[0]=sommets[1];retour[1]=sommets[3];retour[2]=sommets[2];break;
			case 3 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[1];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[3];retour[3]=sommets[2];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[3];retour[3]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Hexa8                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Hexa8 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 6;}
	inline int DONNE_NBR_NOEUDS() const {return 8;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[4];
			case 2 : return sommets[0];
			case 3 : return sommets[1];
			case 4 : return sommets[2];
			case 5 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];retour[3]=sommets[3];break;
			case 1 : retour[0]=sommets[4];retour[1]=sommets[5];retour[2]=sommets[6];retour[3]=sommets[7];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[4];retour[2]=sommets[5];retour[3]=sommets[1];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[5];retour[2]=sommets[6];retour[3]=sommets[2];break;
			case 4 : retour[0]=sommets[2];retour[1]=sommets[6];retour[2]=sommets[7];retour[3]=sommets[3];break;
			case 5 : retour[0]=sommets[3];retour[1]=sommets[7];retour[2]=sommets[4];retour[3]=sommets[0];break;
	
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[5];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[7];retour[2]=sommets[6];retour[3]=sommets[5];break;
			case 2 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[4];retour[3]=sommets[5];break;
			case 3 : retour[0]=sommets[4];retour[1]=sommets[1];retour[2]=sommets[5];retour[3]=sommets[6];break;
			case 4 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[6];retour[3]=sommets[3];break;
			case 5 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[7];retour[3]=sommets[0];break;
	
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Hexa20                */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Hexa20 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 6;}
	inline int DONNE_NBR_NOEUDS() const {return 20;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[4];
			case 2 : return sommets[0];
			case 3 : return sommets[1];
			case 4 : return sommets[2];
			case 5 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];retour[3]=sommets[3];break;
			case 1 : retour[0]=sommets[4];retour[1]=sommets[5];retour[2]=sommets[6];retour[3]=sommets[7];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[4];retour[2]=sommets[5];retour[3]=sommets[1];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[5];retour[2]=sommets[6];retour[3]=sommets[2];break;
			case 4 : retour[0]=sommets[2];retour[1]=sommets[6];retour[2]=sommets[7];retour[3]=sommets[3];break;
			case 5 : retour[0]=sommets[3];retour[1]=sommets[7];retour[2]=sommets[4];retour[3]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[5];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[0];retour[1]=sommets[7];retour[2]=sommets[6];retour[3]=sommets[5];break;
			case 2 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[4];retour[3]=sommets[5];break;
			case 3 : retour[0]=sommets[4];retour[1]=sommets[1];retour[2]=sommets[5];retour[3]=sommets[6];break;
			case 4 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[6];retour[3]=sommets[3];break;
			case 5 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[7];retour[3]=sommets[0];break;	
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Pyra5                 */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Pyra5 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 5;}
	inline int DONNE_NBR_NOEUDS() const {return 5;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[0];
			case 2 : return sommets[1];
			case 3 : return sommets[2];
			case 4 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour;
		switch (num_face) 
			{
			case 0 : retour.resize(4);retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];retour[3]=sommets[3];break;
			case 1 : retour.resize(3);retour[0]=sommets[0];retour[1]=sommets[4];retour[2]=sommets[1];break;
			case 2 : retour.resize(3);retour[0]=sommets[1];retour[1]=sommets[4];retour[2]=sommets[2];break;
			case 3 : retour.resize(3);retour[0]=sommets[2];retour[1]=sommets[4];retour[2]=sommets[3];break;
			case 4 : retour.resize(3);retour[0]=sommets[3];retour[1]=sommets[4];retour[2]=sommets[0];break;
			}
		return retour;
		}

	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[4];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[4];retour[3]=sommets[1];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[4];retour[3]=sommets[2];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[4];retour[3]=sommets[3];break;
			case 4 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[4];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Pyra13                */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Pyra13 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 5;}
	inline int DONNE_NBR_NOEUDS() const {return 13;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[0];
			case 2 : return sommets[1];
			case 3 : return sommets[2];
			case 4 : return sommets[3];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour;
		switch (num_face) 
			{
			case 0 : retour.resize(4);retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];retour[3]=sommets[3];break;
			case 1 : retour.resize(3);retour[0]=sommets[0];retour[1]=sommets[4];retour[2]=sommets[1];break;
			case 2 : retour.resize(3);retour[0]=sommets[1];retour[1]=sommets[4];retour[2]=sommets[2];break;
			case 3 : retour.resize(3);retour[0]=sommets[2];retour[1]=sommets[4];retour[2]=sommets[3];break;
			case 4 : retour.resize(3);retour[0]=sommets[3];retour[1]=sommets[4];retour[2]=sommets[0];break;
			}
		return retour;
		}
	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[4];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[4];retour[3]=sommets[1];break;
			case 2 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[4];retour[3]=sommets[2];break;
			case 3 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[4];retour[3]=sommets[3];break;
			case 4 : retour[0]=sommets[2];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[4];break;
			}
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Penta6                */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Penta6 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 5;}
	inline int DONNE_NBR_NOEUDS() const {return 6;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[3];
			case 2 : return sommets[0];
			case 3 : return sommets[1];
			case 4 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour;
		switch (num_face) 
			{
			case 0 : retour.resize(3);retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 1 : retour.resize(3);retour[0]=sommets[3];retour[1]=sommets[4];retour[2]=sommets[5];break;
			case 2 : retour.resize(4);retour[0]=sommets[0];retour[1]=sommets[3];retour[2]=sommets[4];retour[3]=sommets[1];break;
			case 3 : retour.resize(4);retour[0]=sommets[1];retour[1]=sommets[4];retour[2]=sommets[5];retour[3]=sommets[2];break;
			case 4 : retour.resize(4);retour[0]=sommets[2];retour[1]=sommets[5];retour[2]=sommets[3];retour[3]=sommets[1];break;
			}
		return retour;
		}

	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[4];retour[3]=sommets[5];break;
			case 2 : retour[0]=sommets[5];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[1];break;
			case 3 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[4];retour[3]=sommets[2];break;
			case 4 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[5];retour[3]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*           Classe Wrapper_Maille_Penta15               */
/*                                                       */
/*********************************************************/

class Wrapper_Maille_Penta15 : public Wrapper_Maille
{
public :
	inline int DONNE_NBR_FACES() const {return 5;}
	inline int DONNE_NBR_NOEUDS() const {return 15;}
	inline int DONNE_PREMIER_NOEUD_DE_FACE(int num_face) const
		{
		switch (num_face)
			{
			case 0 : return sommets[0];
			case 1 : return sommets[3];
			case 2 : return sommets[0];
			case 3 : return sommets[1];
			case 4 : return sommets[2];
			}
		}
	inline vector<int> DONNE_FACE(int num_face) const
		{
		vector<int> retour;
		switch (num_face) 
			{
			case 0 : retour.resize(3);retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[2];break;
			case 1 : retour.resize(3);retour[0]=sommets[3];retour[1]=sommets[4];retour[2]=sommets[5];break;
			case 2 : retour.resize(4);retour[0]=sommets[0];retour[1]=sommets[3];retour[2]=sommets[4];retour[3]=sommets[1];break;
			case 3 : retour.resize(4);retour[0]=sommets[1];retour[1]=sommets[4];retour[2]=sommets[5];retour[3]=sommets[2];break;
			case 4 : retour.resize(4);retour[0]=sommets[2];retour[1]=sommets[5];retour[2]=sommets[3];retour[3]=sommets[1];break;
			}
		return retour;
		}

	inline vector<int> DONNE_SIMPLEXE_BASE(int num_face) const
		{
		vector<int> retour(4);
		switch (num_face) 
			{
			case 0 : retour[0]=sommets[3];retour[1]=sommets[0];retour[2]=sommets[1];retour[3]=sommets[2];break;
			case 1 : retour[0]=sommets[2];retour[1]=sommets[3];retour[2]=sommets[4];retour[3]=sommets[5];break;
			case 2 : retour[0]=sommets[5];retour[1]=sommets[0];retour[2]=sommets[3];retour[3]=sommets[1];break;
			case 3 : retour[0]=sommets[0];retour[1]=sommets[1];retour[2]=sommets[4];retour[3]=sommets[2];break;
			case 4 : retour[0]=sommets[1];retour[1]=sommets[2];retour[2]=sommets[5];retour[3]=sommets[0];break;
			}
		return retour;
		}
};

/*********************************************************/
/*                                                       */
/*            Classe Type_Wrapper_Maille                 */
/*                                                       */
/*********************************************************/

Type_Wrapper_Maille::~Type_Wrapper_Maille()
	{
	// delete contenu[MED_SEG2   ];
	// delete contenu[MED_SEG3   ];
	delete contenu[MED_TRIA3  ];
	delete contenu[MED_TRIA6  ];
	delete contenu[MED_QUAD4  ];
	delete contenu[MED_QUAD8  ];
	delete contenu[MED_TETRA4 ];
	delete contenu[MED_TETRA10];
	delete contenu[MED_HEXA8  ];
	delete contenu[MED_HEXA20 ];
	delete contenu[MED_PYRA5  ];
	delete contenu[MED_PYRA13 ];
	delete contenu[MED_PENTA6 ];
	delete contenu[MED_PENTA15];
	}

Type_Wrapper_Maille::Type_Wrapper_Maille()
	{
	// contenu[MED_SEG2   ]= new Wrapper_Maille_Seg2    ();
	// contenu[MED_SEG3   ]= new Wrapper_Maille_Seg3    ();
	contenu[MED_TRIA3  ]= new Wrapper_Maille_Tria3   ();
	contenu[MED_TRIA6  ]= new Wrapper_Maille_Tria6   ();
	contenu[MED_QUAD4  ]= new Wrapper_Maille_Quad4   ();
	contenu[MED_QUAD8  ]= new Wrapper_Maille_Quad8   ();
	contenu[MED_TETRA4 ]= new Wrapper_Maille_Tetra4  ();
	contenu[MED_TETRA10]= new Wrapper_Maille_Tetra10 ();
	contenu[MED_HEXA8  ]= new Wrapper_Maille_Hexa8   ();
	contenu[MED_HEXA20 ]= new Wrapper_Maille_Hexa20  ();
	contenu[MED_PYRA5  ]= new Wrapper_Maille_Pyra5   ();
	contenu[MED_PYRA13 ]= new Wrapper_Maille_Pyra13  ();
	contenu[MED_PENTA6 ]= new Wrapper_Maille_Penta6  ();
	contenu[MED_PENTA15]= new Wrapper_Maille_Penta15 ();
	}


#endif
