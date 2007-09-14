#ifndef __TARGET_INTERSECTOR__HXX__
#define __TARGET_INTERSECTOR__HXX__

namespace INTERP_UTILS
{
  /**
   * \brief Abstract base class of Intersector classes. 
   * These classes represent a target element and calculate its intersection
   * with the source elements.
   */
  class TargetIntersector
  {
  public:

    /// Virtual destructor
    virtual ~TargetIntersector() {}
    
    /**
     * Calculate the volume of the intersection between target cell 
     * and the given source cell.
     *
     * @param srcCell     global number of the source cell
     */
    virtual double intersectSourceCell(int srcCell) = 0;
  };
};

#endif
