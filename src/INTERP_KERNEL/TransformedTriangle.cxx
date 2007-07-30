#include "TransformedTriangle.hxx"
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <functional>
#include "VectorUtils.hxx"
#include <math.h>

class CircularSortOrder
{
public:

  enum CoordType { XY, XZ, YZ };

  CircularSortOrder(const double* barycenter, const double* pt0, const CoordType type)
    : _pt0( pt0 )
  {
    // get the indices to use in determinant
    _aIdx = (type == YZ) ? 2 : 0;
    _bIdx = (type == XY) ? 1 : 2;
    
    _a = barycenter[_aIdx] - pt0[_aIdx];
    _b = barycenter[_bIdx] - pt0[_bIdx];
    //    std::cout << "Creating order of type " << type << " with pt0= " << vToStr(pt0) << std::endl;  
    //    std::cout << "a = " << _a << ", b = " << _b << std::endl;
  }

  bool operator()(const double* pt1, const double* pt2)
  {
    //{ calculations could be optimised here, avoiding intermediary affectations
    const double diff[4] = 
      {
	pt1[_aIdx] - _pt0[_aIdx], pt1[_bIdx] - _pt0[_bIdx], // pt1 - pt0
	pt2[_aIdx] - _pt0[_aIdx], pt2[_bIdx] - _pt0[_bIdx], // pt2 - pt0
      };

    // We need to check if one of the points is equal to pt0,
    // since pt0 should always end up at the beginning
    // is pt1 == pt0 ? -> if yes, pt1 < pt2
    if(diff[0] == 0.0 && diff[1] == 0.0)
      return true; 
    // is pt2 == pt0 ? -> if yes pt2  < pt1
    if(diff[2] == 0.0 && diff[3] == 0.0)
      return false; 

    // normal case
    const double det1 = _a*diff[1] - _b*diff[0];
    const double det2 = _a*diff[3] - _b*diff[2];

    return det1 < det2;
  }

private:
  int _aIdx, _bIdx;
  double _a, _b;
  const double* _pt0;
};

class ProjectedCentralCircularSortOrder
{
public:  
  enum CoordType { XY, XZ, YZ };
  
  ProjectedCentralCircularSortOrder(const double* barycenter, const CoordType type)
    : _aIdx((type == YZ) ? 2 : 0), 
      _bIdx((type == XY) ? 1 : 2),
      _a(barycenter[_aIdx]), 
      _b(barycenter[_bIdx])
  {
  }

  bool operator()(const double* pt1, const double* pt2)
  {
    // calculate angles with the axis
    const double ang1 = atan2(pt1[_aIdx] - _a, pt1[_bIdx] - _b);
    const double ang2 = atan2(pt2[_aIdx] - _a, pt2[_bIdx] - _b);

    return ang1 > ang2;
  }

private:
  const int _aIdx, _bIdx;
  const double _a, _b;
};

namespace INTERP_UTILS
{
  ////////////////////////////////////////////////////////////////////////////
  /// PUBLIC  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  /**
   * Constructor
   *
   * The coordinates are copied to the internal member variables
   * (slower but safer - investigate if this is necessary)
   *
   * @param p   array of three doubles containing coordinates of P
   * @param q   array of three doubles containing coordinates of Q
   * @param r   array of three doubles containing coordinates of R
   */
  TransformedTriangle::TransformedTriangle(double* p, double* q, double* r)
    : _isDoubleProductsCalculated(false),  _isTripleProductsCalculated(false)
  {
  
    for(int i = 0 ; i < 3 ; ++i) 
      {
	// xyz coordinates
	_coords[5*P + i] = p[i];
	_coords[5*Q + i] = q[i];
	_coords[5*R + i] = r[i];
      }

    // h coordinate
    _coords[5*P + 3] = 1 - p[0] - p[1] - p[2];
    _coords[5*Q + 3] = 1 - q[0] - q[1] - q[2];
    _coords[5*R + 3] = 1 - r[0] - r[1] - r[2];
  
    // H coordinate
    _coords[5*P + 4] = 1 - p[0] - p[1];
    _coords[5*Q + 4] = 1 - q[0] - q[1];
    _coords[5*R + 4] = 1 - r[0] - r[1];

    // initialise rest of data
    preCalculateDoubleProducts();
    preCalculateTripleProducts();
 
  }

