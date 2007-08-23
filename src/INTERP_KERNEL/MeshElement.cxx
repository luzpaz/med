#include "MeshElement.hxx"

#include "TetraAffineTransform.hxx"
#include "TransformedTriangle.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MeshUtils.hxx"
#include "BoundingBox.hxx"



namespace INTERP_UTILS
{

  /**
   * Constructor
   *
   * @param mesh    mesh that the element belongs to
   * @param type    geometric type of the element
   * @param index   global number of element in the mesh
   */
  MeshElement::MeshElement(const int index, const MED_EN::medGeometryElement type, const MEDMEM::MESH& mesh)
    : _index(index), _box(0), _type(type)
  {
    // get coordinates of vertices
    const int numNodes = getNumberOfNodesForType(type);
    
    const double* vertices[numNodes];

    for(int i = 0 ; i < numNodes ; ++i)
      {
	vertices[i] = getCoordsOfNode(i + 1, index, mesh);
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

  
  int MeshElement::getIndex() const
  {
    return _index;
  }
  
  void MeshElement::dumpCoords() const
  {
    std::cout << "Bounding box of element " << _index << " is " << std::endl;
    _box->dumpCoords();
  }
  

  /// ElementBBoxOrder
  bool ElementBBoxOrder::operator()( MeshElement* elem1, MeshElement* elem2)
  {
    assert(elem1 != 0);
    assert(elem2 != 0);
    assert(elem1->_box != 0);
    assert(elem2->_box != 0);
    
    const double coord1 = elem1->_box->getCoordinate(_coord);
    const double coord2 = elem2->_box->getCoordinate(_coord);
    
    return coord1 < coord2;
  }

};
