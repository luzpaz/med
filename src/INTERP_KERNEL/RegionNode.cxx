#include "RegionNode.hxx"

namespace INTERP_UTILS
{
  /**
   * Default constructor
   * 
   */
  RegionNode::RegionNode()
  {
  }
    
  /**
   * Destructor
   *
   */
  RegionNode::~RegionNode()
  {
  }
    
  /**
   *  Accessor to source region
   *
   * @return   reference to source region
   */
  MeshRegion& RegionNode::getSrcRegion()
  {
    return _srcRegion;
  }
  /**
   *  Accessor to target region
   *
   * @return   reference to target region
   */
  MeshRegion& RegionNode::getTargetRegion()
  {
    return _targetRegion;
  }

};
