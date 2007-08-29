inline void TransformedTriangle::resetDoubleProducts(const TriSegment seg, const TetraCorner corner)
{
  // set the three corresponding double products to 0.0
  static const DoubleProduct DOUBLE_PRODUCTS[12] =
    {
      C_YZ, C_ZX, C_XY, // O
      C_YZ, C_ZH, C_YH, // X
      C_ZX, C_ZH, C_XH, // Y
      C_XY, C_YH, C_XH  // Z
    };
  
  for(int i = 0 ; i < 3 ; ++i) {
    const DoubleProduct dp = DOUBLE_PRODUCTS[3*corner + i];
    
    LOG(6, std::endl << "resetting inconsistent dp :" << dp << " for corner " << corner);
    _doubleProducts[8*seg + dp] = 0.0;
  };
}

/**
 * Returns the stable double product  c_{xy}^{pq}
 *
 * @pre The stable double products have been calculated with preCalculateDoubleProducts.
 * @param seg   segment of triangle
 * @param dp    double product sought
 *
 * @returns stabilised double product c_{xy}^{pq}
 *
 */
inline double TransformedTriangle::calcStableC(const TriSegment seg, const DoubleProduct dp) const
{
  return _doubleProducts[8*seg + dp];
}

/**
 * Returns the stable triple product t_X for a given corner
 * The triple product gives the signed volume of the tetrahedron between 
 * this corner and the triangle PQR. These triple products have been calculated
 * in a way to avoid problems with cancellation.
 *
 * @pre            double products have already been calculated
 * @pre            triple products have already been calculated
 * @param corner   corner for which the triple product is calculated
 * @returns        triple product associated with corner (see Grandy, eqs. [50]-[52])
 */
inline double TransformedTriangle::calcStableT(const TetraCorner corner) const
{
  assert(_isTripleProductsCalculated);
  assert(_validTP[corner]);
  return _tripleProducts[corner];
}

/**
 * Calculates the given double product c_{xy}^{pq} = x_p*y_q - y_p*x_q for a
 * a segment PQ of the triangle. This method does not compensate for 
 * precision errors.
 *
 * @param seg   segment of triangle
 * @param dp    double product sought
 *
 * @returns double product c_{xy}^{pq}
 *
 */
inline double TransformedTriangle::calcUnstableC(const TriSegment seg, const DoubleProduct dp) const
{
  
  // find the points of the triangle
  // 0 -> P, 1 -> Q, 2 -> R 
  const int pt1 = seg;
  const int pt2 = (seg + 1) % 3;
  
  // find offsets
  const int off1 = DP_OFFSET_1[dp];
  const int off2 = DP_OFFSET_2[dp];
  
  return _coords[5*pt1 + off1] * _coords[5*pt2 + off2] - _coords[5*pt1 + off2] * _coords[5*pt2 + off1];
}

inline void TransformedTriangle::preCalculateTriangleSurroundsEdge() 
{
  for(TetraEdge edge = OX ; edge <= ZX ; edge = TetraEdge(edge + 1))
    {
      _triangleSurroundsEdgeCache[edge] = testTriangleSurroundsEdge(edge);
    }
}

/**
 * Tests if the given edge of the tetrahedron intersects the triangle PQR. (Grandy, eq [17])
 *
 * @param edge   edge of tetrahedron
 * @returns      true if PQR intersects the edge, and the edge is not in the plane of the triangle.
 */
inline bool TransformedTriangle::testSurfaceEdgeIntersection(const TetraEdge edge) const 
{ 
  return _triangleSurroundsEdgeCache[edge] && testEdgeIntersectsTriangle(edge);
}

/**
 * Tests if the given segment of the triangle intersects the given facet of the tetrahedron. 
 * (Grandy, eq. [19])
 *
 * @param seg    segment of the triangle
 * @param facet  facet of the tetrahedron
   * @returns      true if the segment intersects the facet
   */
inline bool TransformedTriangle::testSegmentFacetIntersection(const TriSegment seg, const TetraFacet facet) const 
{ 
  return testFacetSurroundsSegment(seg, facet) && testSegmentIntersectsFacet(seg, facet); 
}

/**
 * Tests if the triangle PQR intersects the ray pointing in the upwards z - direction
 * from the given corner of the tetrahedron. (Grandy eq. [29])
 * 
 * @param corner corner of the tetrahedron on the h = 0 facet (X, Y, or Z)
 * @returns      true if the upwards ray from the corner intersects the triangle. 
 */
inline bool TransformedTriangle::testSurfaceRayIntersection(const TetraCorner corner) const
{ 
  return testTriangleSurroundsRay( corner ) && testSurfaceAboveCorner( corner ); 
}

/**
 * Tests if the given corner of triangle PQR lies in the interior of the unit tetrahedron
 * (0 <= x_p, y_p, z_p, h_p <= 1)
 * 
 * @param corner corner of the triangle PQR
 * @returns      true if the corner lies in the interior of the unit tetrahedron. 
 */
inline bool TransformedTriangle::testCornerInTetrahedron(const TriCorner corner) const
{
  const double pt[4] = 
    {
      _coords[5*corner],     // x
      _coords[5*corner + 1], // y
      _coords[5*corner + 2], // z
      _coords[5*corner + 3]  // z
    };
  
  for(int i = 0 ; i < 4 ; ++i) 
    {
      if(pt[i] < 0.0 || pt[i] > 1.0)
	{
	  return false;
	}
    }
    return true;
}

/**
   * Tests if the given corner of triangle PQR lies on the facet h = 0 (the XYZ facet)
   * (0 <= x_p, y_p, z_p <= 1 && h_p = 0)
   * 
   * @param corner corner of the triangle PQR
   * @returns      true if the corner lies on the facet h = 0
   */
inline  bool TransformedTriangle::testCornerOnXYZFacet(const TriCorner corner) const
  {
    const double pt[4] = 
      {
	_coords[5*corner],     // x
	_coords[5*corner + 1], // y 
	_coords[5*corner + 2], // z
	_coords[5*corner + 3]  // h
      };
    
    if(pt[3] != 0.0) 
      {
	return false;
      }

    for(int i = 0 ; i < 3 ; ++i) 
      {
	if(pt[i] < 0.0 || pt[i] > 1.0)
	  {
	    return false;
	  }
      }
    return true;
  }

inline  bool TransformedTriangle::testCornerAboveXYZFacet(const TriCorner corner) const
  {
    const double x = _coords[5*corner];
    const double y = _coords[5*corner + 1];
    const double h = _coords[5*corner + 3];
    const double H = _coords[5*corner + 4];
        
    return h < 0.0 && H >= 0.0 && x >= 0.0 && y >= 0.0;
	
  }
