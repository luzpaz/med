#include "TransformedTriangle.hxx"
#include <iostream>
#include <fstream>
#include <cassert>

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
    // not implemented
    return 0.0;
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
    // not implemented
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
    // not implemented
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
    // not implemented
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
    // not implemented
    return -3.14;
  }


}; // NAMESPACE
