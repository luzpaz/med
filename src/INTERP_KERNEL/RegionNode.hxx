#ifndef __REGION_NODE_HXX__
#define __REGION_NODE_HXX__

#include "MeshRegion.hxx"

namespace INTERP_UTILS
{

  /**
   * Class containing a tuplet of a source region and a target region. 
   * This is used as the object to put on the stack in the depth-first search
   * in the bounding-box filtering process.
   */
  class RegionNode
  {
  public:
    
    RegionNode();
    
    ~RegionNode();

    MeshRegion& getSrcRegion();
    
    MeshRegion& getTargetRegion();

  private:

    MeshRegion _srcRegion;          /// source region
    MeshRegion _targetRegion;       /// target region

  };

};


#endif
