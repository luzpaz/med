#ifndef COORDONNEES_BARYCENTRIQUES_HPP
#define COORDONNEES_BARYCENTRIQUES_HPP

// DECLARATIONS

#define _TEMPL_SPE_ template <class NUAGEMAILLE, class NUAGENOEUD, class NOEUD>
#define _COORDBARYC_ Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,DIMENSION>
#define _COORDBARY_2D_ Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,2>
#define _COORDBARY_3D_ Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,3>

template <class NUAGEMAILLE, class NUAGENOEUD, class NOEUD, int DIMENSION> class Coordonnees_Barycentriques
{
// TEMPLATE GENERIQUE VIDE OBLIGE DE PASSER PAR UNE SPECIALISATION
};

_TEMPL_SPE_ class _COORDBARY_2D_
{
protected :
	NUAGEMAILLE * mailles;
	NUAGENOEUD * sommets;
	
	vector<int> etat_coord_baryc;
	vector< vector< vector<double> > > coord_baryc;
	
public :
	
	Coordonnees_Barycentriques():mailles(NULL),sommets(NULL) {}
	Coordonnees_Barycentriques(NUAGEMAILLE * m, NUAGENOEUD *n);
	~Coordonnees_Barycentriques() {}
	vector<double> Donne_Pseudo_Coord_Baryc(int num_maille,const NOEUD &M);
	vector<double> Calcule_Base_Coord_Baryc(const vector<int> &simplexe_base);	
	vector<double> Calcule_Coord_Baryc(int num_maille, const NOEUD & M);
};

_TEMPL_SPE_ class _COORDBARY_3D_
{
protected :
	NUAGEMAILLE * mailles;
	NUAGENOEUD * sommets;
	
	vector<int> etat_coord_baryc;
	vector< vector< vector<double> > > coord_baryc;
	
public :
	
	Coordonnees_Barycentriques():mailles(NULL),sommets(NULL) {}
	Coordonnees_Barycentriques(NUAGEMAILLE * m, NUAGENOEUD *n);
	~Coordonnees_Barycentriques() {}
	vector<double> Donne_Pseudo_Coord_Baryc(int num_maille,const NOEUD &M);
	vector<double> Calcule_Base_Coord_Baryc(const vector<int> &simplexe_base);	
	vector<double> Calcule_Coord_Baryc(int num_maille, const NOEUD & M);
};

// CODE

_TEMPL_SPE_ _COORDBARY_2D_::Coordonnees_Barycentriques(NUAGEMAILLE * m, NUAGENOEUD *n):mailles(m),sommets(n)
		{
		cout<<"Creation des Coordonn�es Barycentriques : "<<flush;
		int nbr_mailles=mailles->SIZE();
		etat_coord_baryc=vector<int>(nbr_mailles,FAUX);
		coord_baryc=vector< vector< vector<double> > >(nbr_mailles);
		cout<<"OK ! "<<endl;
		}

_TEMPL_SPE_ vector<double> _COORDBARY_2D_::Donne_Pseudo_Coord_Baryc(int num_maille,const NOEUD &M)
	{
	int i,j,nbr_faces;
	if (etat_coord_baryc[num_maille]==FAUX) 
		{
		nbr_faces=mailles->DONNE_NBR_FACES(num_maille);
		
		coord_baryc[num_maille]=vector< vector<double> >(nbr_faces);
		
		for (i=0;i<nbr_faces;i++)
			{
			vector<int> simplexe_base=mailles->DONNE_SIMPLEXE_BASE(num_maille,i);
			coord_baryc[num_maille][i]=Calcule_Base_Coord_Baryc(simplexe_base);
			etat_coord_baryc[num_maille]=VRAI;
			}
		}	
	return Calcule_Coord_Baryc(num_maille,M);
	}

_TEMPL_SPE_ vector<double> _COORDBARY_2D_::Calcule_Base_Coord_Baryc(const vector<int> &simplexe_base)
	{
	const vector<int> &ref=simplexe_base;
	vector<double> retour(3);
	int i,j;
		
	double x0=(*sommets)[ref[0]][0];
	double y0=(*sommets)[ref[0]][1];
	double x1=(*sommets)[ref[1]][0];
	double y1=(*sommets)[ref[1]][1];
	double x2=(*sommets)[ref[2]][0];
	double y2=(*sommets)[ref[2]][1];
		
	double delta=(x1*y2-x2*y1)+(x2*y0-x0*y2)+(x0*y1-x1*y0);

	retour[0]=(y1-y2)/delta;
	retour[1]=(x2-x1)/delta;
	retour[2]=(x1*y2-x2*y1)/delta;
	
	return retour;
	}

