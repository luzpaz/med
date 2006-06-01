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
#ifndef MEDMEM_WRAPPER_FIELD_HXX
#define MEDMEM_WRAPPER_FIELD_HXX

#include "MEDMEM_Field.hxx"

#include <vector>

//////////////////////////////////////////////////////////////////
///                                                            ///
///                 DECLARATIONS ET CODE                       ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*        Template Arithmétiques de Valdhuizen           */
/*                                                       */
/*********************************************************/

// permet de faire des opérations algébriques sur des Wrappers_MED_Field sans faire d'allocations inutiles
// voir les articles de Valdhuizen pour la compréhension du processus

struct Plus 
{
public :
	static double apply(double a,double b) {return a+b;}
};

struct Multiply
{
public :
	static double apply(double a,double b) {return a*b;}
};


template <typename Left,typename Op, typename Right> struct X
{
	Left left;
	Right right;
	X(Left l,Right r):left(l),right(r){}
	double operator[](int i)
		{
		return Op::apply(left[i],right[i]);
		}
};

template <typename Right> struct X<double,Multiply,Right>
{
	double left;
	Right right;
	X(double l,Right r):left(l),right(r){}
	double operator[](int i)
		{
		return Multiply::apply(left,right[i]);
		}
};

template <class TYPE> class Valeur
{
protected :
	TYPE * valeurs;
	int nbr_valeurs;
	int a_detruire;
public :
	Valeur();
	Valeur(TYPE * val,int nv);
	Valeur(int n);
	template <typename Left,typename Op,typename Right> Valeur(X<Left,Op,Right> expression);
	template <typename Left,typename Op,typename Right> void operator=(X<Left,Op,Right> expression);
	void operator=(Valeur v);
	Valeur(const Valeur &v);
	~Valeur();
	TYPE operator[](int i);
	int SIZE() const;
	double NormeAbs();
};

template <typename TYPE> X< Valeur<TYPE>,Plus,Valeur<TYPE> > operator+(Valeur<TYPE> v1,Valeur<TYPE> v2)
{
return X< Valeur<TYPE>,Plus,Valeur<TYPE> >(v1,v2);
}

template <typename TYPE> X< double,Multiply,Valeur<double> > operator*(TYPE sca,Valeur<TYPE> v)
{
return X< TYPE,Multiply,Valeur<TYPE> >(sca,v);
}

template <typename TYPE,typename  L, typename O, typename R> X< Valeur<TYPE>,Plus,X<L,O,R> > operator+(Valeur<TYPE> v,X<L,O,R> expression)
{
return X< Valeur<TYPE>,Plus,X<L,O,R> >(v,expression);
}

template <typename TYPE,typename  L, typename O, typename R> X< X<L,O,R>,Plus,Valeur<TYPE> > operator+(X<L,O,R> expression,Valeur<TYPE> v)
{
return X< X<L,O,R>,Plus,Valeur<TYPE> >(expression,v);
}

template <typename  Ll, typename  Ol, typename  Rl,typename  Lr, typename  Or, typename  Rr> X< X<Ll,Ol,Rl>,Plus, X<Lr,Or,Rr> > operator+(X<Ll,Ol,Rl> el, X<Lr,Or,Rr> er )
{
return X< X<Ll,Ol,Rl>,Plus,X<Lr,Or,Rr> >(el,er);
}

template <typename  L, typename O, typename R> X< double,Multiply,X<L,O,R> > operator*(double sca,X<L,O,R> expression)
{
return X< double,Multiply,X<L,O,R> >(sca,expression);
}

template <typename Left,typename Op,typename Right> X< double,Multiply,X<Left,Op,Right> > operator/(X<Left,Op,Right> l,double x)
{
return X< double,Multiply,X<Left,Op,Right> >(((double) 1/x),l);
}

/*********************************************************/
/*                                                       */
/*                    Classe Valeur                      */
/*                                                       */
/*********************************************************/

// Problèmes : les constructeurs par copie ne sont pas satisfaisants
// Valeur est symboliquement l'argument d'une classe formelle Vecteur<Valeur>
// elle peut etre un réel ou un pointeur sur réel, simulant un vecteur de vecteur

