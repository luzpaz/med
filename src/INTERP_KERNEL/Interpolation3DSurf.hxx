#ifndef _INTERPOLATION_3D_SURF_HXX_
#define _INTERPOLATION_3D_SURF_HXX_

#include "MEDMEM_Mesh.hxx"
#include "Interpolation.hxx"

namespace MEDMEM
{
  class Interpolation3DSurf : public Interpolation
  {
  private: 
    IntersectionType _Intersection_type;
    double _MedianPlane;
    bool _Do_rotate;
    double _Precision;
    double _DimCaracteristic;
    double _Surf3DAdjustmentEps;
    int  _PrintLevel;

  public:
    Interpolation3DSurf();
    
    // geometric precision, debug print level, coice of the median plane, intersection etc ...
    void setOptions(double Precision, int PrintLevel, double MedianPlane, 
		    IntersectionType intersectionType, bool do_rotate);
    
    // Main function to interpolate triangular and qudadratic meshes
    std::vector<map<int, double> > interpolateMeshes(const MEDMEM::MESH& mesh1, const MEDMEM::MESH& mesh2);

  private: 

  };

};

#endif
