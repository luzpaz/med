#include "TransformedTriangle.hxx"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <utility>

#include "VectorUtils.hxx"

#undef SECOND_CORNER_RESET
#undef FIXED_DELTA 1.0e-15

namespace INTERP_UTILS
{
  
  ////////////////////////////////////////////////////////////////////////////////////
  /// Constants                                                      /////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  const int TransformedTriangle::DP_OFFSET_1[8] = {1, 2, 0, 2, 0, 1, 4, 1};
  const int TransformedTriangle::DP_OFFSET_2[8] = {2, 0, 1, 3, 3, 3, 0, 4};

  const int TransformedTriangle::COORDINATE_FOR_DETERMINANT_EXPANSION[12] =
    {
      // row 1, 2, 3
      0, 1, 2, // O
      3, 1, 2, // X
      0, 3, 2, // Y
      0, 1, 3  // Z
    };
  
  const TransformedTriangle::DoubleProduct TransformedTriangle::DP_FOR_DETERMINANT_EXPANSION[12] = 
    {
      // row 1, 2, 3
      C_YZ, C_ZX, C_XY, // O
      C_YZ, C_ZH, C_YH, // X
      C_ZH, C_ZX, C_XH, // Y
      C_YH, C_XH, C_XY  // Z
    };
  
  //const double TransformedTriangle::MACH_EPS = 1.0e-15;
  const long double TransformedTriangle::MACH_EPS = std::numeric_limits<double>::epsilon();
  const long double TransformedTriangle::MULT_PREC_F = 4.0 * TransformedTriangle::MACH_EPS;
  const long double TransformedTriangle::THRESHOLD_F = 20.0;

  const double TransformedTriangle::TRIPLE_PRODUCT_ANGLE_THRESHOLD = 0.1;

  ////////////////////////////////////////////////////////////////////////////////////
  /// Double and triple product calculations                           ///////////////
  ////////////////////////////////////////////////////////////////////////////////////
  
  /**
   * Pre-calculates all double products for this triangle, and stores
   * them internally. This method makes compensation for precision errors,
   * and it is thus the "stable" double products that are stored.
   *
   */
  void TransformedTriangle::preCalculateDoubleProducts(void)
  {
    if(_isDoubleProductsCalculated)
      return;

    // -- calculate all unstable double products -- store in _doubleProducts
    for(TriSegment seg = PQ ; seg <= RP ; seg = TriSegment(seg + 1))
      {
	for(DoubleProduct dp = C_YZ ; dp <= C_10 ; dp = DoubleProduct(dp + 1))
	  {
	    _doubleProducts[8*seg + dp] = calcUnstableC(seg, dp);
	  }
      }
  

    // -- (1) for each segment : check that double products satisfy Grandy, [46]
    // -- and make corrections if not
    for(TriSegment seg = PQ ; seg <= RP ; seg = TriSegment(seg + 1))
      {
	if(!areDoubleProductsConsistent(seg))
	  {
	    std::map<double, TetraCorner> distances;
	    LOG(4, "inconsistent! ");

	    for(TetraCorner corner = O ; corner <= Z ; corner = TetraCorner(corner + 1))
	      {
		// calculate distance corner - segment axis
		const double dist = calculateDistanceCornerSegment(corner, seg);
		distances.insert( std::make_pair( dist, corner ) );
	      }

	    // first element -> minimum distance
	    const TetraCorner minCorner = distances.begin()->second;

	    resetDoubleProducts(seg, minCorner);
	    // test : reset also for second corner if distance is small enough
#ifdef SECOND_CORNER_RESET
	    std::map<double, TetraCorner>::const_iterator iter = distances.begin();
	    ++iter;
	    if(iter->first < 1.0e-12)
	      {
		resetDoubleProducts(seg, iter->second);
	      }
#endif	    
	  }

      }
  
  
    // -- (2) check that each double product statisfies Grandy, [47], else set to 0
 
    for(TriSegment seg = PQ ; seg <= RP ; seg = TriSegment(seg + 1))
      {
	for(DoubleProduct dp = C_YZ ; dp <=  C_10 ; dp = DoubleProduct(dp + 1))
	  {
	    // find the points of the triangle
	    // 0 -> P, 1 -> Q, 2 -> R 
	    const int pt1 = seg;
	    const int pt2 = (seg + 1) % 3;

	    // find offsets
	    const int off1 = DP_OFFSET_1[dp];
	    const int off2 = DP_OFFSET_2[dp];

	    const double term1 = _coords[5*pt1 + off1] * _coords[5*pt2 + off2]; 
	    const double term2 = _coords[5*pt1 + off2] * _coords[5*pt2 + off1];
#ifdef FIXED_DELTA
	    const double delta = FIXED_DELTA;
#else
	    const long double delta = MULT_PREC_F * ( std::abs(term1) + std::abs(term2) );
#endif
	  
	    if( epsilonEqual(_doubleProducts[8*seg + dp], 0.0, THRESHOLD_F * delta))
	      {
		if(_doubleProducts[8*seg + dp] != 0.0)
		  {
		    LOG(5, "Double product for (seg,dp) = (" << seg << ", " << dp << ") = " );
		    LOG(5, std::abs(_doubleProducts[8*seg + dp]) << " is imprecise, reset to 0.0" );
		  }
		_doubleProducts[8*seg + dp] = 0.0;
		  
	      }
	  }
      }
    
    _isDoubleProductsCalculated = true;
  }

