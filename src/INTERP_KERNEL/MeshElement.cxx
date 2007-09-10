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
   * @param index   global number of element in the mesh
   * @param mesh    mesh that the element belongs to
   * @param type    geometric type of the element
   */
  MeshElement::MeshElement(const int index, const MEDMEM::MESH& mesh)
    : _index(index), _box(0), _type(mesh.getElementType(MED_EN::MED_CELL, index))
  {
    // get coordinates of vertices
    const int numNodes = getNumberOfNodesForType(_type);

    assert(numNodes >= 3);

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

  

  /////////////////////////////////////////////////////////////////////
  /// ElementBBoxOrder                                    /////////////
  /////////////////////////////////////////////////////////////////////
  /**
   * Constructor
   *
   * @param  coord   BoundingBox coordinate (XMIN, XMAX, etc) on which to base the ordering
   */
  ElementBBoxOrder::ElementBBoxOrder(BoundingBox::BoxCoord coord)
    : _coord(coord)
  {
  }

  /**
   * Comparison operator based on the bounding boxes of the elements
   *
   * @return true if the coordinate _coord of the bounding box of elem1 is 
   *          strictly smaller than that of the bounding box of elem2
   */
  bool ElementBBoxOrder::operator()( MeshElement* elem1, MeshElement* elem2)
  {
    const BoundingBox* box1 = elem1->getBoundingBox();
    const BoundingBox* box2 = elem2->getBoundingBox();

    assert(elem1 != 0);
    assert(elem2 != 0);
    assert(box1 != 0);
    assert(box2 != 0);
    
    const double coord1 = box1->getCoordinate(_coord);
    const double coord2 = box2->getCoordinate(_coord);
    
    return coord1 < coord2;
  }

};
