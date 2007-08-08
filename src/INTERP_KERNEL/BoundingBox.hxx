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
    
    
    /**
     * Default constructor
     * 
     */
    //BoundingBox() : coords({0.0, 0.0, 0.0, 0.0, 0.0, 0.0}) {}
    
    /**
     * Constructor creating box from an array of the points corresponding
     * to the vertices of the element.
     * Each point is represented by an array of three doubles.
     *
     * @param pts     array of points 
     * @param numPts  number of vertices
     *
     */
    BoundingBox(const double** pts, const int numPts);

    /**
     * Constructor creating box from union of two boxes
     *
     */
    BoundingBox(const BoundingBox& box1, const BoundingBox& box2);

    /**
     * Destructor
     *
     */
    ~BoundingBox();

    /**
     * Determines if the intersection with a given box is empty
     * 
     * @param    box box with which intersection is tested
     * @returns  true if intersection between boxes is empty, false if not
     */
    bool isDisjointWith(const BoundingBox& box) const;
    
    /**
     * Sets a coordinate of the box to a given value.
     * 
     * @param coord coordinate to set
     * @param value new value for coordinate
     *
     */
    void setCoordinate(const BoxCoord coord, double value);

    /**
     * Gets a coordinate of the box
     * 
     * @param coord coordinate to set
     * @returns value of coordinate
     *
     */
    double getCoordinate(const BoxCoord coord) const;

    void updateWithPoint(const double* pt);

  private:
    
    bool isValid() const;
    
    double _coords[6];

    

  };

  //  typedef BoundingBox::BoxCoord BoxCoord;

};

#endif
