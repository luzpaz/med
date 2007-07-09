#include "TransformedTriangle.hxx"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

namespace INTERP_UTILS
{

  ////////////////////////////////////////////////////////////////////////////////////
  /// Constants                                                      /////////////////
  ////////////////////////////////////////////////////////////////////////////////////

  // correspondance facet - double product
  // Grandy, table IV
  const TransformedTriangle::DoubleProduct TransformedTriangle::DP_FOR_SEG_FACET_INTERSECTION[12] = 
    {
      C_XH, C_XY, C_ZX, // OYZ
      C_YH, C_YZ, C_XY, // OZX
      C_ZH, C_ZX, C_YZ, // OXY
      C_XH, C_YH, C_ZH  // XYZ
    };
  // signs associated with entries in DP_FOR_SEGMENT_FACET_INTERSECTION
  const double TransformedTriangle::SIGN_FOR_SEG_FACET_INTERSECTION[12] = 
    {
      1.0, 1.0, -1.0,
      1.0, 1.0, -1.0,
      1.0, 1.0, -1.0,
      1.0, 1.0,  1.0
    };

    
  ////////////////////////////////////////////////////////////////////////////////////
  /// Intersection test methods and intersection point calculations           ////////
  ////////////////////////////////////////////////////////////////////////////////////
  /**
   * Tests if the given edge of the tetrahedron intersects the triangle PQR. (Grandy, eq [17])
   *
   * @param edge   edge of tetrahedron
   * @returns      true if PQR intersects the edge, and the edge is not in the plane of the triangle.
   */
  bool TransformedTriangle::testSurfaceEdgeIntersection(const TetraEdge edge) const 
  { 
    return testTriangleSurroundsEdge(edge) && testEdgeIntersectsTriangle(edge); 
  }

  /**
   * Calculates the point of intersection between the given edge of the tetrahedron and the 
   * triangle PQR. (Grandy, eq [22])
   *
   * @pre   testSurfaceEdgeIntersection(edge) returns true
   * @param edge   edge of tetrahedron
   * @param pt     array of three doubles in which to store the coordinates of the intersection point
   */
  void TransformedTriangle::calcIntersectionPtSurfaceEdge(const TetraEdge edge, double* pt) const
  {
    // not implemented
  }

  /**
   * Tests if the given segment of the triangle intersects the given facet of the tetrahedron. 
   * (Grandy, eq. [19])
   *
   * @param seg    segment of the triangle
   * @param facet  facet of the tetrahedron
   * @returns      true if the segment intersects the facet
   */
  bool TransformedTriangle::testSegmentFacetIntersection(const TriSegment seg, const TetraFacet facet) const 
  { 
    return testFacetSurroundsSegment(seg, facet) && testSegmentIntersectsFacet(seg, facet); 
  }

  /**
   * Calculates the point of intersection between the given segment of the triangle
   * and the given facet of the tetrahedron. (Grandy, eq. [23])
   *
   * @pre   testSurfaceEdgeIntersection(seg, facet) returns true
   * 
   * @param seg    segment of the triangle
   * @param facet  facet of the tetrahedron
   * @param pt     array of three doubles in which to store the coordinates of the intersection point
   */
  void TransformedTriangle::calcIntersectionPtSegmentFacet(const TriSegment seg, const TetraFacet facet, double* pt) const
  {
    // not implemented
  }

