
#ifndef MEDMEM_WRAPPER_NODES_HXX
#define MEDMEM_WRAPPER_NODES_HXX

#ifndef NULL
#define NULL 0
#endif

// les classes de wrapping

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
	friend ostream & operator<<(ostream &os,const Wrapper_Noeud<DIMENSION> &A)
		{
		os<<"( "<<flush;
		for (int i=0;i<DIMENSION;i++) os<<A[i]<<" "<<flush;
		os<<")"<<flush;
		return os;
		}
	};
	
template <int DIMENSION> class Wrapper_Nuage_Noeud
	{
	protected : 
		int nbr_noeuds;
		double *noeuds;
		Wrapper_Noeud<DIMENSION> show;
	public :
		Wrapper_Nuage_Noeud():nbr_noeuds(0),noeuds(NULL) {}
		Wrapper_Nuage_Noeud(int nn, double *n):nbr_noeuds(nn),noeuds(n),show(noeuds) {}
		~Wrapper_Nuage_Noeud() {}
		Wrapper_Noeud<DIMENSION> & operator [] (int i)
			{
			if ((i<0)||(i>=nbr_noeuds))
				{
				cerr<<"Outbound call dans Wrapper Nuage Noeud"<<endl;
				cerr<<"Inférior Bound = "<<0<<endl;
				cerr<<"Supérior Bound = "<<nbr_noeuds-1<<endl;
				cerr<<"Call = "<<i<<endl;
				exit(-1);
				}
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