  TransformedTriangle::~TransformedTriangle()
  {
    // delete elements of polygons
    for(std::vector<double*>::iterator it = _polygonA.begin() ; it != _polygonA.end() ; ++it)
      {
	delete[] *it;
      }
    for(std::vector<double*>::iterator it = _polygonB.begin() ; it != _polygonB.end() ; ++it)
      {
	delete[] *it;
      }    
  }

  /**
   * Calculates the volume of intersection between the triangle and the 
   * unit tetrahedron.
   *
   * @returns   volume of intersection of this triangle with unit tetrahedron, 
   *            as described in Grandy
   *
   */
  double TransformedTriangle::calculateIntersectionVolume()
  {
    // check first that we are not below z - plane

    if(isTriangleBelowTetraeder())
      {
	return 0.0;
      }

    // get the sign of the volume -  equal to the sign of the z-component of the normal
    // of the triangle, u_x * v_y - u_y * v_x, where u = q - p and v = r - p
    // if it is zero, the triangle is perpendicular to the z - plane and so the volume is zero
    const double uv_xy[4] = 
      {
	_coords[5*Q] - _coords[5*P], _coords[5*Q + 1] - _coords[5*P + 1], // u_x, u_y
	_coords[5*R] - _coords[5*P], _coords[5*R + 1] - _coords[5*P + 1]  // v_x, v_y
      };

    double sign = uv_xy[0] * uv_xy[3] - uv_xy[1] * uv_xy[2];
    
    if(sign == 0.0)
      {
	return 0.0;
      }

    // normalize
    sign = sign > 0.0 ? 1.0 : -1.0;


    std::cout << std::endl << "-- Calculating intersection polygons ... " << std::endl; 
    calculateIntersectionPolygons();
    
    // calculate volume under A
    std::cout << std::endl << "-- Treating polygon A ... " << std::endl; 
    double barycenter[3];
    
    double volA = 0.0;

    if(_polygonA.size() > 2)
      {
	calculatePolygonBarycenter(A, barycenter);
	sortIntersectionPolygon(A, barycenter);
	volA = calculateVolumeUnderPolygon(A, barycenter);
	std::cout << "Volume is " << sign * volA << std::endl;
      }

    double volB = 0.0;

    // if triangle is not in h = 0 plane, calculate volume under B
    if(!isTriangleInPlaneOfFacet(XYZ) && _polygonB.size() > 2)
      {
	std::cout << std::endl << "-- Treating polygon B ... " << std::endl; 
	calculatePolygonBarycenter(B, barycenter);
	sortIntersectionPolygon(B, barycenter);
	volB = calculateVolumeUnderPolygon(B, barycenter);
	std::cout << "Volume is " << sign * volB << std::endl;
      }

    std::cout << std::endl << "volA + volB = " << sign * (volA + volB) << std::endl << std::endl;

    return sign * (volA + volB);

  } 
    
  ////////////////////////////////////////////////////////////////////////////
  /// PRIVATE ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
    
