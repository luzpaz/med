#ifndef MEDMEM_WRAPPER_CONNECTIVITY_HXX
#define MEDMEM_WRAPPER_CONNECTIVITY_HXX

#include "MEDMEM_Connectivity.hxx"

#include <vector>

//////////////////////////////////////////////////////////////////
///                                                            ///
///                DECLARATIONS ET CODE                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*               Wrapper_Med_Connectivity                */
/*                                                       */
/*********************************************************/

// obligé de faire de la recopie car MED n'utilise pas la numérotation standart C

class Wrapper_Med_Connectivity
{
protected :
	int * mailles;
	int nbr_mailles;
	vector<int> types;
	vector< int* > premier_pointeur;
	
public :	
	~Wrapper_Med_Connectivity() {if (mailles) delete [] mailles;}
	Wrapper_Med_Connectivity():mailles(NULL) {}
	Wrapper_Med_Connectivity(CONNECTIVITY * maillesmed)
		{
		const med_int * tab_sommets_mailles=maillesmed->getConnectivity(MED_NODAL,MED_CELL,MED_ALL_ELEMENTS);
		const med_int * med_connectivite=maillesmed->getConnectivityIndex(MED_FULL_INTERLACE,MED_CELL);
		const med_int * med_index=maillesmed->getValueIndex(MED_FULL_INTERLACE);
		nbr_mailles=maillesmed->getNumberOf(MED_CELL,MED_ALL_ELEMENTS);
		int size=med_index[nbr_mailles]-med_index[0];
		types.resize(nbr_mailles);
		premier_pointeur.resize(nbr_mailles);
		mailles=new int[size];
		int i;
		for (i=0;i<size;i++) 
			{
			mailles[i]=tab_sommets_mailles[i]-1;
			}
		for (i=0;i<nbr_mailles;i++)
			{
			types[i]=maillesmed->getElementType(MED_CELL,i+1); // A VERIFIER : le +1
			premier_pointeur[i]=&mailles[med_index[i]-1]; // A VERIFIER : la formule
			}
		}
	// Méthodes de la politique
	inline int SIZE() {return nbr_mailles;}
	inline int DONNE_TYPE_MAILLE(int i) {return types[i];}
	inline int * DONNE_PREMIER_POINTEUR(int i) {return premier_pointeur[i];}
};


#endif
