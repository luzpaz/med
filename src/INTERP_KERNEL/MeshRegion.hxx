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
    
    MeshRegion();

    ~MeshRegion();

    void addElement(MeshElement* const element, const MEDMEM::MESH& mesh);

    void split(MeshRegion& region1, MeshRegion& region2, BoundingBox::BoxCoord coord, const MEDMEM::MESH& mesh);

    bool isDisjointWithElementBoundingBox(const MeshElement& elem) const;

    std::vector<MeshElement*>::const_iterator getBeginElements() const;

    std::vector<MeshElement*>::const_iterator getEndElements() const;

    int getNumberOfElements() const;

  private:
    /// Vector of pointers to elements. NB : these pointers are not owned by the region object, and are thus
    /// neither allocated or liberated in this class. The elements must therefore be allocated and liberated outside this class
    std::vector<MeshElement*> _elements;

    /// BoundingBox containing all the nodes of all the elements in the region.
    BoundingBox* _box;
  
  };

};


#endif
