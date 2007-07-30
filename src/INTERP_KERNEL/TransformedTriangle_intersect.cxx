#include "TransformedTriangle.hxx"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <limits>

namespace INTERP_UTILS
{
  
  ////////////////////////////////////////////////////////////////////////////////////
  /// Constants                                                      /////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  const int TransformedTriangle::DP_OFFSET_1[8] = {1, 2, 0, 2, 0, 1, 4, 1};
  const int TransformedTriangle::DP_OFFSET_2[8] = {2, 0, 1, 3, 3, 3, 0, 4};

  const int TransformedTriangle::COORDINATE_FOR_DETERMINANT_EXPANSION[12] =
    {
      0, 1, 2, // O
      3, 1, 2, // X
      0, 3, 2, // Y
      0, 1, 3  // Z
    };
  
  const TransformedTriangle::DoubleProduct TransformedTriangle::DP_FOR_DETERMINANT_EXPANSION[12] = 
    {
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

  // coordinates of corner ptTetCorner
  const double TransformedTriangle::COORDS_TET_CORNER[12] = 
    {
      0.0, 0.0, 0.0, // O
      1.0, 0.0, 0.0, // X
      0.0, 1.0, 0.0, // Y
      0.0, 0.0, 1.0  // Z
    };

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

    // aliases to shorten code
    typedef TransformedTriangle::DoubleProduct DoubleProduct;
    typedef TransformedTriangle::TetraCorner TetraCorner;
    typedef TransformedTriangle::TriSegment TriSegment;
    typedef TransformedTriangle TT; 

    // -- calculate all unstable double products -- store in _doubleProducts
    for(TriSegment seg = TT::PQ ; seg <= TT::RP ; seg = TriSegment(seg + 1))
      {
	for(DoubleProduct dp = TT::C_YZ ; dp <= TT::C_10 ; dp = DoubleProduct(dp + 1))
	  {
	    _doubleProducts[8*seg + dp] = calcUnstableC(seg, dp);
	  }
      }
  

    // -- (1) for each segment : check that double products satisfy Grandy, [46]
    // -- and make corrections if not
    for(TriSegment seg = TT::PQ ; seg <= TT::RP ; seg = TriSegment(seg + 1))
      {
	const double term1 = _doubleProducts[8*seg + TT::C_YZ] * _doubleProducts[8*seg + TT::C_XH];
	const double term2 = _doubleProducts[8*seg + TT::C_ZX] * _doubleProducts[8*seg + TT::C_YH];
	const double term3 = _doubleProducts[8*seg + TT::C_XY] * _doubleProducts[8*seg + TT::C_ZH];

	//	std::cout << std::endl;
	//std::cout << "for seg " << seg << " consistency " << term1 + term2 + term3 << std::endl;
	//std::cout << "term1 :" << term1 << " term2 :" << term2 << " term3: " << term3 << std::endl;

	//	if(term1 + term2 + term3 != 0.0)
	const int num_zero = (term1 == 0.0 ? 1 : 0) + (term2 == 0.0 ? 1 : 0) + (term3 == 0.0 ? 1 : 0);
	const int num_neg = (term1 < 0.0 ? 1 : 0) + (term2 < 0.0 ? 1 : 0) + (term3 < 0.0 ? 1 : 0);
	
	
	if(num_zero == 2 || (num_zero !=3 && num_neg == 0) || (num_zero !=3 && num_neg == 3))
	  {
	    //std::cout << "inconsistent! "<< std::endl;

	    // find TetraCorner nearest to segment
	    double min_dist;
	    TetraCorner min_corner;
	  
	    for(TetraCorner corner = TT::O ; corner <= TT::Z ; corner = TetraCorner(corner + 1))
	      {
		// calculate distance corner - segment axis
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

		//{ use functions in VectorUtils for this

		// cross product of difference vectors
		const double cross[3] = 
		  { 
		    diffPQ[1]*diffCornerP[2] - diffPQ[2]*diffCornerP[1], 
		    diffPQ[2]*diffCornerP[0] - diffPQ[0]*diffCornerP[2],
		    diffPQ[0]*diffCornerP[1] - diffPQ[1]*diffCornerP[0],
		  };

	      
		const double cross_squared = cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2];
		const double norm_diffPQ_squared = diffPQ[0]*diffPQ[0] + diffPQ[1]*diffPQ[1] + diffPQ[2]*diffPQ[2];
		const double dist = cross_squared / norm_diffPQ_squared;
	      
		// update minimum (initializs with first corner)
		if(corner == TT::O || dist < min_dist)
		  {
		    min_dist = dist;
		    min_corner = corner;
		  }
	      }

	    // set the three corresponding double products to 0.0
	    static const DoubleProduct DOUBLE_PRODUCTS[12] =
	      {
		TT::C_YZ, TT::C_ZX, TT::C_XY, // O
		TT::C_YZ, TT::C_ZH, TT::C_YH, // X
		TT::C_ZX, TT::C_ZH, TT::C_XH, // Y
		TT::C_XY, TT::C_YH, TT::C_XH  // Z
	      };

	    for(int i = 0 ; i < 3 ; ++i) {
	      DoubleProduct dp = DOUBLE_PRODUCTS[3*min_corner + i];
	      
	      // std::cout << std::endl << "inconsistent dp :" << dp << std::endl;	      
	      _doubleProducts[8*seg + dp] = 0.0;
	    }

	  }
      }
  
  
    // -- (2) check that each double product statisfies Grandy, [47], else set to 0
 
