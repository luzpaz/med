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
   * It permits access to the element's global number, type and bounding box and allows
   * easy bounding box intersection tests between MeshElements and collections of MeshElement (MeshRegions)
   */
  class MeshElement
  {

  public:
    
    MeshElement(const int index, const MED_EN::medGeometryElement type, const MEDMEM::MESH& mesh);
    
    ~MeshElement();
    
    int getIndex() const;
    
    const BoundingBox* getBoundingBox() const;

    MED_EN::medGeometryElement getType() const;

  private:
    const int _index;                /// global number of the element
    BoundingBox* _box;               /// bounding box of the element - does not change after having been initialised
    MED_EN::medGeometryElement _type;/// type of the element
  };


  /*
   * Class defining an order for MeshElements based on their bounding boxes.
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
    BoundingBox::BoxCoord _coord;  /// BoundingBox coordinate (XMIN, XMAX, etc) on which to base the ordering
  };

};



#endif
