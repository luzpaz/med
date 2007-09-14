#ifndef __MESH_REGION_HXX__
#define __MESH_REGION_HXX__

#include <vector>

#include "BoundingBox.hxx"
#include "MEDMEM_Mesh.hxx"

namespace INTERP_UTILS
{
  class MeshElement;

  /**
   * \brief Class representing a set of elements in a mesh together with their bounding box.
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

    inline std::vector<MeshElement*>::const_iterator getBeginElements() const;

    inline std::vector<MeshElement*>::const_iterator getEndElements() const;

    inline int getNumberOfElements() const;

  private:

    /// disallow copying
    MeshRegion(const MeshRegion& m);

    /// disallow assignment
    MeshRegion& operator=(const MeshRegion& m);

    /// Vector of pointers to contained MeshElements. 
    /// NB : these pointers are not owned by the region object, and are thus
    /// neither allocated or liberated in this class. The elements must therefore be allocated and liberated outside the class.
    std::vector<MeshElement*> _elements;

    /// BoundingBox containing all the nodes of all the elements in the region.
    BoundingBox* _box;
  
  };

  /**
   * Accessor to beginning of elements vector
   *
   * @return  constant iterator pointing at the beginning of the vector or elements
   */
  inline std::vector<MeshElement*>::const_iterator MeshRegion::getBeginElements() const
  {
    return _elements.begin();
  }

  /**
   * Accessor to end of elements vector
   *
   * @return  constant iterator pointing at the end of the vector or elements
   */
  inline std::vector<MeshElement*>::const_iterator MeshRegion::getEndElements() const
  {
    return _elements.end();
  }
  
  /**
   * Gives information on how many elements are contained in the region.
   *
   * @return  the number of elements contained in the region
   */
  inline int MeshRegion::getNumberOfElements() const
  {
    return _elements.size();
  }

};


#endif
