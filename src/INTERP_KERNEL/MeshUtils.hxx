#ifndef __MESH_UTILS_HXX__
#define __MESH_UTILS_HXX__

#include "MEDMEM_define.hxx"
#include <cassert>

#include <set>

using namespace MEDMEM;
using namespace MED_EN;

namespace INTERP_UTILS
{

  /**
   * Returns the coordinates of a node of an element
   * (1 <= node <= #nodes)
   *
   * @param      node       the node for which the coordinates are sought
   * @param      element    an element of the mesh
   * @param      mesh       a mesh
   * @return    pointer to an array of 3 doubles containing the coordinates of the node
   */
  inline const double* getCoordsOfNode(int node, int element, const MESH& mesh)
  {
    const int connIdx = getGlobalNumberOfNode(node, element, mesh);
    return &(mesh.getCoordinates(MED_FULL_INTERLACE)[3*connIdx]);
  }

  /**
   * Returns the number of nodes of a given type of element
   *
   * @param   type   a type of element
   * @return  the number of nodes in elements of the type
   */
  inline int getNumberOfNodesForType(const medGeometryElement type)
  {
    assert(type > 300);
    assert(type < 400);
    
    // int(type) = yxx where y is dimension of entity (here 3)
    // and xx is the number of nodes of the element
    return static_cast<int>(type) - 300;
  }

  /**
   * Returns the global number of the node of an element.
   * (1 <= node <= #nodes of element)
   *
   * @param      node       the node for which the global number is sought
   * @param      element    an element of the mesh
   * @param      mesh       a mesh
   * @return    the node's global number (its coordinates in the coordinates array are at [3*globalNumber, 3*globalNumber + 2]
   */
  inline int getGlobalNumberOfNode(int node, int element, const MESH& mesh)
  {
    assert(node >= 1);
    assert(node <= mesh.getNumberOfNodes());
    const int nodeOffset = node - 1;
    const int elemIdx = mesh.getConnectivityIndex(MED_NODAL, MED_CELL)[element - 1] - 1;
    return mesh.getConnectivity(MED_FULL_INTERLACE, MED_NODAL, MED_CELL, MED_ALL_ELEMENTS)[elemIdx + nodeOffset] - 1;
  }
    
};  





#endif
