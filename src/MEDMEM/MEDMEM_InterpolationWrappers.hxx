
#ifndef WRAPPERS_HPP
#define WRAPPERS_HPP

// les classes de wrapping

template <int DIMENSION> class Wrapper_Noeud
{
protected :
  double * coordonnees;
public :
	Wrapper_Noeud():coordonnees(NULL) 
		{
		}
        Wrapper_Noeud(const double * coord):coordonnees( (double *) coord) 
		{
		}
	~Wrapper_Noeud() 
		{
		coordonnees=NULL;
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
	};
	
template <int DIMENSION> class Wrapper_Nuage
	{
	protected : 
		int nbr_noeuds;
                const double *  noeuds;
		Wrapper_Noeud<DIMENSION> show;
	public :
		Wrapper_Nuage():nbr_noeuds(0),noeuds(NULL) {}
                Wrapper_Nuage( const double * const n, int nn):nbr_noeuds(nn),noeuds(n),show(noeuds) {}
		~Wrapper_Nuage() {noeuds=NULL;}
		Wrapper_Noeud<DIMENSION> & operator [] (int i)
			{
			show.positionne((double *) &noeuds[DIMENSION*i]);
			return show;
			}
		int size() const {return nbr_noeuds;}
	};

#endif
