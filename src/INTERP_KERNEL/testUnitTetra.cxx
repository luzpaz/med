#include "MEDMEM_Mesh.hxx"
#include "Interpolation3D.hxx"
#include "./Test/TestingUtils.hxx"

using namespace MEDMEM;

// simple test to see if a certain mesh is diagonal when intersected with itself

int main()
{
	MEDMEM::MESH mesh1(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	MEDMEM::MESH mesh2(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	Interpolation3D interpolator;
	vector<map<int,double> > matrix = interpolator.interpol_maillages(mesh1,mesh2);
	dumpIntersectionMatrix(matrix);
}
