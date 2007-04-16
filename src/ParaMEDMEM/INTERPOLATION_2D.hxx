#ifndef _INTERPOLATION_2D_HXX_
#define _INTERPOLATION_2D_HXX_


#include "MEDMEM_InterpolationHighLevelObjects.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Interpolation.hxx"


#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <complex>


struct monMaillageS;


struct monMaillageP;


class INTERPOLATION_2D
{


    private: 
	double _Precision;
	double _DimCaracteristic;
	int  _SecondMethodOfLocalisation;
	int  _PrintLevel;
	// Méthodes publiques
    public:

	INTERPOLATION_2D();


	// precision geometrique, choix de la methode comlementaire de localisation, niveau d'impressions
	void setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel);

	//pour calculer la surface d'un triangle.
	double Surf_Tri(const double* P_1,const double* P_2,
		const double* P_3);

	//calcul déterminant de trois points (cf doc CGAL)
	double mon_determinant(const double* P_1,const double*  P_2,
		const double* P_3);

	//calcul la norme d'un vecteur.
	double norme_vecteur(const double* P_1,const double* P_2);

	//calcul le cos et le sin.
	vector<double> calcul_cos_et_sin(const double* P_1,
		const double* P_2,const double* P_3);

	//fonction pour vérifier qu'un point n'a pas déja été considérer dans 
	//le vecteur et le rajouter au vecteur sinon.
	void verif_point_dans_vect(const double* P, vector<double>& V);

	//calcul les coordonnées du barycentre
	//d'un polygone
	vector<double> bary_poly(const vector<double>& V);

	//fonction qui calcul la surface d'un polygone
	double Surf_Poly(const vector<double>& Poly);

	//calcul de l'intersection de deux segments.
	void inters_de_segment(const double* P_1,const double* P_2,
		const double* P_3,const double* P_4,vector<double>& Vect);

	//fonction qui teste si un point est dans une maille
	int point_dans_triangle(const double* P_0,const double* P_1,
		const double* P_2,const double* P_3);

	//fonction qui rajouter les sommets du triangle P2P2P3 si ceux-ci sont compris
	// dans le triangle P4P5P6 et n'ont pas encore été considérer.
	//Rq:point du triangle donné dans ordre quelconque.
	void rajou_sommet_triangl(const double* P_1,const double* P_2,
		const double* P_3,const double* P_4,const double* P_5,
		const double* P_6,vector<double>& V);

	//calcul l'intersection de deux triangles (sommets du triangle donné dans
	//ordre quelconque)
	vector<double> intersec_de_triangle(const double* P_1,const double* P_2,
		const double* P_3,const double * P_4,const double * P_5,
		const double * P_6);  

	//fonction pour reconstituer un polygon convexe à partir
	//d'un nuage de point.
	vector<double> reconstruct_polygon(const vector<double>& V);

	//fonction pour trouver les mailles voisines d'une maille triangle.
	vector<int> touv_maill_vois(int i_S,const monMaillageS& MaS);

	//fonction pour vérifier qu'un n°de maille n'a pas déja été considérer
	// dans le vecteur et le rajouter au vecteur sinon.
	void INTERPOLATION_2D::verif_maill_dans_vect(int Num, vector<int>& V);


	//localise le barycentre des mailles de P dans le maillage S
	void local_iP_dans_S(MEDMEM::MESH& myMesh_S,MEDMEM::MESH& myMesh_P,
		monMaillageP& MaP,monMaillageS& MaS,vector<int>& localis );


	//fonction qui permet de remplir le vecteur donnant la surface d'intersection 
	//de la mailles i_P du maillage projetté avec la maille i_S du maillage support.
	inline void rempli_vect_d_intersect(int i_P,int i_S,const monMaillageP& MaP,const monMaillageS& MaS,
		vector<map<int,double> >& Surface_d_intersect,
		FIELD<double>& myField_P);


	//fonction principale pour interpoler deux maillages triangulaires.
	std::vector<map<int, double> > interpol_maillages(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);


	//  fonction qui écrit les résultats d'annelise dans un fichier:      
	//  pour chaque maille du maillage 1 on stoque la fraction volumique  
	//          considéré lors de l'algorithme                            





	void WriteFractSurfInFile(MEDMEM::FIELD<double>& FractSurf,MEDMEM::MESH* myMesh_P,
		MEDMEM::MESH* myMesh_S,string FileName,string NameMesh_P,string NameMesh_S);



	// MEDMEM::FIELD<double>* createField();
	// ...

    private: 


};

#endif