  ////////////////////////////////////////////////////////////////////////////////////
  /// High-level methods called directly by calculateIntersectionVolume()     ////////
  ////////////////////////////////////////////////////////////////////////////////////
  /**
   * Calculates the intersection polygons A and B, performing the intersection tests
   * and storing the corresponding points in the vectors _polygonA and _polygonB
   *
   * @post _polygonA contains the intersection polygon A and _polygonB contains the
   *       intersection polygon B.
   *
   */
  void TransformedTriangle::calculateIntersectionPolygons()
  {
    assert(_polygonA.size() == 0);
    assert(_polygonB.size() == 0);

    // -- surface intersections
    // surface - edge
    for(TetraEdge edge = OX ; edge <= ZX ; edge = TetraEdge(edge + 1))
      {
	if(testSurfaceEdgeIntersection(edge))
	  {
	    //{ we only really need to calculate the point once
	    double* ptA = new double[3];
	    calcIntersectionPtSurfaceEdge(edge, ptA);
	    _polygonA.push_back(ptA);
	    std::cout << "Surface-edge : " << vToStr(ptA) << " added to A " << std::endl;
	    if(edge >= XY)
	      {
		double* ptB = new double[3];
		copyVector3(ptA, ptB);
		_polygonB.push_back(ptB);
		std::cout << "Surface-edge : " << vToStr(ptB) << " added to B " << std::endl;
	      }
	    
	  }
      }

    // surface - ray
    for(TetraCorner corner = X ; corner < NO_TET_CORNER ; corner = TetraCorner(corner + 1))
      {
	if(testSurfaceRayIntersection(corner))
	  {
	    double* ptB = new double[3];
	    copyVector3(&COORDS_TET_CORNER[3 * corner], ptB);
	    _polygonB.push_back(ptB);
	    std::cout << "Surface-ray : " << vToStr(ptB) << " added to B" << std::endl;
	  }
      }
    
    // -- segment intersections
    for(TriSegment seg = PQ ; seg < NO_TRI_SEGMENT ; seg = TriSegment(seg + 1))
      {
	// segment - facet
	for(TetraFacet facet = OYZ ; facet < NO_TET_FACET ; facet = TetraFacet(facet + 1))
	  {
	    if(testSegmentFacetIntersection(seg, facet))
	      {
		double* ptA = new double[3];
		calcIntersectionPtSegmentFacet(seg, facet, ptA);
		_polygonA.push_back(ptA);
		std::cout << "Segment-facet : " << vToStr(ptA) << " added to A" << std::endl;
		if(facet == XYZ)
		  {
		    double* ptB = new double[3];
		    copyVector3(ptA, ptB);
		    _polygonB.push_back(ptB);
		    std::cout << "Segment-facet : " << vToStr(ptB) << " added to B" << std::endl;
		  }
	      }
	  }

	// segment - edge
	for(TetraEdge edge = OX ; edge <= ZX ; edge = TetraEdge(edge + 1))
	  {
	    if(testSegmentEdgeIntersection(seg, edge))
	      {
		double* ptA = new double[3];
		calcIntersectionPtSegmentEdge(seg, edge, ptA);
		_polygonA.push_back(ptA);
		std::cout << "Segment-edge : " << vToStr(ptA) << " added to A" << std::endl;
		if(edge >= XY)
		  {
		    double* ptB = new double[3];
		    copyVector3(ptA, ptB);
		    _polygonB.push_back(ptB);
		  }
	      }
	  }
	
	// segment - corner
	for(TetraCorner corner = O ; corner < NO_TET_CORNER ; corner = TetraCorner(corner + 1))
	  {
	    if(testSegmentCornerIntersection(seg, corner))
	      {
		double* ptA = new double[3];
		copyVector3(&COORDS_TET_CORNER[3 * corner], ptA);
		_polygonA.push_back(ptA);
		std::cout << "Segment-corner : " << vToStr(ptA) << " added to A" << std::endl;
		if(corner != O)
		  {
		    double* ptB = new double[3];
		    _polygonB.push_back(ptB);
		    copyVector3(&COORDS_TET_CORNER[3 * corner], ptB);
		    std::cout << "Segment-corner : " << vToStr(ptB) << " added to B" << std::endl;
		  }
	      }
	  }

	// segment - ray 
	for(TetraCorner corner = X ; corner < NO_TET_CORNER ; corner = TetraCorner(corner + 1))
	  {
	    if(testSegmentRayIntersection(seg, corner))
	      {
		double* ptB = new double[3];
		copyVector3(&COORDS_TET_CORNER[3 * corner], ptB);
		_polygonB.push_back(ptB);
		std::cout << "Segment-ray : " << vToStr(ptB) << " added to B" << std::endl;
	      }
	  }
	
       	// segment - halfstrip
	for(TetraEdge edge = XY ; edge <= ZX ; edge = TetraEdge(edge + 1))
	  {
	    if(testSegmentHalfstripIntersection(seg, edge))
	      {
		double* ptB = new double[3];
		calcIntersectionPtSegmentHalfstrip(seg, edge, ptB);
		_polygonB.push_back(ptB);
		std::cout << "Segment-halfstrip : " << vToStr(ptB) << " added to B" << std::endl;
	      }
	  }
      }      
    
    // inclusion tests
    for(TriCorner corner = P ; corner < NO_TRI_CORNER ; corner = TriCorner(corner + 1))
      {
	// { XYZ - inclusion only possible if in Tetrahedron?
	// tetrahedron
	if(testCornerInTetrahedron(corner))
	  {
	    double* ptA = new double[3];
	    copyVector3(&_coords[5*corner], ptA);
	    _polygonA.push_back(ptA);
	    std::cout << "Inclusion tetrahedron : " << vToStr(ptA) << " added to A" << std::endl;
	  }

	// XYZ - plane
	if(testCornerOnXYZFacet(corner))
	  {
	    double* ptB = new double[3];
	    copyVector3(&_coords[5*corner], ptB);
	    _polygonA.push_back(ptB);
	    std::cout << "Inclusion XYZ-plane : " << vToStr(ptB) << " added to B" << std::endl;
	  }
      }
  }

