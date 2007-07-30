#include "MeshElement.hxx"

namespace INTERP_UTILS
{

  /**
   * Constructor
   *
   * @param mesh    mesh that the element belongs to
   * @param type    geometric type of the element
   * @param index   global number of element in the mesh
   */
  MeshElement::MeshElement(const int index, const MEDEN::medGeometryElement type, const MEDMEM::Mesh& mesh)
    : _index(index - 1), _mesh(&mesh), _type(type), _box(0)
  {
    // get coordinates of vertices
    const int numNodes = getNumberNodes();
    const double* vertices[numNodes];

    for(int i = 0 ; i < numNodes ; ++i)
      {
	vertices[i] = getCoordsOfNode(i + 1);
      }

    // create bounding box
    _box = new BoundingBox(vertices, numNodes);
  
  }
    
  /**
   * Destructor
   *
   */
  MeshElement::~MeshElement()
  {
    if(_box)
      {
	delete _box;
      }
  }

  /**
   * Determines if this element is in the interior of another element 
   * by calculating the triple products for each point of this element with respect
   * to all the faces of the other object (faces must be triangulated ... ) If all triple
   * products have the same sign, then the element is in the interior of the other element
   *
   * @param otherElement the supposedly enclosing element
   * @returns true if this element is enclosed in the other element, false if not
   */
  bool MeshElement::isElementIncludedIn(const MeshElement& otherElement) const
  {
    // not implemented
    return false;
  }

  /**
   * Determines whether the intersection of this element is trivially empty. This is done by checking for each
   * face of one element if it is such that all the vertices of the other element is on the same side of this face.
   * If there is such a face, then the intersection is trivially empty. If there is no such face, we do not know if 
   * the intersection is empty.
   *
   * @pre The elements are convex. If this is not true, we return false.
   * @param otherElement the element believed to be disjoint with this one
   * @returns true if the two elements are convex and there exists a face of this element such as described 
   *          above, false otherwise
   */
  bool MeshElement::isElementTriviallyDisjointWith(const MeshElement& otherElement) const
  {
    // not implemented
    return false;
  }

  /**
   * Returns the number of nodes of this element
   *
   * @returns  the number of nodes of this element
   */
  int MeshElement::getNumberNodes() const
  {
    assert(_type > 300);
    assert(_type < 400);

    // int(type) = yxx where y is dimension of entity (here 3)
    // and xx is the number of nodes of the element
    return static_cast<int>(_type) - 300;
  }

  /**
   * Returns the coordinates of a node of this element
   * (1 <= node <= #nodes)
   *
   * @param      node  the node for which the coordinates are sought
   * @returns    pointer to an array of 3 doubles containing the coordinates
   */
  const double* MeshElement::getCoordsOfNode(int node) const
  {
    const int nodeOffset = node - 1;
    const int elemIdx = mesh->getConnectivityIndex(MED_NODAL, MED_CELL)[_index];
    return &(_mesh->getCoordinates(MED_FULL_INTERLACE)[elemIdx + nodeOffset]);
  }
  
  /**
   * Triangulate the faces of this element and apply an affine Transform to the triangles
   *
   * @param      triangles  vector in which triangles are stored
   * @param      T          affine transform that is applied to the nodes of the triangles
   */
  void MeshElement::triangulate(std::vector<TransformedTriangle>& triangles, const TetraAffineTransform& T) const
  {
    // not implemented
  }

};
