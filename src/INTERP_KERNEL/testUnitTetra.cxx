#include "MEDMEM_Mesh.hxx"
#include "Interpolation3D.hxx"

#include <algorithm>

using namespace MEDMEM;
using namespace std;

double sumVolume(const IntersectionMatrix& m) 
{  
  vector<double> volumes;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  volumes.push_back(iter2->second);
	  //	  vol += std::abs(iter2->second);
	}
    }
  
  // sum in ascending order to avoid rounding errors

  sort(volumes.begin(), volumes.end());
  const double vol = accumulate(volumes.begin(), volumes.end(), 0.0);

  return vol;
}

// simple test to see if a certain mesh is diagonal when intersected with itself

void dumpIntersectionMatrix(const IntersectionMatrix& m) 
{
  int i = 0;
  std::cout << "Intersection matrix is " << endl;
  for(IntersectionMatrix::const_iterator iter = m.begin() ; iter != m.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  
	  std::cout << "V(" << i << ", " << iter2->first << ") = " << iter2->second << endl;
	  
	}
      ++i;
    }
  std::cout << "Sum of volumes = " << sumVolume(m) << std::endl;
}



int main()
{
	MEDMEM::MESH mesh1(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	MEDMEM::MESH mesh2(MED_DRIVER,"/home/vb144235/resources/DividedUnitTetraSimpler.med","DividedUnitTetraSimpler");
	Interpolation3D interpolator;
	vector<map<int,double> > matrix = interpolator.interpol_maillages(mesh1,mesh2);

	// dump
	dumpIntersectionMatrix(matrix);
}