  /**
   * Tests if the given segment of the triangle intersects the given edge of the tetrahedron (Grandy, eq. [20]
   * 
   * @param seg    segment of the triangle
   * @param edge   edge of tetrahedron
   * @returns      true if the segment intersects the edge 
   */
  bool TransformedTriangle::testSegmentEdgeIntersection(const TriSegment seg, const TetraEdge edge) const
  {
    // facets shared by each edge
    static const TetraFacet FACET_FOR_EDGE[12] =
      {
	OXY, OZX, // OX
	OXY, OYZ, // OY
	OZX, OYZ, // OZ
	OXY, XYZ, // XY
	OYZ, XYZ, // YZ
	OZX, XYZ  // ZX
      };

    if(calcStableC(seg,DoubleProduct( edge )) != 0.0)
      {
	return false;
      } 
    else
      {
	// check condition that the double products for one of the two
	// facets adjacent to the edge has a positive product
	bool isFacetCondVerified = false;
	TetraFacet facet[2];
	for(int i = 0 ; i < 2 ; ++i) 
	  {
	    facet[i] = FACET_FOR_EDGE[2*edge + i];
	    
	    // find the two c-values -> the two for the other edges of the facet
	    int idx1 = 0 ; 
	    int idx2 = 1;
	    DoubleProduct dp1 = DP_FOR_SEG_FACET_INTERSECTION[3*facet[i] + idx1];
	    DoubleProduct dp2 = DP_FOR_SEG_FACET_INTERSECTION[3*facet[i] + idx2];
	    if(dp1 == DoubleProduct( edge ))
	      {
		idx1 = 2;
		dp1 = DP_FOR_SEG_FACET_INTERSECTION[3*facet[i] + idx1];
	      }
	    else if(dp2 == DoubleProduct( edge ))
	      {
		idx2 = 2;
		dp2 = DP_FOR_SEG_FACET_INTERSECTION[3*facet[i] + idx2];
	      }
	    
	    const double c1 = SIGN_FOR_SEG_FACET_INTERSECTION[3*facet[i]+idx1]*calcStableC(seg, dp1);
	    const double c2 = SIGN_FOR_SEG_FACET_INTERSECTION[3*facet[i]+idx2]*calcStableC(seg, dp2);

	    isFacetCondVerified = isFacetCondVerified || c1*c2 > 0.0;
	  }
	if(!isFacetCondVerified)
	  {
	    return false;
	  }
	else
	  {
	    return testSegmentIntersectsFacet(seg, facet[0]) || testSegmentIntersectsFacet(seg, facet[1]);
	  }
      }
  }
    
  /**
   * Calculates the point of intersection between the given segment of the triangle
   * and the given edge of the tetrahedron. (Grandy, eq. [25])
   *
   * @pre   testSegmentEdgeIntersection(seg, edge) returns true
   * 
   * @param seg    segment of the triangle
   * @param edge   edge of the tetrahedron
   * @param pt     array of three doubles in which to store the coordinates of the intersection point
   */
  void TransformedTriangle::calcIntersectionPtSegmentEdge(const TriSegment seg, const TetraEdge edge, double* pt) const 
  {
    // not implemented
  }
    
  /**
   * Tests if the given segment of the triangle intersects the given corner of the tetrahedron.
   * (Grandy, eq. [21])
   *
   * @param seg    segment of the triangle
   * @param corner corner of the tetrahedron
   * @returns      true if the segment intersects the corner
   */
  bool TransformedTriangle::testSegmentCornerIntersection(const TriSegment seg, const TetraCorner corner) const 
  {
    // edges meeting at a given corner
    static const TetraEdge EDGES_FOR_CORNER[12] =
      {
	OX, OY, OZ, // O
	OX, XY, ZX, // X
	OY, XY, YZ, // Y
	OZ, ZX, YZ  // Z
      };

    // facets meeting at a given corner
    static const TetraFacet FACETS_FOR_CORNER[12] =
      {
	OXY, OYZ, OZX, // O
	OZX, OXY, XYZ, // X
	OYZ, XYZ, OXY, // Y
	OZX, XYZ, OYZ  // Z
      };

    // check double products are zero
    for(int i = 0 ; i < 3 ; ++i)
      {
	const TetraEdge edge = EDGES_FOR_CORNER[3*corner + i];
	const DoubleProduct dp = DoubleProduct( edge );
	const double c = calcStableC(seg, dp);
	if(c != 0.0)
	  {
	    return false;
	  }
      }
    
    // check segment intersect a facet
    for(int i = 0 ; i < 3 ; ++i)
      {
	const TetraFacet facet = FACETS_FOR_CORNER[3*corner + i];
	if(testSegmentIntersectsFacet(seg, facet))
	  {
	    return true;
	  }
      }
    
    return false;
  }
    

  /**
   * Tests if the triangle PQR intersects the ray pointing in the upwards z - direction
   * from the given corner of the tetrahedron. (Grandy eq. [29])
   * 
   * @param corner corner of the tetrahedron on the h = 0 facet (X, Y, or Z)
   * @returns      true if the upwards ray from the corner intersects the triangle. 
   */
  bool TransformedTriangle::testSurfaceRayIntersection(const TetraCorner corner) const
  { 
    return testTriangleSurroundsRay( corner ) && testSurfaceAboveCorner( corner ); 
  }

