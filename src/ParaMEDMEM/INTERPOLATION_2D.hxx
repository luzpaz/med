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
#include "INTERPOLATION.hxx"
#include "MEDMEM_Interpolation.hxx"


#include <cmath>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <vector>
#include <complex>


namespace ParaMEDMEM_utils
{
	struct monMaillageS;
	struct monMaillageP;
};

namespace ParaMEDMEM
{

	class INTERPOLATION;

class INTERPOLATION_2D : public INTERPOLATION
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


	//localise le barycentre des mailles de P dans le maillage S
	void local_iP_dans_S(MEDMEM::MESH& myMesh_S,MEDMEM::MESH& myMesh_P,
		ParaMEDMEM_utils::monMaillageP& MaP,ParaMEDMEM_utils::monMaillageS& MaS,vector<int>& localis );


	//fonction qui permet de remplir le vecteur donnant la surface d'intersection 
	//de la mailles i_P du maillage projetté avec la maille i_S du maillage support.
	inline void rempli_vect_d_intersect(int i_P,int i_S,const ParaMEDMEM_utils::monMaillageP& MaP,const ParaMEDMEM_utils::monMaillageS& MaS,
		vector<map<int,double> >& Surface_d_intersect,
		FIELD<double>& myField_P);


	//fonction principale pour interpoler deux maillages triangulaires.
	std::vector<map<int, double> > interpol_maillages(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);

    private: 


};

};

#endif
