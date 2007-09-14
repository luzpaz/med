#ifndef _INTERPOLATION_HXX_
#define _INTERPOLATION_HXX_


#include <vector>
#include <map>

#include "MEDMEM_Mesh.hxx"

/**
 * \mainpage
 * Status : documentation of 3D - part of intersection matrix calculation more or less complete
 *
 *
 */

namespace MEDMEM
{

class Interpolation
{
public:
	Interpolation(){}
	virtual ~Interpolation(){}

	//interpolation of two triangular meshes.
	virtual std::vector<std::map<int, double> > interpol_maillages(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2)=0;

};

};
#endif
