#include "BoundingBox.hxx"

#include <iostream>
#include <algorithm>

namespace INTERP_UTILS
{

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
   BoundingBox::BoundingBox(const double** pts, const int numPts)
   {
     using namespace std;
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
   * Constructor creating box from union of two boxes
   *
   */
  BoundingBox::BoundingBox(const BoundingBox& box1, const BoundingBox& box2) 
  {
    using namespace std;
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
  }

  /**
   * Determines if the intersection with a given box is empty
   * 
   * @param    box box with which intersection is tested
   * @returns  true if intersection between boxes is empty, false if not
   */
  bool BoundingBox::isDisjointWith(const BoundingBox& box) const
  {
    for(BoxCoord c = XMIN ; c <= ZMIN ; c = BoxCoord(c + 1))
      {
	
	const double otherMinCoord = box.getCoordinate(c);
	const double otherMaxCoord = box.getCoordinate(BoxCoord(c + 3));
	
	// boxes are disjoint if there exists a direction in which the 
	// minimum coordinate of one is greater than the maximum coordinate of the other

	//? stable version
	/*	const double tol = 1.0e-2;
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
   * @returns value of coordinate
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


  bool BoundingBox::isValid() const
  {
    bool valid = true;
    for(BoxCoord c = XMIN ; c < ZMIN ; c = BoxCoord(c + 1))
      {
	if(_coords[c] >= _coords[c + 3])
	  {
	    std::cout << "BoundingBox |: coordinate " << c << " is invalid : " << _coords[c] << " >= " << _coords[c+3] << std::endl;
	    valid = false;
	  }
      }
    return valid;
  }

};
