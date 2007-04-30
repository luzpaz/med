#ifndef _INTERPOLATION_3D_SURF_HXX_
#define _INTERPOLATION_3D_SURF_HXX_



#include "MEDMEM_Mesh.hxx"
#include "INTERPOLATION_Utils.hxx"
#include "TranslationRotationMatrix.hxx"
#include "INTERPOLATION.hxx"

#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>

namespace ParaMEDMEM
{
	
	class INTERPOLATION_3D_surf : public INTERPOLATION
{


    private: 
	double _Precision;
	double _DimCaracteristic;
	int  _SecondMethodOfLocalisation;
	int  _PrintLevel;
	double _Surf3DAdjustmentEps;
	// Méthodes publiques
    public:

	INTERPOLATION_3D_surf();


	// precision geometrique, choix de la methode comlementaire de localisation, niveau d'impressions
	void setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel);



	//fonction principale pour interpoler deux maillages triangulaires.
	std::vector<map<int, double> > interpol_maillages(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);


    private: 

	//fonction qui permet de remplir le vecteur donnant la surface d'intersection 
	//de la mailles i_P du maillage projetté avec la maille i_S du maillage support.
	inline void rempli_vect_d_intersect(int i_P,int i_S,
																			const ParaMEDMEM_utils::monMaillageP& MaP,
																			const ParaMEDMEM_utils::monMaillageS& MaS,
																			vector<map<int,double> >& Surface_d_intersect);


	void createBoundingBoxes(const struct ParaMEDMEM_utils::monMaillageS mesh, vector<double>& bbox);

	void  rotate3DTriangle(const double* PP1, 
											 const double* PP2,
											 const double* PP3,
											 TranslationRotationMatrix& matrix);

	void adjustBoundingBoxes(vector<double>& bbox);


};

};

#endif
