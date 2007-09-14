#ifndef __REGION_NODE_HXX__
#define __REGION_NODE_HXX__

#include "MeshRegion.hxx"

namespace INTERP_UTILS
{

  /**
   * \brief Class containing a tuplet of a source region and a target region. 
   * This is used as the object to put on the stack in the depth-first search
   * in the bounding-box filtering process.
   */
  class RegionNode
  {
  public:
    
    RegionNode();
    
    ~RegionNode();

    inline MeshRegion& getSrcRegion();
    
    inline MeshRegion& getTargetRegion();

  private:
    
    /// source region
    MeshRegion _srcRegion;          
    
    /// target region
    MeshRegion _targetRegion;       

  };

  /**
   *  Accessor to source region
   *
   * @return   reference to source region
   */
  inline MeshRegion& RegionNode::getSrcRegion()
  {
    return _srcRegion;
  }
  /**
   *  Accessor to target region
   *
   * @return   reference to target region
   */
  inline MeshRegion& RegionNode::getTargetRegion()
  {
    return _targetRegion;
  }

};


#endif
