#ifndef __INTERPOLATION_3D_HXX__
#define __INTERPOLATION_3D_HXX__

// MEDMEM includes
#include "Interpolation.hxx"
#include "MEDMEM_Mesh.hxx"

// standard includes
#include <vector>
#include <map>


// typedefs
typedef std::vector< std::map< int, double > > IntersectionMatrix;

namespace INTERP_UTILS
{
  class MeshTestToolkit;
};

class Interpolation3DTest;

namespace MEDMEM 
{
  /**
   * \brief Class used to calculate the volumes of intersection between the elements of two 3D meshes.
   * 
   */
  class Interpolation3D : public Interpolation
  {
  public:
    //    friend class INTERP_UTILS::MeshTestToolkit;

    Interpolation3D();
    virtual ~Interpolation3D();

    virtual IntersectionMatrix interpol_maillages(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh);

  private:
    
    void calculateIntersectionVolumes(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, IntersectionMatrix& matrix);

  };

};

#endif
