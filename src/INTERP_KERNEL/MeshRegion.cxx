#include "MeshRegion.hxx"

#include "MeshElement.hxx"
#include "MeshUtils.hxx"

namespace INTERP_UTILS
{
    
  /**
   * Default constructor
   * 
   */
  MeshRegion::MeshRegion()
    : _box(0)
  {
  }
    
  /**
   * Destructor
   *
   */
  MeshRegion::~MeshRegion()
  {
    if(_box != 0)
      {
	delete _box;
      }
  }

  /**
   * Adds an element to the region, updating the bounding box. If the bounding box does not yet
   * exist, it is created here. This creation is delayed to make it possible to have empty MeshRegions
   *
   * @param element pointer to element to add to region
   * @param mesh    mesh to which element belongs
   *
   */
  void MeshRegion::addElement(MeshElement* const element, const MEDMEM::MESH& mesh)
  {
    _elements.push_back(element);

    const int numNodes = getNumberOfNodesForType(element->getType());
    const int elemIdx = element->getIndex();
	
    if(_box == 0)
      {
	const double* pts[numNodes];

	// get coordinates of the nodes of the element
	for(int i = 0 ; i < numNodes ; ++i)
	  {
	    pts[i] = getCoordsOfNode(i + 1, elemIdx, mesh);
	  }
	    
	_box = new BoundingBox(pts, numNodes);
	    
      } else {

	for(int i = 0 ; i < numNodes ; ++i)
	  {
	    const double* pt = getCoordsOfNode(i + 1, elemIdx, mesh);
	    _box->updateWithPoint(pt);
	  }
      }
  }

  /**
   * Splits the region in two along the given axis, copying the elements with bounding boxes whose maximum
   * coordinate along the axis are smaller than the middle of the bounding box of this region in region1. The
   * rest of the elements are copied to region2.
   *
   * @param region1 region in which to store one half of this region
   * @param region2 region in which to store the other of this region
   * @param coord   coordinate of BoundingBox to use when splitting the region
   * @param mesh    mesh to which region belongs
   *
   */
  void MeshRegion::split(MeshRegion& region1, MeshRegion& region2, BoundingBox::BoxCoord coord, const MEDMEM::MESH& mesh)
  {
    // create ordering
    ElementBBoxOrder cmp(coord);

    // sort elements by their bounding boxes
    std::sort(_elements.begin(), _elements.end(), cmp);

    // put the first half of the elements in region1 and the 
    // rest in region2
    std::vector<MeshElement*>::const_iterator iter = _elements.begin();
    int elemCount = 0;

    while(elemCount < static_cast<int>(_elements.size() / 2))
      {
	region1.addElement(*iter, mesh);
	++iter;
	++elemCount;
      }

    while(iter != _elements.end())
      {
	region2.addElement(*iter, mesh);
	++iter;
      }
  }

  /**
   * Determines if a given element can intersect the elements of this region by 
   * testing whether the bounding box of the region intersects the bounding box of the element.
   * Note that the test is only true in one direction : if the bounding boxes are disjoint, the
   * element cannot intersect any of the elements in the region, but if they are not disjoint, the 
   * element may or may not do so.
   *
   * @param   elem  Element with which to test for disjoint-ness
   * @return  true if the bounding box of the element is disjoint with the bounding box of the region, false otherwise
   */
  bool MeshRegion::isDisjointWithElementBoundingBox(const MeshElement& elem) const
  {
    const BoundingBox* elemBox = elem.getBoundingBox();

    assert(_box != 0);
    assert(elemBox != 0);

    return _box->isDisjointWith(*elemBox);
  }  

  
};