    for(TriSegment seg = TT::PQ ; seg <= TT::RP ; seg = TriSegment(seg + 1))
      {
	for(DoubleProduct dp = TT::C_YZ ; dp <= TT::C_10 ; dp = DoubleProduct(dp + 1))
	  {
	    // find the points of the triangle
	    // 0 -> P, 1 -> Q, 2 -> R 
	    const int pt1 = seg;
	    const int pt2 = (seg + 1) % 3;

	    // find offsets
	    const int off1 = DP_OFFSET_1[dp];
	    const int off2 = DP_OFFSET_2[dp];

	    const long double term1 = static_cast<long double>(_coords[5*pt1 + off1]) * static_cast<long double>(_coords[5*pt2 + off2]); 
	    const long double term2 = static_cast<long double>(_coords[5*pt1 + off2]) * static_cast<long double>(_coords[5*pt2 + off1]);

	    const long double delta = MULT_PREC_F * ( std::abs(term1) + std::abs(term2) );
	  
	    if( std::abs(static_cast<long double>(_doubleProducts[8*seg + dp])) < THRESHOLD_F*delta )
	      {
		if(_doubleProducts[8*seg + dp] != 0.0)
		  {
		    std::cout << "Double product for (seg,dp) = (" << seg << ", " << dp << ") = " << std::abs(_doubleProducts[8*seg + dp]) << " is imprecise, reset to 0.0" << std::endl;
		  }
		_doubleProducts[8*seg + dp] = 0.0;
		  
	      }
	  }
      }
    
    _isDoubleProductsCalculated = true;
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

