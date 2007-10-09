#ifndef _CONVEX_INTERSECTOR_HXX_
#define _CONVEX_INTERSECTOR_HXX_

#include "MEDMEM_Mesh.hxx"
#include "InterpolationUtils.hxx"
#include "PolygonAlgorithms.hxx"

namespace MEDMEM 
{
  template <int DIM>
  class ConvexIntersector: public PlanarIntersector
  {
  public:
    ConvexIntersector(const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
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
//     vector< vector< const double *, int > _Ordered_nodes_A;
//     vector< vector< const double *, int > _Ordered_nodes_B;
//     vector< vector< double > >  _Coords_A;
//     vector< vector< double > >  _Coords_B;
//     vector< int > _Nb_dist_nodes_A;
//     vector< int > _Nb_dist_nodes_B;
    double _Epsilon;
    double _Precision;
    double _MedianPlane;
    bool _Do_rotate;
    int _PrintLevel;
  };
}

#endif
