#ifndef __INTERSECTOR_TETRA_HXX__
#define __INTERSECTOR_TETRA_HXX__

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"

#include "Intersector.hxx"
#include <vector>
#include <ext/hash_map>
#include <functional>

#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"

using __gnu_cxx::hash_map;

namespace INTERP_UTILS
{
  class TriangleFaceKey
  {
  public:
    int _nodes[3];
    int _hashVal;
    
    TriangleFaceKey(int node1, int node2, int node3)
    {
      sort3Ints(_nodes, node1, node2, node3);
      //      assert(_nodes[0] < _nodes[1]);
      //assert(_nodes[0] < _nodes[2]);
      //assert(_nodes[1] < _nodes[2]);
      _hashVal = ( _nodes[0] + _nodes[1] + _nodes[2] ) % 29;
    }

    bool operator==(const TriangleFaceKey& rhs) const
    {
      return _nodes[0] == rhs._nodes[0] && _nodes[1] == rhs._nodes[1] && _nodes[2] == rhs._nodes[2];
    }

    int hashVal() const
    {
      return _hashVal;
    }
    
    inline void sort3Ints(int* sorted, int node1, int node2, int node3);
  };
}

namespace __gnu_cxx
{
  template<>
  class hash<INTERP_UTILS::TriangleFaceKey>
  {
  public:
    int operator()(const INTERP_UTILS::TriangleFaceKey& key) const
    {
      return key.hashVal();
    }
  };
};

namespace INTERP_UTILS
{

 
  /*
   * Class calculating the volume of intersection of individual 3D elements.
   *
   */
  class IntersectorTetra
  {

  public : 
    IntersectorTetra(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh,  int targetCell);

    ~IntersectorTetra();

    double intersectSourceCell(int srcCell);
    
    mutable int filtered;

  private:

    TetraAffineTransform* _t;
    
    hash_map< int, double*> _nodes;
    hash_map< TriangleFaceKey, double > _volumes;
    
    inline void checkIsOutside(const double* pt, bool* isOutside) const;
    inline void calculateNode(int globalNodeNum);
    inline void calculateVolume(TransformedTriangle& tri, const TriangleFaceKey& key);
    //    inline void sort3Ints(int* sorted, int node1, int node2, int node3);
    //inline std::string createFaceKey(int node1, int node2, int node3); 

    const MEDMEM::MESH& _srcMesh;
    
  };

  inline void IntersectorTetra::checkIsOutside(const double* pt, bool* isOutside) const
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
  
  inline void IntersectorTetra::calculateNode(int globalNodeNum)
  {
    assert(globalNodeNum >= 0);
    assert(globalNodeNum < _srcMesh.getNumberOfNodes());
    const double* node = &(_srcMesh.getCoordinates(MED_EN::MED_FULL_INTERLACE)[3*globalNodeNum]);
    double* transformedNode = new double[3];
    assert(transformedNode != 0);
    
    _t->apply(transformedNode, node);
    _nodes[globalNodeNum] = transformedNode;
  }

  inline void IntersectorTetra::calculateVolume(TransformedTriangle& tri, const TriangleFaceKey& key)
  {
    const double vol = tri.calculateIntersectionVolume();
    _volumes.insert(make_pair(key, vol));
  }

  inline void TriangleFaceKey::sort3Ints(int* sorted, int node1, int node2, int node3)
  {
    if(node1 < node2)
      {
	if(node1 < node3)
	  {
	    // node 1 is min
	    sorted[0] = node1;
	    sorted[1] = node2 < node3 ? node2 : node3;
	    sorted[2] = node2 < node3 ? node3 : node2;
	  }
	else
	  {
	    // node3 , node1, node2
	    sorted[0] = node3;
	    sorted[1] = node1;
	    sorted[2] = node2;
	  }
      }
    else // x2 < x1
      {
	if(node2 < node3)
	  {
	    // node 2 is min
	    sorted[0] = node2;
	    sorted[1] = node1 < node3 ? node1 : node3;
	    sorted[2] = node1 < node3 ? node3 : node1;
	  } 
	else
	  {
	    // node 3, node 2, node 1
	    sorted[0] = node3;
	    sorted[1] = node2;
	    sorted[2] = node1;
	  }
      }
  }
	    
#if 0
  inline std::string IntersectorTetra::createFaceKey(int node1, int node2, int node3)
  {
    int sorted[3];
    sort3Ints(sorted, node1, node2, node3);

    std::stringstream sstr;
    sstr << node1 << "-" << node2 << "-" << node3;
    return sstr.str();
  }
#endif

};


#endif
