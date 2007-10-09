#ifndef _GENERIC_INTERSECTOR_HXX_
#define _GENERIC_INTERSECTOR_HXX_

#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"

namespace MEDMEM 
{
  class MESH;
  template <int DIM>
  class GenericIntersector: public PlanarIntersector
  {
  public:
    GenericIntersector (const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
			const double DimCaracteristic, double Precision, double MedianPlane,
			bool do_rotate, int PrintLevel);
    double intersectCells(int icell_A, int icell_B, int nb_NodesA, int nb_NodesB);
    
  private :
    const int * _Connect_A;
    const int * _Connect_B;
    const int *_Conn_index_A;
    const int *_Conn_index_B;
    const double * _Coords_A;
    const double * _Coords_B;
    double _Epsilon;
    double _Precision;
    double _MedianPlane;
    bool _Do_rotate;
    int _PrintLevel;
  };
}

#endif