  /**
   * Tests if the given segment of the triangle intersects the half-strip above the 
   * given edge of the h = 0 plane. (Grandy, eq. [30])
   * 
   * @param seg    segment of the triangle
   * @param edge   edge of the h = 0 plane of the tetrahedron (XY, YZ, ZX)
   * @returns      true if the upwards ray from the corner intersects the triangle. 
   */
  bool TransformedTriangle::testSegmentHalfstripIntersection(const TriSegment seg, const TetraEdge edg)
  {
    // get right index here to avoid "filling out" array
    const int edgeIndex = static_cast<int>(edg) - 3;
    
    // double products used in test
    // products 1 and 2 for each edge -> first condition in Grandy [30]
    // products 3 and 4 for each edge -> third condition
    // NB : some uncertainty whether these last are correct
    static const DoubleProduct DP_FOR_HALFSTRIP_INTERSECTION[12] =
      {
	C_10, C_01, C_ZH, C_YZ, // XY
	C_01, C_XY, C_XH, C_ZX, // YZ
	C_XY, C_10, C_YH, C_XY  // ZX
      };

    // facets to use in second condition (S_m)
    static TetraFacet FACET_FOR_HALFSTRIP_INTERSECTION[3] = 
      {
	XYZ, // XY
	OYZ, // YZ
	OZX  // ZX
      };

    const double cVals[4] = 
      {
	calcStableC(seg, DP_FOR_HALFSTRIP_INTERSECTION[4*edgeIndex]),
	calcStableC(seg, DP_FOR_HALFSTRIP_INTERSECTION[4*edgeIndex + 1]),
	calcStableC(seg, DP_FOR_HALFSTRIP_INTERSECTION[4*edgeIndex + 2]),
	calcStableC(seg, DP_FOR_HALFSTRIP_INTERSECTION[4*edgeIndex + 3])
      };
    
    const TetraFacet facet =  FACET_FOR_HALFSTRIP_INTERSECTION[edgeIndex];

    //    std::cout << "Halfstrip tests : " << (cVals[0]*cVals[1] < 0.0) << ", " << testSegmentIntersectsFacet(seg, facet) << ", " << (cVals[2]*cVals[3] > 0.0) << std::endl;
      
    return (cVals[0]*cVals[1] < 0.0) && testSegmentIntersectsFacet(seg, facet) && (cVals[2]*cVals[3] > 0.0);
  }

  /**
   * Calculates the point of intersection between the given segment of the triangle
   * and the halfstrip above the given edge of the tetrahedron. (Grandy, eq. [31])
   *
   * @pre   testSegmentHalfstripIntersection(seg, edge) returns true
   * 
   * @param seg    segment of the triangle
   * @param edge   edge of the tetrahedron defining the halfstrip
   * @param pt     array of three doubles in which to store the coordinates of the intersection point
   */
  void TransformedTriangle::calcIntersectionPtSegmentHalfstrip(const TriSegment seg, const TetraEdge edge, double* pt) const
  {
    // not implemented
  }
    
  /**
   * Tests if the given segment of triangle PQR intersects the ray pointing 
   * in the upwards z - direction from the given corner of the tetrahedron. (Grandy eq. [29])
   * 
   * @param seg    segment of the triangle PQR
   * @param corner corner of the tetrahedron on the h = 0 facet (X, Y, or Z)
   * @returns      true if the upwards ray from the corner intersects the segment. 
   */
  bool TransformedTriangle::testSegmentRayIntersection(const TriSegment seg, const TetraCorner corner) const
  {
    assert(corner == X || corner == Y || corner == Z);

    // readjust index since O is not used
    const int cornerIdx = static_cast<int>(corner) - 1;

    // double products to use in test
    // dp 1   -> cond 1
    // dp 2-7 -> cond 3
    //? NB : last two rows are not completely understood and may contain errors
    static const DoubleProduct DP_SEGMENT_RAY_INTERSECTION[21] = 
      {
	C_10, C_YH, C_ZH, C_01, C_XY, C_YH, C_XY, // X
	C_01, C_XH, C_ZH, C_XY, C_10, C_XH, C_ZX, // Y
	C_XY, C_YH, C_XH, C_10, C_01, C_ZH, C_YZ  // Z
      };

    // facets to use
    //? not sure this is correct
    static const TetraFacet FACET_SEGMENT_RAY_INTERSECTION[3] = 
      {
	OZX, // X
	OXY, // Y
	OYZ, // Z
      };
    

    const DoubleProduct dp0 = DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx];
    