  /**
   * Calculates the barycenters of the given intersection polygon.
   *
   * @pre  the intersection polygons have been calculated with calculateIntersectionPolygons()
   * 
   * @param poly        one of the two intersection polygons
   * @param barycenter  array of three doubles where barycenter is stored
   *
   */
  void TransformedTriangle::calculatePolygonBarycenter(const IntersectionPolygon poly, double* barycenter)
  {
    std::cout << "--- Calculating polygon barycenter" << std::endl;

    // get the polygon points
    std::vector<double*>& polygon = (poly == A) ? _polygonA : _polygonB;

    // calculate barycenter
    const int m = polygon.size();

    for(int j = 0 ; j < 3 ; ++j)
      {
	barycenter[j] = 0.0;
      }

    if(m != 0)
      {
	for(int i = 0 ; i < m ; ++i)
	  {
	    const double* pt = polygon[i];
	    for(int j = 0 ; j < 3 ; ++j)
	      {
		barycenter[j] += pt[j] / double(m);
	      }
	  }
      }
    std::cout << "Barycenter is " << vToStr(barycenter) << std::endl;
  }

  /**
   * Sorts the given intersection polygon in circular order around its barycenter.
 
   * @pre  the intersection polygons have been calculated with calculateIntersectionPolygons()
   * @post the vertices in _polygonA and _polygonB are sorted in circular order around their
   *       respective barycenters
   *
   * @param poly        one of the two intersection polygons
   * @param barycenter  array of three doubles with the coordinates of the barycenter
   * 
   */
  void TransformedTriangle::sortIntersectionPolygon(const IntersectionPolygon poly, const double* barycenter)
  {
    std::cout << "--- Sorting polygon ..."<< std::endl;

    using ::ProjectedCentralCircularSortOrder;
    typedef ProjectedCentralCircularSortOrder SortOrder; // change is only necessary here and in constructor
    typedef SortOrder::CoordType CoordType;

    // get the polygon points
    std::vector<double*>& polygon = (poly == A) ? _polygonA : _polygonB;

    if(polygon.size() == 0)
      return;

    // determine type of sorting
    CoordType type = SortOrder::XY;
    if(poly == A) // B is on h = 0 plane -> ok
      {
	// is triangle parallel to x == 0 ?
	if(isTriangleInPlaneOfFacet(OZX)) 
	  {
	    type = SortOrder::YZ;
	  }
	else if(isTriangleInPlaneOfFacet(OYZ))
	  {
	    type = SortOrder::XZ;
	  }
      }

    // create order object
    SortOrder order(barycenter, type);

    // sort vector with this object
    // NB : do not change place of first object, with respect to which the order
    // is defined
    sort((polygon.begin()), polygon.end(), order);
    //stable_sort((++polygon.begin()), polygon.end(), order);
    
    
    std::cout << "Sorted polygon is " << std::endl;
    for(int i = 0 ; i < polygon.size() ; ++i)
      {
	std::cout << vToStr(polygon[i]) << std::endl;
      }

  }

