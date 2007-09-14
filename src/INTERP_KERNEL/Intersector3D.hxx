#ifndef __INTERSECTOR_3D_HXX__
#define __INTERSECTOR_3D_HXX__

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"

#include "Intersector.hxx"

#include <vector>

namespace INTERP_UTILS
{
  class TransformedTriangle;
  class TetraAffineTransform;
};

namespace MEDMEM
{
  /** 
   * \brief OBSOLETE
   * Class calculating the volume of intersection of individual 3D elements.
   *
   */
  class Intersector3D : public Intersector
  {

  public : 
    Intersector3D(const MESH& srcMesh, const MESH& targetMesh);

    virtual ~Intersector3D();

    virtual double intersectCells(int srcCell, int targetCell);
    
    mutable int filtered;

  private:

    //inline bool isTriangleOutsideTetra(double* points) const;
    inline void checkIsOutside(const double* pt, bool* isOutside) const;

    void triangulate(const MED_EN::medGeometryElement type, const int element, std::vector<INTERP_UTILS::TransformedTriangle>& triangles, const INTERP_UTILS::TetraAffineTransform& T) const;

    const MESH& _srcMesh;
    const MESH& _targetMesh;
    
  };

//   inline bool Intersector3D::isTriangleOutsideTetra(double* points) const
//   {
//     const bool leftX = points[0] <= 0.0 && points[3] <= 0.0 && points[6] <= 0.0;
//     const bool rightX = points[0] >= 1.0 && points[3] >= 1.0 && points[6] >= 1.0;
//     const bool leftY = points[1] <= 0.0 && points[4] <= 0.0 && points[7] <= 0.0;
//     const bool rightY = points[1] >= 1.0 && points[4] >= 1.0 && points[7] >= 1.0;
//     const bool leftZ = points[2] <= 0.0 && points[5] <= 0.0 && points[8] <= 0.0;
//     const bool rightZ = points[2] >= 1.0 && points[5] >= 1.0 && points[8] >= 1.0;
//     const double h[3] = 
//       {
// 	1 - points[0] - points[1] - points[2],
// 	1 - points[3] - points[4] - points[5],
// 	1 - points[6] - points[7] - points[8]
//       };
//     const bool leftH = h[0] <= 0.0 && h[1] <= 0.0 && h[2] <= 0.0;
//     const bool rightH = h[0] >= 1.0 && h[1] >= 1.0 && h[2] >= 1.0;

//     return leftX || rightX || leftY || rightY || leftZ || rightZ || leftH || rightH;
//   }

  inline void Intersector3D::checkIsOutside(const double* pt, bool* isOutside) const
  {
    isOutside[0] = isOutside[0] && (pt[0] <= 0.0);
    isOutside[1] = isOutside[1] && (pt[0] >= 1.0);
    isOutside[2] = isOutside[2] && (pt[1] <= 0.0);
    isOutside[3] = isOutside[3] && (pt[1] >= 1.0);
    isOutside[4] = isOutside[4] && (pt[2] <= 0.0);
    isOutside[5] = isOutside[5] && (pt[2] >= 1.0);
    isOutside[6] = isOutside[6] && (1.0 - pt[0] - pt[1] - pt[2] <= 0.0);
    isOutside[7] = isOutside[7] && (1.0 - pt[0] - pt[1] - pt[2] >= 1.0);
  }

};


#endif
