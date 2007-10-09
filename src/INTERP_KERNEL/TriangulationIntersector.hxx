#ifndef _TRIANGULATION_INTERSECTOR_HXX_
#define _TRIANGULATION_INTERSECTOR_HXX_

#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"

namespace MEDMEM 
{
  class MESH;
  template <int DIM>
  class TriangulationIntersector: public PlanarIntersector
  {
  public:
    TriangulationIntersector (const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
			      double DimCaracteristic, double Precision, double MedianPlane, int PrintLevel);
    // virtual ~Intersector();// Warum virtual ?
    double intersectCells(int icell_A, int icell_B, int nb_NodesA, int nb_NodesB);
    
  private :
    const int * _Connect_A;
    const int * _Connect_B;
    const double * _Coords_A;
    const double * _Coords_B;
    const int *_Conn_index_A;
    const int *_Conn_index_B;
    double _DimCaracteristic;
    double _Precision;
    double _MedianPlane;
    bool _do_rotate;
    int _PrintLevel;
  };
}

#endif