  bool TransformedTriangle::areDoubleProductsConsistent(const TriSegment seg) const
  {
    const double term1 = _doubleProducts[8*seg + C_YZ] * _doubleProducts[8*seg + C_XH];
    const double term2 = _doubleProducts[8*seg + C_ZX] * _doubleProducts[8*seg + C_YH];
    const double term3 = _doubleProducts[8*seg + C_XY] * _doubleProducts[8*seg + C_ZH];
    

    LOG(6, "for seg " << seg << " consistency " << term1 + term2 + term3 );
    LOG(6, "term1 :" << term1 << " term2 :" << term2 << " term3: " << term3 );
    
    const int num_zero = (term1 == 0.0 ? 1 : 0) + (term2 == 0.0 ? 1 : 0) + (term3 == 0.0 ? 1 : 0);
    const int num_neg = (term1 < 0.0 ? 1 : 0) + (term2 < 0.0 ? 1 : 0) + (term3 < 0.0 ? 1 : 0);
    const int num_pos = (term1 > 0.0 ? 1 : 0) + (term2 > 0.0 ? 1 : 0) + (term3 > 0.0 ? 1 : 0);
    
    assert( num_zero + num_neg + num_pos == 3 );
    
    // calculated geometry is inconsistent if we have one of the following cases
    // * one term zero and the other two of the same sign
    // * two terms zero
    // * all terms positive
    // * all terms negative
    if(((num_zero == 1 && num_neg != 1) || num_zero == 2 || (num_neg == 0 && num_zero != 3) || num_neg == 3 ))
      {
	LOG(4, "inconsistent dp found" );
      }
    return !((num_zero == 1 && num_neg != 1) || num_zero == 2 || (num_neg == 0 && num_zero != 3) || num_neg == 3 );

    //    return (num_zero == 3) || (num_neg != 0 && num_neg != 3 && num_pos != 0 && num_pos != 3);
			       //    return epsilonEqual(term1 + term2 + term3, 0.0);
  }

  void TransformedTriangle::resetDoubleProducts(const TriSegment seg, const TetraCorner corner)
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