    for(TetraCorner corner = O ; corner <= Z ; corner = TetraCorner(corner + 1))
      {
	bool isGoodRowFound = false;

	// find edge / row to use
	int minRow;
	double minAngle;
	bool isMinInitialised = false;

	for(int row = 1 ; row < 4 ; ++row) 
	  {
	    DoubleProduct dp = DP_FOR_DETERMINANT_EXPANSION[3*corner + (row - 1)];

	    // get edge by using correspondance between Double Product and Edge
	    TetraEdge edge = TetraEdge(dp);
	    
	    // use edge only if it is surrounded by the surface
	    if( testTriangleSurroundsEdge(edge) )
	      {
		isGoodRowFound = true;

		// -- calculate angle between edge and PQR
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
	    
		const double normal[3] =
		  {
		    pq[1]*pr[2] - pq[2]*pr[1],
		    pq[2]*pr[0] - pq[0]*pr[2],
		    pq[0]*pr[1] - pq[1]*pr[0]
		  };

		static const double EDGE_VECTORS[18] =
		  {
		    1.0, 0.0, 0.0, // OX
		    0.0, 1.0, 0.0, // OY
		    0.0, 0.0, 1.0, // OZ
		    0.0,-1.0, 1.0, // YZ
		    1.0, 0.0,-1.0, // ZX
		   -1.0,-1.0, 1.0  // XY
		  };

		const double edgeVec[3] = { 
		  EDGE_VECTORS[3*edge],
		  EDGE_VECTORS[3*edge + 1],
		  EDGE_VECTORS[3*edge + 2],
		};

		const double lenNormal = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
		const double lenEdgeVec = sqrt(edgeVec[0]*edgeVec[0] + edgeVec[1]*edgeVec[1] + edgeVec[2]*edgeVec[2]);
		const double dotProd = normal[0]*edgeVec[0] + normal[1]*edgeVec[1] + normal[2]*edgeVec[2];
		
		const double angle = 3.14159262535 - acos( dotProd / ( lenNormal * lenEdgeVec ) );
		
		if(!isMinInitialised || angle < minAngle)
		  {
		    minAngle = angle;
		    minRow = row;
		    isMinInitialised = true;
		  }
		
	      }
	  }
	
	if(isGoodRowFound)
	  {
	    if(minAngle < TRIPLE_PRODUCT_ANGLE_THRESHOLD)
	      {
		_tripleProducts[corner] = calcTByDevelopingRow(corner, minRow, true);
	      } 
	    else 
	      {
		_tripleProducts[corner] = calcTByDevelopingRow(corner, minRow, false);
	      }
	    _validTP[corner] = true;
	  }
	else
	  {
	    // this value will not be used
	    // we set it to whatever
	    // std::cout << "Triple product not calculated for corner " << corner << std::endl;
	    _tripleProducts[corner] = -3.14159265;
	    _validTP[corner] = false;

	  }

      }

    _isTripleProductsCalculated = true;
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
    //    assert(_validTP[corner]);
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

    // coordinate to use for projection (Grandy, [57]) with edges
    // OX, OY, OZ, YZ, ZX, XY in order : 
    // (y, z, x, h, h, h)
    // for the first three we could also use {2, 0, 1}
    static const int PROJECTION_COORDS[6] = { 1, 2, 0, 3, 3, 3 } ;

    double alpha = 0.0;
    
    const int coord = PROJECTION_COORDS[ dp ];
    
    // coordinate values for P, Q and R
    const double coordValues[3] = { _coords[5*P + coord], _coords[5*Q + coord], _coords[5*R + coord] };

    if(project)
      {
	// products coordinate values with corresponding double product
	const double coordDPProd[3] = { coordValues[0] * cQR, coordValues[1] * cRP, coordValues[0] * cPQ };
	
	const double sumDPProd = coordDPProd[0] + coordDPProd[1] + coordDPProd[2];
	const double sumDPProdSq = coordDPProd[0]*coordDPProd[0] + coordDPProd[1]*coordDPProd[1] + coordDPProd[2]*coordDPProd[2];
	alpha = sumDPProd / sumDPProdSq;
      }

    const double p_term = _coords[5*P + offset] * cQR * (1.0 - alpha * coordValues[0] * cQR) ;
    const double q_term = _coords[5*Q + offset] * cRP * (1.0 - alpha * coordValues[1] * cRP) ;
    const double r_term = _coords[5*R + offset] * cPQ * (1.0 - alpha * coordValues[2] * cPQ) ;
    
    // NB : using plus also for the middle term compensates for a double product
    // which is inversely ordered
    //    std::cout << "Triple product for corner " << corner << ", row " << row << " = " << sign*( p_term + q_term + r_term ) << std::endl;
    return sign*( p_term + q_term + r_term );

  }

}; // NAMESPACE
