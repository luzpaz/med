#include "MEDMEM_Mesh.hxx"
#include "Interpolation3D.hxx"

using namespace MEDMEM;

// simple test to see if a certain mesh is diagonal when intersected with itself

int main()
{
	MEDMEM::MESH mesh1(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	MEDMEM::MESH mesh2(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	Interpolation3D interpolator;
	vector<map<int,double> > matrix = interpolator.interpol_maillages(mesh1,mesh2);

	// dump
	int i = 0;
	std::cout << "Intersection matrix is " << endl;
	for(IntersectionMatrix::const_iterator iter = matrix.begin() ; iter != matrix.end() ; ++iter)
	  {
	    for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	      {
		
		std::cout << "V(" << i << ", " << iter2->first << ") = " << iter2->second << endl;
		
	      }
	    ++i;
	  }
	std::cout << "Sum of volumes = " << sumVolume(m) << std::endl;
}
