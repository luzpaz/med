#ifndef __BOUNDING_BOX_HXX__
#define __BOUNDING_BOX_HXX__

namespace INTERP_UTILS
{

  /**
   * Class representing the bounding box of a number of points.
   *
   */
  class BoundingBox 
  {
  public:
    enum BoxCoord { XMIN = 0, YMIN = 1, ZMIN = 2, XMAX = 3, YMAX = 4, ZMAX = 5 };
        
    BoundingBox(const double** pts, const int numPts);

    BoundingBox(const BoundingBox& box1, const BoundingBox& box2);

    ~BoundingBox();

    bool isDisjointWith(const BoundingBox& box) const;
    
    void setCoordinate(const BoxCoord coord, double value);

    double getCoordinate(const BoxCoord coord) const;

    void updateWithPoint(const double* pt);

    void dumpCoords() const;

  private:
    
    bool isValid() const;
    
    /// vector containing the coordinates of the box
    /// interlaced in the order XMIN, YMIN, ZMIN, XMAX, YMAX, ZMAX
    double* _coords;

  };

};

#endif
