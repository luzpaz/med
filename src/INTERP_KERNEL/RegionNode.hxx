#ifndef __REGION_NODE_HXX__
#define __REGION_NODE_HXX__

#include "MeshRegion.hxx"

namespace INTERP_UTILS
{

  /**
   * Class containing a tuplet of a source region and a target region. 
   * This is used as the object to put on the stack in the depth-first search
   * in the bounding-box filtering process.
   *
   */
  class RegionNode
  {
  public:
    
    /**
     * Default constructor
     * 
     */
    RegionNode();
    
    /**
     * Destructor
     *
     */
    ~RegionNode();

    /**
     *  Accessor to source region
     *
     * @return   reference to source region
     */
    MeshRegion& getSrcRegion();
    
    /**
     *  Accessor to target region
     *
     * @return   reference to target region
     */
    MeshRegion& getTargetRegion();


  private:

    MeshRegion _srcRegion;
    MeshRegion _targetRegion;

  };

};


#endif
