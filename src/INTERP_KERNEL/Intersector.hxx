#ifndef __INTERSECTOR__HXX__
#define __INTERSECTOR__HXX__

namespace MEDMEM
{
  class Intersector
  {
  public:
    virtual ~Intersector() {}
    
    /**
     * Calculate the volume of the intersection of two cells
     *
     * @param srcCell     global number of the source cell
     * @param targetCell  global number of the target cell
     */
    virtual double intersectCells(int srcCell, int targetCell) = 0;
  };
};

#endif
