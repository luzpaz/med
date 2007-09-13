#ifndef __INTERSECTOR_HEXA_HXX__
#define __INTERSECTOR_HEXA_HXX__

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "TargetIntersector.hxx"


namespace INTERP_UTILS
{

  class IntersectorTetra;

  /** 
   * Class representing a hexahedron, which allows 
   * 
   *
   */
  class IntersectorHexa : public TargetIntersector
  {

  public:

    enum SplittingPolicy { PLANAR_FACE_5 = 5, PLANAR_FACE_6 = 6, GENERAL_24 = 24, GENERAL_48 = 48 };

    IntersectorHexa(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, int targetCell, SplittingPolicy policy = GENERAL_48);
    
    ~IntersectorHexa();

    virtual double intersectSourceCell(int srcCell);

  private:

    void fiveSplit(const MEDMEM::MESH& srcMesh, const int* const subZone);
    
    void sixSplit(const MEDMEM::MESH& srcMesh, const int* const subZone);

    void calculateGeneral24Tetra(const MEDMEM::MESH& srcMesh);

    void calculateGeneral48Tetra(const MEDMEM::MESH& srcMesh);
    
    void calculateSubNodes(const MEDMEM::MESH& targetMesh, int targetCell, SplittingPolicy policy);
    
    inline const double* getCoordsOfSubNode(int node);
    
    template<int n>
    inline void calcBarycenter(double* barycenter, const int* const pts);

    vector<IntersectorTetra*> _tetra;
    
    vector<const double*> _nodes;
    
  };


  inline const double* IntersectorHexa::getCoordsOfSubNode(int node)
  {
    // replace at with [] for unsafe but faster access
    return _nodes.at(node - 1);
  }
  
  template<int n>
  inline void IntersectorHexa::calcBarycenter(double* barycenter, const int* const pts)
  {
    barycenter[0] = barycenter[1] = barycenter[2] = 0.0;
    for(int i = 0; i < n ; ++i)
      {
	const double* pt = getCoordsOfSubNode(pts[i]);
	barycenter[0] += pt[0];
	barycenter[1] += pt[1];
	barycenter[2] += pt[2];
      }
    
    barycenter[0] /= n;
    barycenter[1] /= n;
    barycenter[2] /= n;
  }

};


#endif
