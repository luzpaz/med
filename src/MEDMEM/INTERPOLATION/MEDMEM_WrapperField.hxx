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

template <class TYPE> class Valeur;

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

template <class TYPE> class Valeur
{
protected :
	TYPE * valeurs;
	int nbr_valeurs;
	int a_detruire;
public :
	Valeur():valeurs(NULL),a_detruire(0){}
	Valeur(TYPE * val,int nv):valeurs(val),nbr_valeurs(nv),a_detruire(0){} // A VERIFIER
	Valeur(int n):nbr_valeurs(n),a_detruire(1)
		{
		valeurs=new TYPE[nbr_valeurs];
		}
	template <typename Left,typename Op,typename Right> Valeur(X<Left,Op,Right> expression)
		{
		for (int i=0;i<nbr_valeurs;i++) valeurs[i]=expression[i];
		}
	template <typename Left,typename Op,typename Right> void operator=(X<Left,Op,Right> expression)
		{
		for (int i=0;i<nbr_valeurs;i++) valeurs[i]=expression[i];
		}
	void operator=(Valeur v)
		{
		for (int i=0;i<nbr_valeurs;i++) valeurs[i]=v[i];
		}
	Valeur(const Valeur &v):nbr_valeurs(v.nbr_valeurs)
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
	~Valeur(){if (a_detruire) delete [] valeurs;}
	TYPE operator[](int i){return valeurs[i];}
	int SIZE() const {return nbr_valeurs;}
};

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
	Wrapper_MED_Field(const FIELD<double> * medfield)
		{
		nbr_valeurs=medfield->getNumberOfValues();
		nbr_composantes=medfield->getNumberOfComponents();
		valeurs=const_cast<double *>(medfield->getValue(MED_FULL_INTERLACE));
		}
	~Wrapper_MED_Field(){}
	inline Valeur<double> operator[](int i) 
		{
		if ((i<0)||(i>=nbr_valeurs))
			{
			cerr<<endl;
			cerr<<"Outbound call dans Wrapper MED Field"<<endl;
			cerr<<"Inférior Bound = "<<0<<endl;
			cerr<<"Supérior Bound = "<<nbr_valeurs-1<<endl;
			cerr<<"Call = "<<i<<endl;
			exit(-1);
			}
		return Valeur<double>(&valeurs[nbr_composantes*i],nbr_composantes);
		}
	double * Get_Valeurs() {return valeurs;}
	inline int Get_Nbr_Valeurs() const {return nbr_valeurs;}
	inline int Get_Nbr_Composantes() const {return nbr_composantes;}
	friend ostream & operator<<(ostream &os, Wrapper_MED_Field);
}; 

ostream & operator<<(ostream &os, Wrapper_MED_Field wmf)
{
for (int i=0;i<wmf.nbr_valeurs;i++) os<<"Wrapper_MED_Field["<<i<<"] = "<<wmf[i]<<endl;
return os;
}

#endif