_TEMPL_SPE_ vector<double> _COORDBARY_2D_::Calcule_Coord_Baryc(int num_maille, const NOEUD & M)
	{
	int i,j;
	vector<double> coord_baryc_M(3,0);
	for (i=0;i<3;i++) 
		{
		for (j=0;j<2;j++) coord_baryc_M[i]+=coord_baryc[num_maille][i][j]*M[j];
		coord_baryc_M[i]+=coord_baryc[num_maille][i][2];
		}
	return coord_baryc_M;
	}

_TEMPL_SPE_ _COORDBARY_3D_::Coordonnees_Barycentriques(NUAGEMAILLE * m, NUAGENOEUD *n):mailles(m),sommets(n)
		{
		cout<<"Creation des Coordonn�es Barycentriques : "<<flush;
		int nbr_mailles=mailles->SIZE();
		etat_coord_baryc=vector<int>(nbr_mailles,FAUX);
		coord_baryc=vector< vector< vector<double> > >(nbr_mailles);
		cout<<"OK ! "<<endl;
		}
	
_TEMPL_SPE_ vector<double> _COORDBARY_3D_::Donne_Pseudo_Coord_Baryc(int num_maille,const NOEUD &M)
	{
	int i,j,nbr_faces;
	if (etat_coord_baryc[num_maille]==FAUX) 
		{
		nbr_faces=mailles->DONNE_NBR_FACES(num_maille);
		
		coord_baryc[num_maille]=vector< vector<double> >(nbr_faces);
		
		for (i=0;i<nbr_faces;i++)
			{
			vector<int> simplexe_base=mailles->DONNE_SIMPLEXE_BASE(num_maille,i);
			coord_baryc[num_maille][i]=Calcule_Base_Coord_Baryc(simplexe_base);
			etat_coord_baryc[num_maille]=VRAI;
			}
		}	
	return Calcule_Coord_Baryc(num_maille,M);
	}


_TEMPL_SPE_ vector<double> _COORDBARY_3D_::Calcule_Base_Coord_Baryc(const vector<int> &simplexe_base)
	{
	const vector<int> &ref=simplexe_base;
	vector<double> retour(4);
	int i,j;
		
	double x0=(*sommets)[ref[0]][0];
	double y0=(*sommets)[ref[0]][1];
	double z0=(*sommets)[ref[0]][2];
	double x1=(*sommets)[ref[1]][0];
	double y1=(*sommets)[ref[1]][1];
	double z1=(*sommets)[ref[1]][2];
	double x2=(*sommets)[ref[2]][0];
	double y2=(*sommets)[ref[2]][1];
	double z2=(*sommets)[ref[2]][2];
	double x3=(*sommets)[ref[3]][0];
	double y3=(*sommets)[ref[3]][1];
	double z3=(*sommets)[ref[3]][2];
		
	double delta1=((y2-y1)*(z3-z1)-(z2-z1)*(y3-y1));
	double delta2=((x3-x1)*(z2-z1)-(x2-x1)*(z3-z1));
	double delta3=((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1));
	
	double delta=delta1*(x0-x1)+delta2*(y0-y1)+delta3*(z0-z1);

	retour[0]=delta1/delta;
	retour[1]=delta2/delta;
	retour[2]=delta3/delta;
	retour[3]=-(delta1*x1+delta2*y1+delta3*z1)/delta;
	
	return retour;
	}

_TEMPL_SPE_ vector<double> _COORDBARY_3D_::Calcule_Coord_Baryc(int num_maille, const NOEUD & M)
	{
	int i,j;
	int nbr_faces=coord_baryc[num_maille].size();
	vector<double> coord_baryc_M(nbr_faces,0);
	for (i=0;i<nbr_faces;i++) 
		{
		for (j=0;j<3;j++) coord_baryc_M[i]+=coord_baryc[num_maille][i][j]*M[j];
		coord_baryc_M[i]+=coord_baryc[num_maille][i][3];
		}
	return coord_baryc_M;
	}

//*/

#undef _TEMPLATE_SPE_ template <class NUAGEMAILLE, class NUAGENOEUD, class NOEUD, int DIMENSION>
#undef _COORDBARYC_2D_ Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,2>
#undef _COORDBARYC_3D_ Coordonnees_Barycentriques<NUAGEMAILLE,NUAGENOEUD,NOEUD,3>

#endif
