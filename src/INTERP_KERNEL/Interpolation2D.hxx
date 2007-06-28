#ifndef _INTERPOLATION2D_HXX_
#define _INTERPOLATION2D_HXX_

#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"




#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <complex>

// namespace MEDMEM 
// {
//   class MESH;
//   template <typename T> class FIELD;
// }

namespace INTERP_UTILS
{
	struct monMaillageS;
	struct monMaillageP;
};

namespace MEDMEM
{

	class Interpolation;

class Interpolation2D : public Interpolation
{

    private: 
	double _Precision;
	double _DimCaracteristic;
	int  _SecondMethodOfLocalisation;
	int  _PrintLevel;

	// Méthodes publiques
    public:

	Interpolation2D();

	// precision geometrique, choix de la methode comlementaire de localisation, niveau d'impressions
	void setOptions(double Precision, int SecondMethodOfLocalisation, int PrintLevel);


	//localise le barycentre des mailles de P dans le maillage S
	void local_iP_dans_S(MEDMEM::MESH& myMesh_S,MEDMEM::MESH& myMesh_P,
		INTERP_UTILS::monMaillageP& MaP,INTERP_UTILS::monMaillageS& MaS,vector<int>& localis );


	//fonction qui permet de remplir le vecteur donnant la surface d'intersection 
	//de la mailles i_P du maillage projetté avec la maille i_S du maillage support.
	inline void rempli_vect_d_intersect(int i_P,int i_S,const INTERP_UTILS::monMaillageP& MaP,const INTERP_UTILS::monMaillageS& MaS,
		vector<map<int,double> >& Surface_d_intersect,
		FIELD<double>& myField_P);


	//fonction principale pour interpoler deux maillages triangulaires.
	std::vector<map<int, double> > interpol_maillages(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);

    private: 


};

};

#endif
