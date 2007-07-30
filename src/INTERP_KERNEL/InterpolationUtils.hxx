#ifndef _INTERPOLATION_UTILS_HXX_
#define _INTERPOLATION_UTILS_HXX_


#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"


#include <cmath>
#include <map>
#include <algorithm>
#include <vector>

namespace INTERP_UTILS
{
  //	const double HUGE=1e300;
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

  // 	void verif_point_dans_vect(const double* P, vector<double>& V,double dim_caracteristic, double precision );
  // 	inline double Surf_Tri(const double* P_1,const double* P_2,const double* P_3);
  // 	inline double norme_vecteur(const double* P_1,const double* P_2);
  // 	inline double mon_determinant(const double* P_1,
  // 																const double*  P_2,
  // 																const double* P_3);
  // 	vector<double> calcul_cos_et_sin(const double* P_1,
  // 																	 const double* P_2,
  // 																	 const double* P_3);
  // 	inline vector<double> bary_poly(const vector<double>& V);
  // 	bool point_dans_triangle(const double* P_0,const double* P_1,
  // 													 const double* P_2,const double* P_3);
  // 	double  Surf_Poly(const vector<double>& Poly);
  // 	vector<double> reconstruct_polygon(const vector<double>& V);
	
  // 	void rajou_sommet_triangl(const double* P_1,
  // 														const double* P_2,
  // 														const double* P_3,
  // 														const double* P_4,
  // 														const double* P_5,
  // 														const double* P_6,
  // 														vector<double>& V, 
  // 														double dim_caracteristic, 
  // 														double precision);
	
	
  // 	inline void inters_de_segment(const double * P_1,
  // 																const double * P_2,
  // 																const double * P_3,
  // 																const double * P_4,
  // 																vector<double>&  Vect, 
  // 																double dim_caracteristic, 
  // 																double precision);
  // 	vector<double> intersec_de_triangle(const double* P_1,const double* P_2,
  // 																			const double* P_3,const double* P_4,const double* P_5,const double* P_6, double dim_caracteristic, double precision);
  // 	vector<int> touv_maill_vois(int i_S,const monMaillageS& MaS);
  // 	void verif_point_dans_vect(const double* P, vector<double>& V,double dim_caracteristic, double precision );
  // 	void WriteFractSurfInFile(MEDMEM::FIELD<double>& FractSurf,MEDMEM::MESH* myMesh_P,
  // 														MEDMEM::MESH* myMesh_S,string NameMesh_P,string NameMesh_S);





  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*   calcul la surface d'un triangle                  */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline double Surf_Tri(const double* P_1,const double* P_2,const double* P_3)
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

