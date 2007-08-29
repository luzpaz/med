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
  class MeshElement;
  class MeshRegion;
};

class Interpolation3DTest;

namespace MEDMEM 
{
  /**
   * Class used to calculate the volumes of intersection between the elements of two 3D meshes.
   * 
   */
  class Interpolation3D : public Interpolation
  {

    friend class ::Interpolation3DTest;


  public :

    Interpolation3D();
    virtual ~Interpolation3D();

    virtual IntersectionMatrix interpol_maillages(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh);

  private:
    
    void calculateIntersectionVolumes(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, IntersectionMatrix& matrix);

  };

};

#endif
