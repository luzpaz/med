#include "INTERPOLATION_2D.hxx"
using namespace std;
using namespace MED_EN;
using namespace MEDMEM;



struct monMaillageS
{
    int _NbMaille;
    int _NbNoeud;
    const int* _Connect;
    const double* _Coord;
    const int* _ReversNConnect;
    const int*  _ReversNConnectIndex;
    double _DimCaracteristic;
};

struct monMaillageP
{
    int _NbMaille;
    int _NbNoeud;
    const int* _Connect;
    const double* _Coord;
    double _DimCaracteristic;
};



INTERPOLATION_2D::INTERPOLATION_2D()
{
    _Precision=1.0E-12;
    _DimCaracteristic=1;
    _SecondMethodOfLocalisation=1;
    _PrintLevel=0;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*   Options :                                        */
/*     Precision : for geometric computation          */
/*     SecondMethodOfLocalisation : 0/1               */
/*     PrintLevel : between 0 and 3                   */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
void INTERPOLATION_2D::setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel)
{
    _Precision=Precision;
    _SecondMethodOfLocalisation=SecondMethodOfLocalisation;
    _PrintLevel=PrintLevel;
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*   calcul la surface d'un triangle                  */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

double INTERPOLATION_2D::Surf_Tri(const double* P_1,const double* P_2,const double* P_3)
{
    double A=(P_3[1]-P_1[1])*(P_2[0]-P_1[0])-(P_2[1]-P_1[1])*(P_3[0]-P_1[0]);
    double Surface = 1/2.0*abs(A);
    return Surface;
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*     fonction qui calcul le déterminant            */
/*      de deux vecteur(cf doc CGAL).                */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/

//fonction qui calcul le déterminant des vecteurs: P3P1 et P3P2
//(cf doc CGAL).

double INTERPOLATION_2D::mon_determinant(const double* P_1,const double*  P_2,
	const double* P_3)
{
    double mon_det=(P_1[0]-P_3[0])*(P_2[1]-P_3[1])-(P_2[0]-P_3[0])*(P_1[1]-P_3[1]);
    return mon_det;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
//calcul la norme du vecteur P1P2

double INTERPOLATION_2D::norme_vecteur(const double* P_1,const double* P_2)
{
    double X=P_1[0]-P_2[0];
    double Y=P_1[1]-P_2[1];
    double norme=sqrt(X*X+Y*Y);
    return norme;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*         calcul le cos et le sin de l'angle P1P2,P1P3     */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

vector<double> INTERPOLATION_2D::calcul_cos_et_sin(const double* P_1,const double* P_2,
	const double* P_3)
{

    vector<double> Vect;
    double P1_P2=norme_vecteur(P_1,P_2);
    double P2_P3=norme_vecteur(P_2,P_3);
    double P3_P1=norme_vecteur(P_3,P_1);

    double N=P1_P2*P1_P2+P3_P1*P3_P1-P2_P3*P2_P3;
    double D=2.0*P1_P2*P3_P1;
    double COS=N/D;
    Vect.push_back(COS);
    double V=mon_determinant(P_2,P_3,P_1);
    double D_1=P1_P2*P3_P1;
    double SIN=V/D_1;
    Vect.push_back(SIN);

    return Vect;

}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*fonction pour vérifier qu'un point n'a pas déja été considérer dans   */ 
/*      le vecteur et le rajouter au vecteur sinon.                     */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

void INTERPOLATION_2D::verif_point_dans_vect(const double* P, vector<double>& V)
{
  
  int taille=V.size();
  //  bool isPresent=false;
  for(int i=0;i<taille/2;i++) 
    {
      double dx=P[0]-V[2*i];
      dx=dx>0.0?dx:-dx;
      if (dx>_Precision)
	continue;
      double dy=P[1]-V[2*i+1];
      dy=dy>0.0?dy:-dy;
      if (dy<_Precision)
	
	return;
      //	if( sqrt((P[0]-V[2*i])*(P[0]-V[2*i])+(P[1]-V[2*i+1])*(P[1]-V[2*i+1])))/_DimCaracteristic<_Precision)
      //  isPresent=true;
      
    }
  //    if(!isPresent)
  //{
  
  V.push_back(P[0]);
  V.push_back(P[1]);
  
  //}
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*     calcul les coordonnées du barycentre d'un polygone   */ 
/*     le vecteur en entrée est constitué des coordonnées   */
/*     des sommets du polygone                              */                             
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

vector<double> INTERPOLATION_2D::bary_poly(const vector<double>& V)
{
    vector<double> Bary;
    int taille=V.size();
    double x=0;
    double y=0;

    for (int i=0;i<taille/2;i++)
    {
	x=x+V[2*i];
	y=y+V[2*i+1];
    }
    double A=2*x/taille;
    double B=2*y/taille;
    Bary.push_back(A);//taille vecteur=2*nb de points.
    Bary.push_back(B);


    return Bary;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*         calcul la surface d'un polygone.                 */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

double  INTERPOLATION_2D::Surf_Poly(const vector<double>& Poly)
{ 

    double Surface=0;
    for (int i=0; i<(Poly.size())/2-2; i++)
    {
	double Surf=Surf_Tri( &Poly[0],&Poly[2*(i+1)],&Poly[2*(i+2)] ); 
	Surface=Surface + Surf ;
    }
    return Surface ;
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _*/
/*  calcul de l'intersection de deux segments: segments P1P2 avec P3P4      */
/*  . Si l'intersection est non nulle et si celle-ci n'est                  */
/*  n'est pas déjà contenue dans Vect on la rajoute à Vect                  */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _*/ 

void INTERPOLATION_2D::inters_de_segment(const double * P_1,const double * P_2,
	const double * P_3,const double * P_4,vector<double>&  Vect)
{


    // calcul du déterminant de P1_1P_2 et P_3P_4.
    double det=(P_2[0]-P_1[0])*(P_4[1]-P_3[1])-(P_4[0]-P_3[0])*(P_2[1]-P_1[1]);


    if(abs(det)/_DimCaracteristic>_Precision)
    {

	double k_1=1/((P_2[0]-P_1[0])*(P_3[1]-P_4[1])+(P_4[0]-P_3[0])*(P_2[1]-P_1[1]))
	    *((P_3[1]-P_4[1])*(P_3[0]-P_1[0])+(P_4[0]-P_3[0])*(P_3[1]-P_1[1]));
	
	if( k_1>=0 &&  k_1<=1)
	  {
	    double k_2=1/((P_4[0]-P_3[0])*(P_1[1]-P_2[1])+(P_2[0]-P_1[0])*(P_4[1]-P_3[1]))
	      *((P_1[1]-P_2[1])*(P_1[0]-P_3[0])+(P_2[0]-P_1[0])*(P_1[1]-P_3[1]));
	    

	    if( k_2>=0 &&  k_2<=1)
	    {
		double P_0[2];
		P_0[0]=P_1[0]+k_1*(P_2[0]-P_1[0]);
		P_0[1]=P_1[1]+k_1*(P_2[1]-P_1[1]);
		verif_point_dans_vect(P_0,Vect);

	    }
	}
    }
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*   fonction qui teste si un point est dans une maille     */
/*   point: P_0                                             */
/*   P_1, P_2, P_3 sommet des mailles                       */   
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

int INTERPOLATION_2D::point_dans_triangle(const double* P_0,const double* P_1,
	const double* P_2,const double* P_3)
{

    int A=0;
    double det_1=mon_determinant(P_1,P_3,P_0);
    double det_2=mon_determinant(P_3,P_2,P_0);
    double det_3=mon_determinant(P_2,P_1,P_0);
    if( (det_1>=0 && det_2>=0 && det_3>=0) || (det_1<=0 && det_2<=0 && det_3<=0) )
    {
	A=1;
    }

    return A;
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/* fonction qui rajoute les sommet du triangle P dans le vecteur V        */ 
/* si ceux-ci sont compris dans le triangle S et ne sont pas déjà dans    */
/* V.                                                                     */
/*sommets de P: P_1, P_2, P_3                                             */
/*sommets de S: P_4, P_5, P_6                                             */  
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */ 

void INTERPOLATION_2D::rajou_sommet_triangl(const double* P_1,const double* P_2,const double* P_3,
	const double* P_4,const double* P_5,const double* P_6,vector<double>& V)
{

    int A_1=point_dans_triangle(P_1,P_4,P_5,P_6);
    if(A_1==1)
    {verif_point_dans_vect(P_1,V);
  //   if (V.size()==1)
//       {
// 	// all nodes are necessarily in the triangle
// 	verif_point_dans_vect(P_2,V);
// 	verif_point_dans_vect(P_3,V);
// 	return;
//       }
    }
    int A_2=point_dans_triangle(P_2,P_4,P_5,P_6);
    if(A_2==1)
    {verif_point_dans_vect(P_2,V);}

    int A_3=point_dans_triangle(P_3,P_4,P_5,P_6);
    if(A_3==1)
    {verif_point_dans_vect(P_3,V);}
    
	
      
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/  
/*      calcul l'intersection de deux triangles            */
/* P_1, P_2, P_3: sommets du premier triangle              */
/* P_4, P_5, P_6: sommets du deuxième triangle             */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 

vector<double> INTERPOLATION_2D::intersec_de_triangle(const double* P_1,const double* P_2,
	const double* P_3,const double* P_4,const double* P_5,const double* P_6)
{

    //debug cout << "  T1 : " << *P_1 << " , " << *(P_1+1) << " , " << *P_2 << " , " << *(P_2+1) << " , " << *P_3 << " , " << *(P_3+1)<< endl;
    //debug cout << "  T2 : " << *P_4 << " , " << *(P_4+1) << " , " << *P_5 << " , " << *(P_5+1) << " , " << *P_6 << " , " << *(P_6+1)<< endl;
    vector<double> Vect;

    inters_de_segment(P_1,P_2,P_4,P_5,Vect);
    inters_de_segment(P_1,P_2,P_5,P_6,Vect);
    inters_de_segment(P_1,P_2,P_6,P_4,Vect);
    inters_de_segment(P_2,P_3,P_4,P_5,Vect);
    inters_de_segment(P_2,P_3,P_5,P_6,Vect);
    inters_de_segment(P_2,P_3,P_6,P_4,Vect);
    inters_de_segment(P_3,P_1,P_4,P_5,Vect);
    inters_de_segment(P_3,P_1,P_5,P_6,Vect);
    inters_de_segment(P_3,P_1,P_6,P_4,Vect);
    rajou_sommet_triangl(P_1,P_2,P_3,P_4,P_5,P_6,Vect);
    rajou_sommet_triangl(P_4,P_5,P_6,P_1,P_2,P_3,Vect);
    //debug cout << "  Inter : ";
    //debug for (int i=0; i<Vect.size(); ++i)
	//debug cout << Vect[i] << "  ";
    //debug cout << endl << endl;

    return Vect;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */  
/* fonction pour reconstituer un polygone convexe à partir  */
/*              d'un nuage de point.                        */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */  

vector<double> INTERPOLATION_2D::reconstruct_polygon(const vector<double>& V)
{

    int taille=V.size();
    if(taille<=6)
    {return V;}
    else
    {
	double COS[taille/2];
	double SIN[taille/2];
	double angle[taille/2];
	vector<double> Bary=bary_poly(V);
	COS[0]=1.0;
	SIN[0]=0.0;
	angle[0]=0.0;
	for(int i=0; i<taille/2-1;i++)
	{
	    vector<double> Trigo=calcul_cos_et_sin(&Bary[0],&V[0],&V[2*(i+1)]);
	    COS[i+1]=Trigo[0];
	    SIN[i+1]=Trigo[1];
	    if(SIN[i+1]>=0)
	    {angle[i+1]=acos(COS[i+1]);}
	    else
	    {angle[i+1]=-acos(COS[i+1]);}
	}

	//ensuite on ordonne les angles.
	vector<double> Pt_ordonne;
	Pt_ordonne.reserve(taille);
	map<double,int> Ordre;
	for(int i=0;i<taille/2;i++)	
	{Ordre[angle[i]]=i;}
	map <double,int>::iterator mi;
	for(mi=Ordre.begin();mi!=Ordre.end();mi++)
	{
	    int j=(*mi).second;
	    Pt_ordonne.push_back(V[2*j]);
	    Pt_ordonne.push_back(V[2*j+1]);
	}
	return Pt_ordonne;
    }
}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ */ 
/*fonction pour trouver les mailles voisines d'une maille triangle.*/
/* (mailles voisines par les faces)                                */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ */ 

vector<int> INTERPOLATION_2D::touv_maill_vois(int i_S,const monMaillageS& MaS)
{

    int N_1=MaS._Connect[3*(i_S-1)];
    int N_2=MaS._Connect[3*(i_S-1)+1];
    int N_3=MaS._Connect[3*(i_S-1)+2];
    vector<int> tab;
    int I_1=MaS._ReversNConnectIndex[N_1-1]-1;// Dans MED le n°des noeuds commencent à 1. 
    int I_2=MaS._ReversNConnectIndex[N_2-1]-1;
    int I_3=MaS._ReversNConnectIndex[N_3-1]-1;
    int F_1=MaS._ReversNConnectIndex[N_1]-1;
    int F_2=MaS._ReversNConnectIndex[N_2]-1;
    int F_3=MaS._ReversNConnectIndex[N_3]-1;


    vector<int> V_12;
    V_12.reserve(2);
    insert_iterator<vector<int> > ib_1(V_12,V_12.begin());
    vector<int> V_23;
    V_23.reserve(2);
    insert_iterator<vector<int> > ib_2(V_23,V_23.begin());
    vector<int> V_13;
    V_13.reserve(2);
    insert_iterator<vector<int> > ib_3(V_13,V_13.begin());


    set_intersection(MaS._ReversNConnect+I_1,MaS._ReversNConnect+F_1,MaS._ReversNConnect+I_2,
	    MaS._ReversNConnect+F_2,ib_1);
    set_intersection(MaS._ReversNConnect+I_2,MaS._ReversNConnect+F_2,MaS._ReversNConnect+I_3,
	    MaS._ReversNConnect+F_3,ib_2);
    set_intersection(MaS._ReversNConnect+I_1,MaS._ReversNConnect+F_1,MaS._ReversNConnect+I_3,
	    MaS._ReversNConnect+F_3,ib_3);


    for(int i=0;i<(V_12).size();i++)
    {
	if(V_12[i]!=i_S)
	{
	    tab.push_back(V_12[i]);
	    break;
	}
    }

    for(int i=0;i<V_23.size();i++)
    {
	if(V_23[i]!=i_S)
	{
	    tab.push_back(V_23[i]);
	    break;
	}
    }

    for(int i=0;i<V_13.size();i++)
    {
	if(V_13[i]!=i_S)
	{
	    tab.push_back(V_13[i]);
	    break;
	}
    }
    return tab;
}

/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
/* fonction pour vérifier qu'un n°de maille n'a pas déja été considérer  */
/*  dans le vecteur et le rajouter au vecteur sinon.                     */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/

void INTERPOLATION_2D::verif_maill_dans_vect(int Num, vector<int>& V)
{
    int taille=V.size();
    int A=0;
    for(int i=0;i<taille;i++)
    {
	if(Num==V[i])
	{
	    A=1;
	    break;
	} 
    }
    if(A==0)
    {V.push_back(Num); }
}



/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*localise le barycentre des mailles de P dans le maillage S*/
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */ 

void INTERPOLATION_2D::local_iP_dans_S(MESH& myMesh_S,MESH& myMesh_P,monMaillageP& MaP,
	monMaillageS& MaS,vector<int>& localis )
{

    //calcul des coordonnées des barycentre des mailles de P


    //calcule des coordonnees du barycentre d'une cellule
    std::auto_ptr<SUPPORT::SUPPORT>  Support ( new SUPPORT(&myMesh_P,"monSupport",MED_CELL) );
    std::auto_ptr<FIELD<double, FullInterlace> > Barycentre (myMesh_P.getBarycenter(Support.get() ));
    const  double* Bary=Barycentre->getValue();  


    //localisation des barycentres des mailles de P dans les mailles de S.
    Meta_Wrapper<2> fromWrapper(MaS._NbNoeud,(double *)MaS._Coord,(MEDMEM::CONNECTIVITY*)myMesh_S.getConnectivityptr());
    Meta_Wrapper<2> toWrapper(MaP._NbMaille,(double *)Bary);
    Meta_Mapping<2> mapping(&fromWrapper,&toWrapper);
    mapping.Cree_Mapping(0);
    for(int i=0;i<MaP._NbMaille;i++)
	localis.push_back(mapping.Get_Mapping()[i]+1);


    if (_SecondMethodOfLocalisation)
    {
	if(_PrintLevel)
	    cout << endl << "Option de localisations complémentaires" << endl;
	/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
	//on vérifie que tout les barycentres ont obtenu un n° de maille 
	//non nul.
	MEDMEM::INTERPOLATION<2> monInterpol_S(myMesh_S);
	for(int i_P=0;i_P<MaP._NbMaille;i_P++)
	{
	    if(localis[i_P]<=0)
	    {
		//debug cout << endl << "------------------------------------------------" << endl << "Localise : barycentre maille " << i_P << " en dehors" << endl; 
		int Test=0;

		int NP_1=MaP._Connect[3*i_P];
		int NP_2=MaP._Connect[3*i_P+1];
		int NP_3=MaP._Connect[3*i_P+2];

		double Coord_bary_i_P[2]={Bary[2*i_P],Bary[2*i_P+1]};
		int Vois_N=monInterpol_S.getNearestNode(Coord_bary_i_P);
		int Ni=MaS._ReversNConnectIndex[Vois_N-1];
		int Nf=MaS._ReversNConnectIndex[Vois_N];
		int diff=Nf-Ni;

		for(int j=0;j<diff;j++)
		{  
		    int N_Maille=MaS._ReversNConnect[Ni-1+j];
		    int NS_1=MaS._Connect[3*(N_Maille-1)];
		    int NS_2=MaS._Connect[3*(N_Maille-1)+1];
		    int NS_3=MaS._Connect[3*(N_Maille-1)+2];


		    //debug cout << "mailles sources testées : " << N_Maille << endl;
		    vector<double> Inter=intersec_de_triangle(MaS._Coord+2*(NS_1-1),MaS._Coord+2*(NS_2-1),

			    MaS._Coord+2*(NS_3-1),MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1));

		    if(Inter.size()>0)
		    {
			//debug cout << "Localise : maille proche barycentre trouvée : " << N_Maille << endl; 
			Test=1;
			localis[i_P]=N_Maille;
			break;
		    }
		}
		if(Test==0)
		{
		    int NoeuMaillP[3]={NP_1,NP_2,NP_3};
		    for(int Num=0;Num<3;Num++)
		    {
			int Num_noeud=NoeuMaillP[Num];
			double coord_i_P_0[2];
			//VB
			coord_i_P_0[0]= MaP._Coord[2*(Num_noeud-1)];
			coord_i_P_0[1]=MaP._Coord[2*(Num_noeud-1)+1];
			//VB
			int Vois_B=monInterpol_S.getNearestNode(coord_i_P_0)+1;
			int Ni=MaS._ReversNConnectIndex[Vois_B-1];
			int Nf=MaS._ReversNConnectIndex[Vois_B];
			int diff=Nf-Ni;

			for(int j=0;j<diff;j++)
			{
			    int N_Maille=MaS._ReversNConnect[Ni-1+j];
			    int N_1=MaS._Connect[3*(N_Maille-1)];
			    int N_2=MaS._Connect[3*(N_Maille-1)+1];
			    int N_3=MaS._Connect[3*(N_Maille-1)+2];

			    //debug cout << "mailles sources testées : " << N_Maille << endl;
			    vector<double> Inter=intersec_de_triangle(MaS._Coord+2*(N_1-1),MaS._Coord+2*(N_2-1),
				    MaS._Coord+2*(N_3-1),MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1) );	

			    if(Inter.size()>0)
			    {
				//debug cout << "Localise : maille proche sommet " << Num+1 << " trouvée : " << N_Maille << endl; 
				Test=1;
				localis[i_P]=N_Maille;
				break;//on sort de la boucle sur les maille commune à un noeud de i_S
			    }
			}
			if(Test==1)
			{break;}//on sort de la boucle sur les noeuds de i_P
		    }   
		}
	    }
	}
    }
}









/* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ */
/*  fonction qui permet de remplir le vecteur donnant la surface d'intersection           */
/*  de la maille i_P du maillage P (projeté) avec la maille i_S du maillage S (support)  */   
/* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _  */


inline void INTERPOLATION_2D::rempli_vect_d_intersect(int i_P,int i_S1,const monMaillageP& MaP,const monMaillageS& MaS,	
	vector<map<int,double> >& Surface_d_intersect,FIELD<double>& myField_P)
{


    //on récupere le n° des noeuds.

    //debug cout << "\nintersect maille " << i_P << endl;
    int NP_1=MaP._Connect[3*(i_P-1)];
    int NP_2=MaP._Connect[3*(i_P-1)+1];
    int NP_3=MaP._Connect[3*(i_P-1)+2];


    //on calcule la surface de la maille i_P

    double Surf_i_P =Surf_Tri(MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),
	    MaP._Coord+2*(NP_3-1));

    double Surface = 0 ;
    vector<int> maill_deja_vu_S ;

    maill_deja_vu_S.push_back(i_S1);

    for (int M_S=0; M_S<maill_deja_vu_S.size(); M_S++)
    {
	if( abs(Surf_i_P-Surface)/Surf_i_P>_Precision && M_S!=maill_deja_vu_S.size() )
	{
	    int i_S=maill_deja_vu_S[M_S];

	    int NS_1=MaS._Connect[3*(i_S-1)];
	    int NS_2=MaS._Connect[3*(i_S-1)+1];
	    int NS_3=MaS._Connect[3*(i_S-1)+2];


	    vector<double> Inter=intersec_de_triangle(MaS._Coord+2*(NS_1-1),MaS._Coord+2*(NS_2-1),
		    MaS._Coord+2*(NS_3-1),MaP._Coord+2*(NP_1-1),MaP._Coord+2*(NP_2-1),MaP._Coord+2*(NP_3-1));




	    //on teste l'intersection.
	    int taille=Inter.size()/2;
	    //debug cout << "  -> maille source : " << i_S << " , nb intersection=" <<  taille << endl;

	    /* CAS1  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	    if(taille==0)
	    {int Rien=0;}

	    /* CAS 2  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
	    else if(taille==1 || taille==2)
	    {
		//on ne remplit pas le vecteur des correspondances n° maille-surfaces 
		//d'intersections mais on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=touv_maill_vois(i_S,MaS);
		for(int i=0;i< M_Vois.size();i++)
		{verif_maill_dans_vect(M_Vois[i],maill_deja_vu_S);}
	    }

	    /* CAS 3  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 
	    else if(taille==3)
	    {
		double Surf_inter = Surf_Poly(Inter) ;

		//on remplit le vecteur des correspondances n° maille-surfaces d'intersections.
		Surface_d_intersect[i_P-1][i_S]=Surf_inter;

		// on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=touv_maill_vois(i_S,MaS);
		for(int i_M=0;i_M<(M_Vois).size();i_M++)
		{verif_maill_dans_vect(M_Vois[i_M],maill_deja_vu_S);}

		Surface = Surface + Surf_inter ; 
	    }

	    /* CAS 4  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

	    else //taille>3
	    {
		vector<double> Poly_Inter=reconstruct_polygon(Inter);

		double Surf_inter =Surf_Poly(Poly_Inter) ;

		//on remplit le vecteur des correspondances n° maille-surfaces d'intersection
		(Surface_d_intersect[i_P-1])[i_S]=Surf_inter;

		// on récupère le numéro des mailles voisines de la maille i_S.
		vector<int> M_Vois=touv_maill_vois(i_S,MaS);
		for(int i_M=0;i_M<(M_Vois).size();i_M++)
		{verif_maill_dans_vect(M_Vois[i_M],maill_deja_vu_S);}

		Surface = Surface + Surf_inter ; 
	    }
	}
	//debug cout << "     surface = " << Surface << endl << flush;
    }

    /* _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on rentre la fraction totale de la maille i_P qui a été considérer lors de l'interpolation.
    double Fract=Surface/Surf_i_P;
    myField_P.setValueIJ(i_P,1,Fract);

}


/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
//fonction principale pour interpoler deux maillages triangulaires.
vector<map<int,double> > INTERPOLATION_2D::interpol_maillages(const MEDMEM::MESH& myMesh_S,
                                                              const MEDMEM::MESH& myMesh_P)
{


    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    // on vérifie que les deux maillages sont formés de triangles.
    int NumberOfCellsTypes_S = myMesh_S.getNumberOfTypes(MED_CELL);
    int NumberOfCellsTypes_P = myMesh_P.getNumberOfTypes(MED_CELL);
    if  ( NumberOfCellsTypes_S != 1  || NumberOfCellsTypes_P != 1)
    { cout<<"l'un au moins des deux maillages n'est pas triangulaires"<<endl;}
    string* Type_S = myMesh_S.getCellTypeNames(MED_CELL);
    string* Type_P = myMesh_P.getCellTypeNames(MED_CELL);
    if ( Type_S[0] != "MED_TRIA3" || Type_P[0] != "MED_TRIA3")
    { cout<<"l'un au moins des deux maillages n'est pas triangulaires"<<endl;}
    //VB
    delete[] Type_S;
    delete[] Type_P;

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    monMaillageS MaS;
    monMaillageP MaP;

    MaS._NbNoeud = myMesh_S.getNumberOfNodes() ;
    MaP._NbNoeud = myMesh_P.getNumberOfNodes() ;
    MaS._NbMaille =myMesh_S.getNumberOfElements(MED_CELL,MED_TRIA3);
    MaP._NbMaille =myMesh_P.getNumberOfElements(MED_CELL,MED_TRIA3);

    //on charge les connectivités des deux maillages.
    MaS._Connect =myMesh_S.getConnectivity(MED_FULL_INTERLACE, 
	    MED_NODAL, MED_CELL, MED_TRIA3) ;
    MaP._Connect =myMesh_P.getConnectivity(MED_FULL_INTERLACE, 
	    MED_NODAL, MED_CELL, MED_TRIA3) ;

    //on charge les coordonnées des noeuds.
    MaS._Coord = myMesh_S.getCoordinates(MED_FULL_INTERLACE);
    MaP._Coord = myMesh_P.getCoordinates(MED_FULL_INTERLACE);

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 
    //on a besoin de recupérer connectivité inverse de S.
    MaS._ReversNConnect=
	myMesh_S.getReverseConnectivity(MED_NODAL);
    MaS._ReversNConnectIndex=
	myMesh_S.getReverseConnectivityIndex(MED_NODAL);


    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */


    int SpaceDimension_S = myMesh_S.getNumberOfNodes() ;
    int SpaceDimension_P = myMesh_P.getNumberOfNodes() ;

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on cherche la dimension caractéristique des maillages
    vector<vector<double> > BoxS=myMesh_S.getBoundingBox();
    vector<vector<double> > BoxP=myMesh_P.getBoundingBox();
    double AreaS=sqrt((BoxS[1][0]-BoxS[0][0])*(BoxS[1][0]-BoxS[0][0])+(BoxS[1][1]-BoxS[0][1])*(BoxS[1][1]-BoxS[0][1]));
    MaS._DimCaracteristic=sqrt(4./sqrt(3.)*AreaS/MaS._NbMaille);
    double AreaP=sqrt((BoxP[1][0]-BoxP[0][0])*(BoxP[1][0]-BoxP[0][0])+(BoxP[1][1]-BoxP[0][1])*(BoxP[1][1]-BoxP[0][1]));
    MaP._DimCaracteristic=sqrt(4./sqrt(3.)*AreaP/MaP._NbMaille);
    _DimCaracteristic=min(MaS._DimCaracteristic,MaP._DimCaracteristic);
    if (_PrintLevel)
    {
	cout<<"recherche de la dimension caractéristique des maillages 2D :"<<endl;
	cout<<"  - dimension caractéristique du maillage source : "<<MaS._DimCaracteristic<<endl;
	cout<<"  - dimension caractéristique du maillage projeté: "<<MaS._DimCaracteristic<<endl;
	cout<<"  - d'où la dimension caractéristique: "<<_DimCaracteristic<<endl;
    }
    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on cherche pour chaque maille i_P du maillage projetté
    // une maille i_S du maillage S qui chevauche la maille i_P.

    vector<int> localis;
    localis.reserve(MaP._NbMaille);
    MEDMEM::MESH* ptr_S = const_cast<MEDMEM::MESH*>(&myMesh_S);
    MEDMEM::MESH* ptr_P = const_cast<MEDMEM::MESH*>(&myMesh_P);
    
    cout << "INTERPOLATION_2D::local_iP_dansS"<<endl;
    local_iP_dans_S(*ptr_S,*ptr_P,MaP,MaS,localis);

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //on crée un tableau où l'on rentrera la valeurs des intersections.
    cout << "INTERPOLATION_2D::calcul intersec"<<endl;
    map<int,double> MAP_init;
    vector<map<int,double> > Surface_d_intersect(MaP._NbMaille,MAP_init);//on initialise.

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //création d'un champ pour rentrer les fractions de mailles considérées.

    std::auto_ptr<SUPPORT> mySupport_P (new SUPPORT(const_cast<MEDMEM::MESH*>(&myMesh_P),"Support on all CELLS",MED_CELL) );
    MEDMEM::FIELD<double> myField_P(mySupport_P.get(),1);
    string NameF=" M2 dans M1" ;
    myField_P.setName(NameF);
    string NameC="fracSurf";
    string ComponentsNames[1]={NameC};
    myField_P.setComponentsNames(ComponentsNames);
    myField_P.setComponentsDescriptions(ComponentsNames);
    string ComponentsUnits[1]={"%"};
    myField_P.setMEDComponentsUnits(ComponentsUnits);

    /*à remplacer par:
      WriteFractSurfInFile(vector<<vector<pair<int,double> > >& FractVol,MEDMEM::MESH* myMesh1,
      MEDMEM::MESH* myMesh2,string FileName,string NameMeshP,string NameMeshS)
      */

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    //boucle sur les mailles de P.
    //Coeur de l'algorithme

    for(int i_P=0; i_P<MaP._NbMaille ; i_P++)
    {
	int i_S_init=localis[i_P];
	if(i_S_init>0)
	    rempli_vect_d_intersect(i_P+1,i_S_init,MaP,MaS,Surface_d_intersect,myField_P);

    }

    if (_PrintLevel >= 2)
    {
	cout<<endl<<"Impression des surfaces d'intersection:"<<endl<<endl;
	cout<<"(maille source, maille cible):surface d'intersection"<<endl;
	for(int i=0; i< Surface_d_intersect.size();i++)
	{ 
	    if(Surface_d_intersect[i].size()!=0)
	    {
		map<int,double>::iterator surface;
		for( surface=Surface_d_intersect[i].begin();surface!=Surface_d_intersect[i].end();surface++)
		    cout<<"    ("<<i+1<<" , "<<(*surface).first<<")"<<" : "<<(*surface).second<<endl;
	    }
	}  
    }

//    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
//    //On sauvegarde le champ crée sur la maillage P.
//    if (_PrintLevel>=3)
//    {
////	string NameFile="fractSurf.med";
////	int id1=myMesh_P.addDriver(MED_DRIVER,NameFile,"M1");
////	int id2=myMesh_S.addDriver(MED_DRIVER,NameFile,"M2");
////	int id3=myField_P.addDriver(MED_DRIVER,NameFile,NameF);
////	myMesh_P.write(id1);
////	myMesh_S.write(id2);
////	myField_P.write(id3);
////
////	cout<<endl<<"Pour chaque mailles de " <<myMesh_S.getName();
////	cout<<"Impression de la fraction surfaciques totale dans le maillage : "<<myMesh_P.getName()<<endl; 
////	for(int i=0;i<MaP._NbMaille;i++)
////	    cout<<"maille n°"<<i+1<<" de "<<myMesh_P.getName()<<": "<<myField_P.getValueIJ(i+1,1)<<endl;
//    }

    /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
    return Surface_d_intersect;

}





/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
/*  fonction qui écrit les résultats d'annelise dans un fichier:      */
/*  pour chaque maille du maillage 1 on stoque la fraction volumique  */
/*          considéré lors de l'algorithme                            */
/*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

void WriteFractSurfInFile(MEDMEM::FIELD<double>& FractSurf,MEDMEM::MESH* myMesh_P,
	MEDMEM::MESH* myMesh_S,string NameMesh_P,string NameMesh_S)
{
    //On sauvegarde le champ crée sur la maillage P.
    string FileName="FracSurf"+NameMesh_S+"_to_"+NameMesh_P;
    int id1=(*myMesh_P).addDriver(MED_DRIVER,FileName,"MP");
    int id2=(*myMesh_S).addDriver(MED_DRIVER,FileName,"MS");
    int id3=FractSurf.addDriver(MED_DRIVER,FileName,"% MP");
    (*myMesh_P).write(id1);
    (*myMesh_S).write(id2);
    FractSurf.write(id3);
}