  inline double mon_determinant(const double* P_1,
				const double*  P_2,
				const double* P_3)
  {
    double mon_det=(P_1[0]-P_3[0])*(P_2[1]-P_3[1])-(P_2[0]-P_3[0])*(P_1[1]-P_3[1]);
    return mon_det;
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/
  //calcul la norme du vecteur P1P2

  inline double norme_vecteur(const double* P_1,const double* P_2)
  {
    double X=P_1[0]-P_2[0];
    double Y=P_1[1]-P_2[1];
    double norme=sqrt(X*X+Y*Y);
    return norme;
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*         calcul le cos et le sin de l'angle P1P2,P1P3     */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline vector<double> calcul_cos_et_sin(const double* P_1,
					  const double* P_2,
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



  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*     calcul les coordonnées du barycentre d'un polygone   */ 
  /*     le vecteur en entrée est constitué des coordonnées   */
  /*     des sommets du polygone                              */                             
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline vector<double> bary_poly(const vector<double>& V)
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

  inline double  Surf_Poly(const vector<double>& Poly)
  { 

    double Surface=0;
    for (int i=0; i<(Poly.size())/2-2; i++)
      {
	double Surf=Surf_Tri( &Poly[0],&Poly[2*(i+1)],&Poly[2*(i+2)] ); 
	Surface=Surface + Surf ;
      }
    return Surface ;
  }


  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*   fonction qui teste si un point est dans une maille     */
  /*   point: P_0                                             */
  /*   P_1, P_2, P_3 sommet des mailles                       */   
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline bool point_dans_triangle(const double* P_0,const double* P_1,
				  const double* P_2,const double* P_3)
  {

    bool A=false;
    double det_1=mon_determinant(P_1,P_3,P_0);
    double det_2=mon_determinant(P_3,P_2,P_0);
    double det_3=mon_determinant(P_2,P_1,P_0);
    if( (det_1>=0 && det_2>=0 && det_3>=0) || (det_1<=0 && det_2<=0 && det_3<=0) )
      {
	A=true;
      }

    return A;
  }



  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*fonction pour vérifier qu'un point n'a pas déja été considérer dans   */ 
  /*      le vecteur et le rajouter au vecteur sinon.                     */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline void verif_point_dans_vect(const double* P, vector<double>& V,double dim_caracteristic, double precision )
  {
    //    int taille=V.size();
    //   for(int i=0;i<taille/2;i++) 
    //     {
    //       double dx=P[0]-V[2*i];
    //       dx=dx>0.0?dx:-dx;
    //       if (dx>_Precision)
    // 	continue;
    //       double dy=P[1]-V[2*i+1];
    //       dy=dy>0.0?dy:-dy;
    //       if (dy<_Precision)
	
    // 	return;
      
    //     }
    //   V.push_back(P[0]);
    //   V.push_back(P[1]);
  
    int taille=V.size();
    bool isPresent=false;
    for(int i=0;i<taille/2;i++) 
      {
	if (sqrt(((P[0]-V[2*i])*(P[0]-V[2*i])+(P[1]-V[2*i+1])*(P[1]-V[2*i+1])))/dim_caracteristic<precision)
	  isPresent=true;
      
      }
    if(!isPresent)
      {
      
	V.push_back(P[0]);
	V.push_back(P[1]);    
      }
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /* fonction qui rajoute les sommet du triangle P dans le vecteur V        */ 
  /* si ceux-ci sont compris dans le triangle S et ne sont pas déjà dans    */
  /* V.                                                                     */
  /*sommets de P: P_1, P_2, P_3                                             */
  /*sommets de S: P_4, P_5, P_6                                             */  
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */ 

  inline void rajou_sommet_triangl(const double* P_1,const double* P_2,const double* P_3,
				   const double* P_4,const double* P_5,const double* P_6,vector<double>& V, double dim_caracteristic, double precision)
  {

    bool A_1=INTERP_UTILS::point_dans_triangle(P_1,P_4,P_5,P_6);
    if(A_1)
      {verif_point_dans_vect(P_1,V,dim_caracteristic, precision);
      //   if (V.size()==1)
      //       {
      // 	// all nodes are necessarily in the triangle
      // 	verif_point_dans_vect(P_2,V);
      // 	verif_point_dans_vect(P_3,V);
      // 	return;
      //       }
      }
    bool A_2=INTERP_UTILS::point_dans_triangle(P_2,P_4,P_5,P_6);
    if(A_2)
      {verif_point_dans_vect(P_2,V,dim_caracteristic,precision);}

    bool A_3=INTERP_UTILS::point_dans_triangle(P_3,P_4,P_5,P_6);
    if(A_3)
      {verif_point_dans_vect(P_3,V,dim_caracteristic, precision);}
          
  }


  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _*/
  /*  calcul de l'intersection de deux segments: segments P1P2 avec P3P4      */
  /*  . Si l'intersection est non nulle et si celle-ci n'est                  */
  /*  n'est pas déjà contenue dans Vect on la rajoute à Vect                  */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _*/ 

  inline void inters_de_segment(const double * P_1,const double * P_2,
				const double * P_3,const double * P_4,vector<double>&  Vect, 
				double dim_caracteristic, double precision)
  {


    // calcul du déterminant de P1_1P_2 et P_3P_4.
    double det=(P_2[0]-P_1[0])*(P_4[1]-P_3[1])-(P_4[0]-P_3[0])*(P_2[1]-P_1[1]);


    if(abs(det)/dim_caracteristic>precision)
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
		verif_point_dans_vect(P_0,Vect, dim_caracteristic, precision);

	      }
	  }
      }
  }



  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/  
  /*      calcul l'intersection de deux triangles            */
  /* P_1, P_2, P_3: sommets du premier triangle              */
  /* P_4, P_5, P_6: sommets du deuxième triangle             */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _*/ 