  double TransformedTriangle::calculateDistanceCornerSegment(const TetraCorner corner, const TriSegment seg) const
  {
    // NB uses fact that TriSegment <=> TriCorner that is first point of segment (PQ <=> P)
    const TriCorner ptP_idx = TriCorner(seg);
    const TriCorner ptQ_idx = TriCorner( (seg + 1) % 3);
    
    const double ptP[3] = { _coords[5*ptP_idx], _coords[5*ptP_idx + 1], _coords[5*ptP_idx + 2]  };
    const double ptQ[3] = { _coords[5*ptQ_idx], _coords[5*ptQ_idx + 1], _coords[5*ptQ_idx + 2]  };
    
    // coordinates of corner
    const double ptTetCorner[3] = 
      { 
	COORDS_TET_CORNER[3*corner    ],
	COORDS_TET_CORNER[3*corner + 1],
	COORDS_TET_CORNER[3*corner + 2]
      };
    
    // dist^2 = ( PQ x CP )^2 / |PQ|^2 where C is the corner point
    
    // difference vectors
    const double diffPQ[3] = { ptQ[0] - ptP[0], ptQ[1] - ptP[1], ptQ[2] - ptP[2] };
    const double diffCornerP[3] = { ptP[0] - ptTetCorner[0], ptP[1] - ptTetCorner[1], ptP[2] - ptTetCorner[2] };
    
    // cross product of difference vectors
    double crossProd[3];
    cross(diffPQ, diffCornerP, crossProd);
    
    const double cross_squared = dot(crossProd, crossProd);
    const double norm_diffPQ_squared = dot(diffPQ, diffPQ);
    
    assert(norm_diffPQ_squared != 0.0);
    
    return cross_squared / norm_diffPQ_squared;
  }

  /**
   * Pre-calculates all triple products for the tetrahedron with respect to
   * this triangle, and stores them internally. This method takes into account
   * the problem of errors due to cancellation.
   *
   */
  void TransformedTriangle::preCalculateTripleProducts(void)
  {
    if(_isTripleProductsCalculated)
      return;

    LOG(4, "Precalculating triple products" );
    for(TetraCorner corner = O ; corner <= Z ; corner = TetraCorner(corner + 1))
      {
	LOG(6, "- Triple product for corner " << corner );

	// find edge / row to use -> that whose edge makes the smallest angle to the triangle
	// use a map to find the minimum
	std::map<double, int> anglesForRows;

	for(int row = 1 ; row < 4 ; ++row) 
	  {
	    const DoubleProduct dp = DP_FOR_DETERMINANT_EXPANSION[3*corner + (row - 1)];

	    // get edge by using correspondance between Double Product and Edge
	    TetraEdge edge = TetraEdge(dp);
	    
	    // use edge only if it is surrounded by the surface
	    if( testTriangleSurroundsEdge(edge) )
	      {
		// -- calculate angle between edge and PQR
		const double angle = calculateAngleEdgeTriangle(edge);
		anglesForRows.insert(std::make_pair(angle, row));		
	      }
	  }
	
	if(anglesForRows.size() != 0) // we have found a good row
	  {
	    const double minAngle = anglesForRows.begin()->first;
	    const int minRow = anglesForRows.begin()->second;

	    if(minAngle < TRIPLE_PRODUCT_ANGLE_THRESHOLD)
	      {
		_tripleProducts[corner] = calcTByDevelopingRow(corner, minRow, true);
		//_tripleProducts[corner] = calcTByDevelopingRow(corner, 1, false);
	      } 
	    else 
	      {
		 _tripleProducts[corner] = calcTByDevelopingRow(corner, minRow, false);
		//_tripleProducts[corner] = calcTByDevelopingRow(corner, 1, false);
	      }
	    _validTP[corner] = true;
	  }
	else
	  {
	    // this value will not be used
	    // we set it to whatever
	    LOG(6, "Triple product not calculated for corner " << corner );
	    _tripleProducts[corner] = -3.14159265;
	    _validTP[corner] = false;

	  }

      }

    _isTripleProductsCalculated = true;
  }