  /**
   * Calculates the volume between the given polygon and the z = 0 plane.
   *
   * @pre  the intersection polygones have been calculated with calculateIntersectionPolygons(),
   *       and they have been sorted in circular order with sortIntersectionPolygons(void)
   * 
   * @param poly        one of the two intersection polygons
   * @param barycenter  array of three doubles with the coordinates of the barycenter
   * @returns           the volume between the polygon and the z = 0 plane
   *
   */
  double TransformedTriangle::calculateVolumeUnderPolygon(IntersectionPolygon poly, const double* barycenter)
  {
    std::cout << "--- Calculating volume under polygon" << std::endl;

    // get the polygon points
    std::vector<double*>& polygon = (poly == A) ? _polygonA : _polygonB;

    double vol = 0.0;
    const int m = polygon.size();

    for(int i = 0 ; i < m ; ++i)
      {
	const double* ptCurr = polygon[i];  // pt "i"
	const double* ptNext = polygon[(i + 1) % m]; // pt "i+1" (pt m == pt 0)
	
	const double factor1 = ptCurr[2] + ptNext[2] + barycenter[2];
	const double factor2 = 
	  ptCurr[0]*(ptNext[1] - barycenter[1]) 
	  + ptNext[0]*(barycenter[1] - ptCurr[1])
	  + barycenter[0]*(ptCurr[1] - ptNext[1]);
	vol += (factor1 * factor2) / 6.0;
      }

    //    std::cout << "Abs. Volume is " << vol << std::endl; 
    return vol;
  }


  ////////////////////////////////////////////////////////////////////////////////////
  /// Detection of (very) degenerate cases                                ////////////
  ////////////////////////////////////////////////////////////////////////////////////

  /**
   * Checks if the triangle lies in the plane of a given facet
   *
   * @param facet     one of the facets of the tetrahedron
   * @returns         true if PQR lies in the plane of the facet, false if not
   */
  bool TransformedTriangle::isTriangleInPlaneOfFacet(const TetraFacet facet)
  {

    // coordinate to check
    const int coord = static_cast<int>(facet);

    for(TriCorner c = P ; c < NO_TRI_CORNER ; c = TriCorner(c + 1))
      {
	// ? should have epsilon-equality here?
	if(_coords[5*c + coord] != 0.0)
	  {
	    return false;
	  }
      }
    
    return true;
  }


  bool TransformedTriangle::isTriangleBelowTetraeder()
  {
    for(TriCorner c = P ; c < NO_TRI_CORNER ; c = TriCorner(c + 1))
      {
	// check z-coords for all points
	if(_coords[5*c + 2] > 0.0)
	  {
	    return false;
	  }
      }
    return true;
  }

void TransformedTriangle::dumpCoords()
{
  std::cout << "Coords : ";
  for(int i = 0 ; i < 3; ++i)
    {
      std::cout << vToStr(&_coords[5*i]) << ",";
    }
  std::cout << std::endl;
}

}; // NAMESPACE