  inline vector<double> intersec_de_triangle(const double* P_1,const double* P_2,
					     const double* P_3,const double* P_4,const double* P_5,const double* P_6, double dim_caracteristic, double precision)
  {

    //		cout << "  T1 : " << *P_1 << " , " << *(P_1+1) << " , " << *P_2 << " , " << *(P_2+1) << " , " << *P_3 << " , " << *(P_3+1)<< endl;
    // cout << "  T2 : " << *P_4 << " , " << *(P_4+1) << " , " << *P_5 << " , " << *(P_5+1) << " , " << *P_6 << " , " << *(P_6+1)<< endl;
    vector<double> Vect;

    inters_de_segment(P_1,P_2,P_4,P_5,Vect, dim_caracteristic, precision);
    inters_de_segment(P_1,P_2,P_5,P_6,Vect, dim_caracteristic, precision);
    inters_de_segment(P_1,P_2,P_6,P_4,Vect, dim_caracteristic, precision);
    inters_de_segment(P_2,P_3,P_4,P_5,Vect, dim_caracteristic, precision);
    inters_de_segment(P_2,P_3,P_5,P_6,Vect, dim_caracteristic, precision);
    inters_de_segment(P_2,P_3,P_6,P_4,Vect, dim_caracteristic, precision);
    inters_de_segment(P_3,P_1,P_4,P_5,Vect, dim_caracteristic, precision);
    inters_de_segment(P_3,P_1,P_5,P_6,Vect, dim_caracteristic, precision);
    inters_de_segment(P_3,P_1,P_6,P_4,Vect, dim_caracteristic, precision);
    rajou_sommet_triangl(P_1,P_2,P_3,P_4,P_5,P_6,Vect, dim_caracteristic, precision);
    rajou_sommet_triangl(P_4,P_5,P_6,P_1,P_2,P_3,Vect, dim_caracteristic, precision);
    //debug cout << "  Inter : ";
    //debug for (int i=0; i<Vect.size(); ++i)
    //debug cout << Vect[i] << "  ";
    //debug cout << endl << endl;

    return Vect;
  }

  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ */ 
  /*fonction pour trouver les mailles voisines d'une maille triangle.*/
  /* (mailles voisines par les faces)                                */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ */ 

  inline vector<int> touv_maill_vois(int i_S,const monMaillageS& MaS)
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

  inline void verif_maill_dans_vect(int Num, vector<int>& V)
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
  /* fonction pour reconstituer un polygone convexe à partir  */
  /*              d'un nuage de point.                        */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */  

  inline vector<double> reconstruct_polygon(const vector<double>& V)
  {

    int taille=V.size();

    //VB : why 6 ?

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
	multimap<double,int> Ordre;
	for(int i=0;i<taille/2;i++)	
	  {Ordre.insert(make_pair(angle[i],i));}
	multimap <double,int>::iterator mi;
	for(mi=Ordre.begin();mi!=Ordre.end();mi++)
	  {
	    int j=(*mi).second;
	    Pt_ordonne.push_back(V[2*j]);
	    Pt_ordonne.push_back(V[2*j+1]);
	  }
	return Pt_ordonne;
      }
  }


  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */
  /*  fonction qui écrit les résultats d'annelise dans un fichier:      */
  /*  pour chaque maille du maillage 1 on stoque la fraction volumique  */
  /*          considéré lors de l'algorithme                            */
  /*_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ */

  inline void WriteFractSurfInFile(MEDMEM::FIELD<double>& FractSurf,MEDMEM::MESH* myMesh_P,
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

  inline void getElemBB(double* bb, const monMaillageP& MaP, int iP)
  {
    bb[0]=HUGE;bb[1]=-HUGE;
    bb[2]=HUGE;bb[3]=-HUGE;
    bb[4]=HUGE;bb[5]=-HUGE;
		
    for (int i=0; i<3; i++)
      {
	double x = MaP._Coord[(MaP._Connect[3*iP+i]-1)*3];
	double y = MaP._Coord[(MaP._Connect[3*iP+i]-1)*3+1];
	double z = MaP._Coord[(MaP._Connect[3*iP+i]-1)*3+2];
	bb[0]=(x<bb[0])?x:bb[0];
	bb[1]=(x>bb[1])?x:bb[1];
	bb[2]=(y<bb[2])?y:bb[2];
	bb[3]=(y>bb[3])?y:bb[3];
	bb[4]=(z<bb[4])?z:bb[4];
	bb[5]=(z>bb[5])?z:bb[5];
      }
		
  }


 

};
#endif