    //? epsilon-equality here?
    if(dp0 == 0.0) // cond. 1
      {
	
	if(testSegmentIntersectsFacet(seg, FACET_SEGMENT_RAY_INTERSECTION[cornerIdx])) // cond. 2.1
	  { 
	  const double H1 = _coords[5*seg + 4];
	  const double H2 = _coords[5*( (seg + 1) % 3) + 4];

	  // S_H -> cond. 2.2
	  //	  std::cout << "H1 = " << H1 << ", H2= " << H2 << std::endl; 
	  if(H1*H2 <= 0.0 && H1 != H2) // should equality be in "epsilon-sense" ?
	    {
    
	      const double cVals[6] = 
		{
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 1]),
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 2]),
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 3]),
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 4]),
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 5]),
		  calcStableC(seg, DP_SEGMENT_RAY_INTERSECTION[7*cornerIdx + 6]),
		};

	      // cond. 3
	      return ( (cVals[0] + cVals[1])*(cVals[2] - cVals[3]) - cVals[4]*cVals[5] ) < 0.0;
	    }
	  }
      }
    
    return false;
  }

  /**
   * Tests if the given corner of triangle PQR lies in the interior of the unit tetrahedron
   * (0 <= x_p, y_p, z_p, h_p <= 1)
   * 
   * @param corner corner of the triangle PQR
   * @returns      true if the corner lies in the interior of the unit tetrahedron. 
   */
  bool TransformedTriangle::testCornerInTetrahedron(const TriCorner corner) const
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
  bool TransformedTriangle::testCornerOnXYZFacet(const TriCorner corner) const
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
    

  ////////////////////////////////////////////////////////////////////////////////////
  /// Utility methods used in intersection tests                       ///////////////
  ////////////////////////////////////////////////////////////////////////////////////
  /**
   * Tests if the triangle PQR surrounds the axis on which the
   * given edge of the tetrahedron lies.
   *
   * @param edge   edge of tetrahedron
   * @returns      true if PQR surrounds edge, false if not (see Grandy, eq. [53])
   */
  bool TransformedTriangle::testTriangleSurroundsEdge(const TetraEdge edge) const
  {
    // NB DoubleProduct enum corresponds to TetraEdge enum according to Grandy, table III
    // so we can use the edge directly
    
    // optimization : we could use _doubleProducts directly here
    const double cPQ = calcStableC(TransformedTriangle::PQ, DoubleProduct(edge));
    const double cQR = calcStableC(TransformedTriangle::QR, DoubleProduct(edge));
    const double cRP = calcStableC(TransformedTriangle::RP, DoubleProduct(edge));

    // if two or more c-values are zero we disallow x-edge intersection
    // Grandy, p.446
    const int numZeros = (cPQ == 0.0 ? 1 : 0) + (cQR == 0.0 ? 1 : 0) + (cRP == 0.0 ? 1 : 0);
   
    return (cPQ*cQR >= 0.0) && (cQR*cRP >=0.0) && (cRP*cPQ >= 0.0) && numZeros < 2;
  }

  /**
   * Tests if the corners of the given edge lie on different sides of the triangle PQR.
   *
   * @param edge   edge of the tetrahedron
   * @returns true if the corners of the given edge lie on different sides of the triangle PQR
   *          or if one (but not both) lies in the plane of the triangle.
   */
  bool TransformedTriangle::testEdgeIntersectsTriangle(const TetraEdge edge) const
  {
    
    assert(edge < H01);

    // correspondance edge - triple products
    // for edges OX, ..., ZX (Grandy, table III)
    static const TetraCorner TRIPLE_PRODUCTS[12] = 
      {
	X, O, // OX
	Y, O, // OY
	Z, O, // OZ 
	Y, Z, // YZ
	Z, X, // ZX
	X, Y  // XY
      };

    // Grandy, [16]
    const double t1 = calcStableT(TRIPLE_PRODUCTS[2*edge]);
    const double t2 = calcStableT(TRIPLE_PRODUCTS[2*edge + 1]);

    //? should equality with zero use epsilon?
    return (t1*t2 <= 0.0) && (t1 - t2 != 0.0);
  }

  /**
   * Tests if the given facet of the tetrahedron surrounds the axis on which the
   * given segment of the triangle lies.
   *
   * @param seg    segment of the triangle
   * @param facet  facet of the tetrahedron
   * @returns      true if the facet surrounds the segment, false if not (see Grandy, eq. [18])
   */
  bool TransformedTriangle::testFacetSurroundsSegment(const TriSegment seg, const TetraFacet facet) const
  {

    const double signs[3] = 
      {
	SIGN_FOR_SEG_FACET_INTERSECTION[3*facet],
	SIGN_FOR_SEG_FACET_INTERSECTION[3*facet + 1],
	SIGN_FOR_SEG_FACET_INTERSECTION[3*facet + 2]
      };
    
    const double c1 = signs[0]*calcStableC(seg, DP_FOR_SEG_FACET_INTERSECTION[3*facet]);
    const double c2 = signs[1]*calcStableC(seg, DP_FOR_SEG_FACET_INTERSECTION[3*facet + 1]);
    const double c3 = signs[2]*calcStableC(seg, DP_FOR_SEG_FACET_INTERSECTION[3*facet + 2]);

    return (c1*c3 > 0.0) && (c2*c3 > 0.0);
  }

  /**
   * Tests if the corners of the given segment lie on different sides of the given facet.
   *
   * @param seg    segment of the triangle
   * @param facet  facet of the tetrahedron
   * @returns true if the corners of the given segment lie on different sides of the facet
   *          or if one (but not both) lies in the plane of the facet. (see Grandy, eq. [18])
   */
  bool TransformedTriangle::testSegmentIntersectsFacet(const TriSegment seg, const TetraFacet facet) const
  {
    // use correspondance facet a = 0 <=> offset for coordinate a in _coords
    // and also correspondance segment AB => corner A
    const double coord1 = _coords[5*seg + facet];
    const double coord2 = _coords[5*( (seg + 1) % 3) + facet];

    //? should we use epsilon-equality here in second test?
    //    std::cout << "coord1 : " << coord1 << " coord2 : " << coord2 << std::endl;
    return (coord1*coord2 <= 0.0) && (coord1 != coord2);
  }

  /**
   * Tests if the triangle PQR lies above a given corner in the z-direction (implying that the 
   * ray pointing upward in the z-direction from the corner can intersect the triangle)
   * (Grandy eq.[28])
   *
   * @param corner corner of the tetrahedron
   * @returns true if the triangle lies above the corner in the z-direction
   */
  bool TransformedTriangle::testSurfaceAboveCorner(const TetraCorner corner) const
  {
    //? is it always YZ here ?
    const double normal = calcStableC(PQ, C_YZ) + calcStableC(QR, C_YZ) + calcStableC(RP, C_YZ);
    return ( calcStableT(corner) * normal ) >= 0.0;
  }

  /**
   * Tests if the triangle PQR surrounds the ray pointing upwards in the z-direction
   * from the given corner.
   *
   * @param corner corner on face XYZ of tetrahedron
   * @returns      true if PQR surrounds ray, false if not (see Grandy, eq. [18])
   */
  bool TransformedTriangle::testTriangleSurroundsRay(const TetraCorner corner) const
  {
    assert(corner == X || corner == Y || corner == Z);

    // double products to use for the possible corners
    static const DoubleProduct DP_FOR_RAY_INTERSECTION[4] = 
      {
	DoubleProduct(0),        // O - only here to fill out and make indices match
	C_10,     // X
	C_01,     // Y
	C_XY      // Z
      };

    const DoubleProduct dp = DP_FOR_RAY_INTERSECTION[corner];

    const double cPQ = calcStableC(PQ, dp);
    const double cQR = calcStableC(QR, dp);
    const double cRP = calcStableC(RP, dp);

    //? NB here we have no correction for precision - is this good?
    // Our authority Grandy says nothing
    return ( cPQ*cQR > 0.0 ) && ( cPQ*cRP > 0.0 );

  }

}; // NAMESPACE