  double TransformedTriangle::calculateAngleEdgeTriangle(const TetraEdge edge) const
  {
    // find normal to PQR - cross PQ and PR
    const double pq[3] = 
      { 
	_coords[5*Q]     - _coords[5*P], 
	_coords[5*Q + 1] - _coords[5*P + 1],
	_coords[5*Q + 2] - _coords[5*P + 2]
      };
    
    const double pr[3] = 
      { 
	_coords[5*R]     - _coords[5*P], 
	_coords[5*R + 1] - _coords[5*P + 1],
	_coords[5*R + 2] - _coords[5*P + 2]
      };
    
    double normal[3];

    cross(pq, pr, normal);
    
    static const double EDGE_VECTORS[18] =
      {
	1.0, 0.0, 0.0, // OX
	0.0, 1.0, 0.0, // OY
	0.0, 0.0, 1.0, // OZ
	-1.0, 1.0, 0.0, // XY
	0.0,-1.0, 1.0, // YZ
	1.0, 0.0,-1.0 // ZX
      };
    
    const double edgeVec[3] = { 
      EDGE_VECTORS[3*edge],
      EDGE_VECTORS[3*edge + 1],
      EDGE_VECTORS[3*edge + 2],
    };

    //return angleBetweenVectors(normal, edgeVec);

    const double lenNormal = norm(normal);
    const double lenEdgeVec = norm(edgeVec);
    const double dotProd = dot(normal, edgeVec);
    //    return asin( dotProd / ( lenNormal * lenEdgeVec ) );
    //#    assert(dotProd / ( lenNormal * lenEdgeVec ) + 1.0 >= 0.0);
    //#    assert(dotProd / ( lenNormal * lenEdgeVec ) - 1.0 <= 0.0);
    
    //? is this more stable? -> no subtraction
    //    return asin( dotProd / ( lenNormal * lenEdgeVec ) ) + 3.141592625358979 / 2.0;
    return 3.14159262535 - acos( dotProd / ( lenNormal * lenEdgeVec ) );

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
  double TransformedTriangle::calcStableC(const TriSegment seg, const DoubleProduct dp) const
  {
    assert(_isDoubleProductsCalculated);
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
  double TransformedTriangle::calcStableT(const TetraCorner corner) const
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
  double TransformedTriangle::calcUnstableC(const TriSegment seg, const DoubleProduct dp) const
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

  /**
   * Calculates triple product associated with the given corner of tetrahedron, developing 
   * the determinant by the given row. The triple product gives the signed volume of 
   * the tetrahedron between this corner and the triangle PQR. If the flag project is true, 
   * one coordinate is projected out in order to eliminate errors in the intersection point
   * calculation due to cancellation.
   * 
   * @pre            double products have already been calculated
   * @param corner   corner for which the triple product is calculated
   * @param row      row (1 <= row <= 3) used to calculate the determinant
   * @param project  indicates whether or not to perform projection as inidicated in Grandy, p.446
   * @returns        triple product associated with corner (see Grandy, [50]-[52])
   */

  double TransformedTriangle::calcTByDevelopingRow(const TetraCorner corner, const int row, const bool project) const
  {
    
    // OVERVIEW OF CALCULATION
    // --- sign before the determinant
    // the sign used depends on the sign in front of the triple product (Grandy, [15]),
    // and the convention used in the definition of the double products
  
    // the sign in front of the determinant gives the following schema for the three terms (I): 
    // corner/row    1    2   3
    // O (sign:+)    +    -   +
    // X (sign:-)    -    +   -
    // Y (sign:-)    -    +   -
    // Z (sign:-)    -    +   -

    // the 2x2 determinants are the following (C_AB <=> A_p*B_q - B_p*A_q, etc)
    // corner/row    1       2     3
    // O (sign:+)   C_YZ   C_XZ  C_XY
    // X (sign:-)   C_YZ   C_HZ  C_HY
    // Y (sign:-)   C_HZ   C_XZ  C_XH
    // Z (sign:-)   C_YH   C_XH  C_XY

    // these are represented in DP_FOR_DETERMINANT_EXPANSION,
    // except for the fact that certain double products are inversed (C_AB <-> C_BA)

    // comparing with the DOUBLE_PRODUCTS and using the fact that C_AB = -C_BA
    // we deduce the following schema (II) :
    // corner/row    1    2   3
    // O (sign:+)    +    -   +
    // X (sign:-)    +    -   -
    // Y (sign:-)    -    -   +
    // Z (sign:-)    +    +   +

    // comparing the two schemas (I) and (II) gives us the following matrix of signs,
    // putting 1 when the signs in (I) and (II) are equal and -1 when they are different :

    static const int SIGNS[12] = 
      {
	1, 1, 1,
	-1,-1, 1,
	1,-1,-1,
	-1, 1,-1
      };

    // find the offsets of the rows of the determinant
    const int offset = COORDINATE_FOR_DETERMINANT_EXPANSION[3 * corner + (row - 1)];
  
    const DoubleProduct dp = DP_FOR_DETERMINANT_EXPANSION[3 * corner + (row - 1)];

    const int sign = SIGNS[3 * corner + (row - 1)];

    const double cQR = calcStableC(QR, dp);
    const double cRP = calcStableC(RP, dp);
    const double cPQ = calcStableC(PQ, dp);

    double alpha = 0.0;
    
    // coordinate to use for projection (Grandy, [57]) with edges
    // OX, OY, OZ, XY, YZ, ZX in order : 
    // (y, z, x, h, h, h)
    // for the first three we could also use {2, 0, 1}
    static const int PROJECTION_COORDS[6] = { 1, 2, 0, 3, 3, 3 } ;
    
    const int coord = PROJECTION_COORDS[ dp ];
    
    // coordinate values for P, Q and R
    const double coordValues[3] = { _coords[5*P + coord], _coords[5*Q + coord], _coords[5*R + coord] };
    
    if(project)
      {
	// products coordinate values with corresponding double product
	const double coordDPProd[3] = { coordValues[0] * cQR, coordValues[1] * cRP, coordValues[2] * cPQ };
	
	const double sumDPProd = coordDPProd[0] + coordDPProd[1] + coordDPProd[2];
	const double sumDPProdSq = dot(coordDPProd, coordDPProd);

	//	alpha = sumDPProd / sumDPProdSq;
		alpha = (sumDPProdSq != 0.0) ? sumDPProd / sumDPProdSq : 0.0;
      }

    const double cQRbar = cQR * (1.0 - alpha * coordValues[0] * cQR);
    const double cRPbar = cRP * (1.0 - alpha * coordValues[1] * cRP);
    const double cPQbar = cPQ * (1.0 - alpha * coordValues[2] * cPQ);

    // check sign of barred products - should not change
    //    assert(cQRbar * cQR >= 0.0);
    //assert(cRPbar * cRP >= 0.0);
    //assert(cPQbar * cPQ >= 0.0);

    const double p_term = _coords[5*P + offset] * cQRbar;
    const double q_term = _coords[5*Q + offset] * cRPbar;
    const double r_term = _coords[5*R + offset] * cPQbar;

    // check that we are not so close to zero that numerical errors could take over, 
    // otherwise reset to zero (cf Grandy, p. 446)
#ifdef FIXED_DELTA
    const double delta = FIXED_DELTA;
#else
    const long double delta = MULT_PREC_F * (std::abs(p_term) + std::abs(q_term) + std::abs(r_term));
#endif

    if( epsilonEqual( p_term + q_term + r_term, 0.0, THRESHOLD_F * delta) )
      {
	LOG(4, "Reset imprecise triple product for corner " << corner << " to zero" ); 
	return 0.0;
      }
    else
      {
	// NB : using plus also for the middle term compensates for a double product
	// which is inversely ordered
	LOG(6, "Triple product for corner " << corner << ", row " << row << " = " << sign*( p_term + q_term + r_term ) );
	return sign*( p_term + q_term + r_term );
      }

  }

}; // NAMESPACE
