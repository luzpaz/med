#ifndef __TARGET_INTERSECTOR__HXX__
#define __TARGET_INTERSECTOR__HXX__

namespace INTERP_UTILS
{
  class TargetIntersector
  {
  public:
    virtual ~TargetIntersector() {}
    
    /**
     * Calculate the volume of the intersection of two cells
     *
     * @param srcCell     global number of the source cell
     */
    virtual double intersectSourceCell(int srcCell) = 0;
  };
};

#endif
