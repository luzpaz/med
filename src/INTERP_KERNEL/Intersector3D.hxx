#ifndef __INTERSECTOR_3D_HXX__
#define __INTERSECTOR_3D_HXX__

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"

#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"
#include "MeshUtils.hxx"
#include "Intersector.hxx"

#include <vector>

using namespace MEDMEM;
using namespace MED_EN;
using namespace INTERP_UTILS;

namespace MEDMEM
{

  class Intersector3D : public Intersector
  {

  public : 
    Intersector3D(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh);

    ~Intersector3D();

    double intersectCells(int srcCell, int targetCell);

    void triangulate(const medGeometryElement type, const int element, const MEDMEM::MESH& mesh, std::vector<TransformedTriangle>& triangles, const TetraAffineTransform& T) const;

  private:
    const MEDMEM::MESH& _srcMesh;
    const MEDMEM::MESH& _targetMesh;

  };
};


#endif
