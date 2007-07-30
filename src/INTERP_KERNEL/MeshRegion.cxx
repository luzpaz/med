#include "MeshRegion.hxx"

namespace INTERP_UTILS
{
  /**
   * Class representing a set of elements in a mesh together with their bounding box.
   * It permits to split itself in two, which is used in the depth-first search filtering process.
   *
   */
  class MeshRegion
  {
  public:
    
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

    /**
     * Adds an element to the region, updating the bounding box.
     *
     * @param element pointer to element to add to region
     *
     */
    void MeshRegion::addElement(MeshElement* const element)
      {
	_elements.push_back(element);
	
	if(_box == 0)
	  {
	    // get coordinates of elements
	    _box = new BoundingBox();
	  }
      }

    /**
     * Splits the region in two along the given axis, copying the elements with bounding boxes whose maximum
     * coordinate along the axis are smaller than the middle of the bounding box of this region in region1. The
     * rest of the elements are copied to region2.
     *
     * @param region1 region in which to store one half of this region
     * @param region2 region in which to store the other of this region
     * @param axis    axis along which to split the region
     *
     */
    void MeshRegion::split(Region& region1, Region& region2, int axis) const;

  private:
    // Vector of pointers to elements. NB : these pointers are not owned by the region object, and are thus
    // neither allocated or liberated in this class. The elements must therefore be allocated and liberated outside this class
    std::vector<MeshElement*> _elements;
    BoundingBox* _box;

  };

};


#endif
