#ifndef _PLANAR_INTERSECTOR_HXX_
#define _PLANAR_INTERSECTOR_HXX_

#include "TranslationRotationMatrix.hxx"

namespace MEDMEM 
{
  class MESH;
  class PlanarIntersector
  {
  public:
    PlanarIntersector (const MEDMEM::MESH& mesh_A, const MEDMEM::MESH& mesh_B, 
		       double DimCaracteristic, double Precision, double MedianPlane){};
    virtual ~ PlanarIntersector(){};

    //Tool for cell intersection
    virtual double intersectCells(int icell_A, int icell_B, int nb_NodesA, int nb_NodesB)=0;

    //Tool for cell filtering
    template <int DIM>
    void createBoundingBoxes(const MEDMEM::MESH& mesh, vector<double>& bbox);
    template <int DIM>
    void adjustBoundingBoxes(vector<double>& bbox, double Surf3DAdjustmentEps );
    template <int DIM>
    void getElemBB(double* bb, const MEDMEM::MESH& mesh, int iP, int nb_nodes);

  protected :
    void projection( vector< double>& Coords_A, vector< double>& Coords_B, 
		     int nb_NodesA, int nb_NodesB, double epsilon, double median_plane, bool do_rotate);
    void rotate3DTriangle( double* PP1, double*PP2, double*PP3,
			  TranslationRotationMatrix& rotation_matrix);

  };
}

#endif
