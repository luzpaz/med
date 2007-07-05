#ifndef __REGION_NODE_HXX__
#define __REGION_NODE_HXX__

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
     *
     */
    MeshRegion& getSrcRegion() const;
    
    /**
     *
     */
    MeshRegion& getTargetRegion() const;


  private:

    MeshRegion _srcRegion;
    MeshRegion _targetRegion;

  };

};


#endif
