#ifndef __MESH_ELEMENT_HXX__
#define __MESH_ELEMENT_HXX__

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"

#include "BoundingBox.hxx"



using namespace MEDMEM;
using namespace MED_EN;


namespace INTERP_UTILS
{

  /**
   * Class representing a single element of a mesh together with its bounding box.
   * It gives access to the element's global number, type and bounding box and allows
   * easy bounding box intersection tests between MeshElements and collections of MeshElement (MeshRegions)
   */
  class MeshElement
  {

  public:
    
    MeshElement(const int index, const MEDMEM::MESH& mesh);
    
    ~MeshElement();
    
    inline int getIndex() const;
    
    inline const BoundingBox* getBoundingBox() const;

    inline MED_EN::medGeometryElement getType() const;

  private:
    /// disallow copying
    MeshElement(const MeshElement& elem);

    /// disallow assignment
    MeshElement& operator=(const MeshElement& elem);

    /// global number of the element
    const int _index;               
    
    /// bounding box of the element - does not change after having been initialised
    BoundingBox* _box;              
    
    /// type of the element
    MED_EN::medGeometryElement _type;
  };

  /**
   * Accessor to global number
   *
   * @return  global number of the element
   */
  inline int MeshElement::getIndex() const
  {
    return _index;
  }  
  
  /**
   * Accessor to bounding box
   *
   * @return pointer to bounding box of the element
   */
  inline const BoundingBox* MeshElement::getBoundingBox() const
  {
    return _box;
  }

  /**
   * Accessor to the type of the element
   *
   * @return  type of the element
   */
  inline MED_EN::medGeometryElement MeshElement::getType() const
  {
    return _type;
  }


  /**
   * \brief Class defining an order for MeshElements based on their bounding boxes.
   * The order defined between two elements is that between a given coordinate of 
   * their bounding boxes. For instance, if the order is based on YMIN, an element whose boxes
   * has a smaller YMIN is sorted before one with a larger YMIN.
   *
   */
  class ElementBBoxOrder
  {
  public : 
    
    ElementBBoxOrder(BoundingBox::BoxCoord coord);
    
    bool operator()(MeshElement* elem1, MeshElement* elem2);
    
  private :
    /// BoundingBox coordinate (XMIN, XMAX, etc) on which to base the ordering
    BoundingBox::BoxCoord _coord;  
  };

};



#endif
