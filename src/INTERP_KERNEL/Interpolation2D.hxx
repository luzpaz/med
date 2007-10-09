#ifndef _INTERPOLATION_2D_SURF_HXX_
#define _INTERPOLATION_2D_SURF_HXX_

#include "MEDMEM_Mesh.hxx"
#include "Interpolation.hxx"

namespace MEDMEM
{
  class Interpolation2D : public Interpolation
  {
  private: 
    double _Precision;
    double _DimCaracteristic;
    int  _PrintLevel;
    IntersectionType _Intersection_type;

  public:
    Interpolation2D();
    
    // geometric precision, debug print level, coice of the median plane, intersection etc ...
    void setOptions(double Precision, int PrintLevel, 
		    IntersectionType intersectionType);
    
    // Main function to interpolate triangular and qudadratic meshes
    std::vector<map<int, double> > interpolateMeshes(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);

  private: 

  };

};

#endif
