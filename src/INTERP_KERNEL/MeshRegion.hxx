#ifndef __MESH_REGION_HXX__
#define __MESH_REGION_HXX__

#include <vector>

#include "BoundingBox.hxx"

#include "MEDMEM_Mesh.hxx"

namespace INTERP_UTILS
{
  class MeshElement;

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
    MeshRegion();
    
    /**
     * Destructor
     *
     */
    ~MeshRegion();

    /**
     * Adds an element to the region, updating the bounding box.
     *
     * @param element pointer to element to add to region
     *
     */
    void addElement(MeshElement* const element, const MEDMEM::MESH& mesh);

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
    void split(MeshRegion& region1, MeshRegion& region2, BoundingBox::BoxCoord coord, const MEDMEM::MESH& mesh);

    bool isDisjointWithElementBoundingBox(const MeshElement& elem) const;

    std::vector<MeshElement*>::const_iterator getBeginElements() const;

    std::vector<MeshElement*>::const_iterator getEndElements() const;

    int getNumberOfElements() const;

  private:
    // Vector of pointers to elements. NB : these pointers are not owned by the region object, and are thus
    // neither allocated or liberated in this class. The elements must therefore be allocated and liberated outside this class
    std::vector<MeshElement*> _elements;
    BoundingBox* _box;
  
  };

};


#endif
