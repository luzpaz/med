#include "BoundingBox.hxx"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace INTERP_UTILS
{
  
  /**
   * Constructor creating box from an array of the points corresponding
   * to the vertices of the element.
   * Each point is represented by an array of three doubles.
   *
   * @param pts     array of points 
   * @param numPts  number of vertices
   *
   */
   BoundingBox::BoundingBox(const double** pts, const int numPts)
     :_coords(new double[6])
   {
     using namespace std;
     assert(_coords != 0);
     assert(numPts > 1);
     

     // initialize with first two points
     const double* pt1 = pts[0];
     const double* pt2 = pts[1];

     for(BoxCoord c = XMIN ; c <= ZMIN ; c = BoxCoord(c + 1))
       {
	 _coords[c] = min(pt1[c], pt2[c]);
	 _coords[c + 3] = max(pt1[c], pt2[c]);
       }

     for(int i = 2 ; i < numPts ; ++i)
       {
	 updateWithPoint(pts[i]);
       }
     assert(isValid());
   }

  /**
   * Constructor creating box from union of two boxes, resulting in a box that enclose both of them
   *
   * @param  box1  the first box
   * @param  box2  the second box
   */
  BoundingBox::BoundingBox(const BoundingBox& box1, const BoundingBox& box2) 
    : _coords(new double[6])
  {
    using namespace std;
    assert(_coords != 0);

    for(BoxCoord c = XMIN ; c <= ZMIN ; c = BoxCoord(c + 1))
       {
	 _coords[c] = min(box1._coords[c], box2._coords[c]);
	 _coords[c + 3] = max(box1._coords[c + 3], box2._coords[c + 3]);
       }
    assert(isValid());
  }

  /**
   * Destructor
   *
   */
  BoundingBox::~BoundingBox()
  {
    delete[] _coords;
  }

  /**
   * Determines if the intersection with a given box is empty
   * 
   * @param    box   BoundingBox with which intersection is tested
   * @return  true if intersection between boxes is empty, false if not
   */
  bool BoundingBox::isDisjointWith(const BoundingBox& box) const
  {
    for(BoxCoord c = XMIN ; c <= ZMIN ; c = BoxCoord(c + 1))
      {
	const double otherMinCoord = box.getCoordinate(c);
	const double otherMaxCoord = box.getCoordinate(BoxCoord(c + 3));
	
	// boxes are disjoint if there exists a direction in which the 
	// minimum coordinate of one is greater than the maximum coordinate of the other

	//? more stable version
	/*const double tol = 1.0e-2*_coords[c];
	if(_coords[c] > otherMaxCoord + tol 
	   || _coords[c + 3] < otherMinCoord - tol)
	*/
	
	if(_coords[c] > otherMaxCoord 
	   || _coords[c + 3] < otherMinCoord)
	
	   {
	    return true;
	  }
	
      }
    return false;
  }
    
  /**
   * Sets a coordinate of the box to a given value.
   * 
   * @param coord coordinate to set
   * @param value new value for coordinate
   *
   */
  void BoundingBox::setCoordinate(const BoxCoord coord, double value)
  {
    _coords[coord] = value;
  }

  /**
   * Gets a coordinate of the box
   * 
   * @param coord coordinate to set
   * @return value of coordinate
   *
   */
  double BoundingBox::getCoordinate(const BoxCoord coord) const
  {
    return _coords[coord];
  }

  /**
   * Updates the bounding box to include a given point
   * 
   * @param pt    point to be included
   *
   */
  void BoundingBox::updateWithPoint(const double* pt)
  {
    using namespace std;

    for(BoxCoord c = XMIN ; c <= ZMIN ; c = BoxCoord(c + 1))
      {
	const double ptVal = pt[c];

	// update min and max coordinates
	_coords[c] = min(_coords[c], ptVal);
	_coords[c + 3] = max(_coords[c + 3], ptVal);

      }
  }

  /*
   * Prints the coordinates of the box to std::cout
   *
   */
  void BoundingBox::dumpCoords() const
  {
    std::cout << "[xmin, xmax] = [" << _coords[XMIN] << ", " << _coords[XMAX] << "]" << " | ";
    std::cout << "[ymin, ymax] = [" << _coords[YMIN] << ", " << _coords[YMAX] << "]" << " | ";
    std::cout << "[zmin, zmax] = [" << _coords[ZMIN] << ", " << _coords[ZMAX] << "]";
    std::cout << std::endl;
  }

  /*
   * Checks if the box is valid, which it is if its minimum coordinates are
   * smaller than its maximum coordinates in all directions.
   *
   * @return  true if the box is valid, false if not
   */
  bool BoundingBox::isValid() const
  {
    bool valid = true;
    for(BoxCoord c = XMIN ; c < ZMIN ; c = BoxCoord(c + 1))
      {
	if(_coords[c] > _coords[c + 3])
	  {
	    std::cout << "+++ Error in  BoundingBox |: coordinate " << c << " is invalid : "
		      <<_coords[c] << " > " << _coords[c+3] << std::endl;
	    valid = false;
	  }
      }
    return valid;
  }

};