template <class TYPE> Valeur<TYPE>::Valeur():valeurs(NULL),a_detruire(0){}
template <class TYPE> Valeur<TYPE>::Valeur(TYPE * val,int nv):valeurs(val),nbr_valeurs(nv),a_detruire(0){} // A VERIFIER
template <class TYPE> Valeur<TYPE>::Valeur(int n):nbr_valeurs(n),a_detruire(1)
{
  valeurs=new TYPE[nbr_valeurs];
}
template <class TYPE> template <typename Left,typename Op,typename Right> Valeur<TYPE>::Valeur(X<Left,Op,Right> expression)
{
  for (int i=0;i<nbr_valeurs;i++) valeurs[i]=expression[i];
}
template <class TYPE> template <typename Left,typename Op,typename Right> void Valeur<TYPE>::operator=(X<Left,Op,Right> expression)
{
  for (int i=0;i<nbr_valeurs;i++) valeurs[i]=expression[i];
}
template <class TYPE> void Valeur<TYPE>::operator=(Valeur v)
{
  for (int i=0;i<nbr_valeurs;i++) valeurs[i]=v[i];
}
template <class TYPE> Valeur<TYPE>::Valeur(const Valeur &v):nbr_valeurs(v.nbr_valeurs)
{
  if (v.a_detruire) 
    {
      a_detruire=1;
      valeurs=new TYPE[nbr_valeurs];
      for (int i=0;i<nbr_valeurs;i++) valeurs[i]=v.valeurs[i];
    }
  else
    {
      a_detruire=0;
      valeurs=v.valeurs;
    }
}
template <class TYPE> Valeur<TYPE>::~Valeur(){if (a_detruire) delete [] valeurs;}
template <class TYPE> TYPE Valeur<TYPE>::operator[](int i){return valeurs[i];}
template <class TYPE> int Valeur<TYPE>::SIZE() const {return nbr_valeurs;}
template <class TYPE> double Valeur<TYPE>::NormeAbs()
{
  int i;
  double tmp=0;
  for (i=0;i<nbr_valeurs;i++) tmp+=fabs(valeurs[i]);
  return tmp;
}

template <class TYPE> ostream &operator<<(ostream &os,Valeur<TYPE> v)
	{	
	os<<"("<<flush;
	for (int i=0;i<v.SIZE();i++) os<<" "<<v[i]<<flush;
	os<<" ) "<<flush;
	return os;
	}

/*********************************************************/
/*                                                       */
/*               Classe Wrapper_MED_Field                */
/*                                                       */
/*********************************************************/

// c'est la classe de Wrapping sur un objet FIELD<double> MEDMEMOIRE

class Wrapper_MED_Field
{
protected :
	int nbr_valeurs;
	int nbr_composantes;
	double * valeurs;
public :
	Wrapper_MED_Field():valeurs(NULL){}
	Wrapper_MED_Field(int nv, int nc, double * v):nbr_valeurs(nv),nbr_composantes(nc),valeurs(v)
		{
		}
	Wrapper_MED_Field(const MEDMEM::FIELD<double,MEDMEM::FullInterlace> * medfield)
		{
		nbr_valeurs=medfield->getNumberOfValues();
		nbr_composantes=medfield->getNumberOfComponents();
		valeurs=const_cast<double *>(medfield->getValue());
		}
	~Wrapper_MED_Field(){}
	inline Valeur<double> operator[](int i) 
		{
		return Valeur<double>(&valeurs[nbr_composantes*i],nbr_composantes);
		}
	double * Get_Valeurs() {return valeurs;}
	inline int Get_Nbr_Valeurs() const {return nbr_valeurs;}
	inline int Get_Nbr_Composantes() const {return nbr_composantes;}
	friend ostream & operator<<(ostream &os, Wrapper_MED_Field);
}; 

inline ostream & operator<<(ostream &os, Wrapper_MED_Field wmf)
{
for (int i=0;i<wmf.nbr_valeurs;i++) os<<"Wrapper_MED_Field["<<i<<"] = "<<wmf[i]<<endl;
return os;
}

#endif
