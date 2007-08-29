#ifndef __TRANSFORMED_TRIANGLE_HXX__
#define __TRANSFORMED_TRIANGLE_HXX__

#include <vector>

#undef EPS_TESTING  // does not give good results

#ifdef EPS_TESTING
#define TEST_EPS 1.0e-14
#endif

#ifdef OPTIMIZE
#define OPT_INLINE inline
#else
#define OPT_INLINE 
#endif

// Levels : 
// 1 - overview of algorithm + volume result
// 2 - algorithm detail
// 3 - intersection polygon results detail
// 4 - intersection polygon search detail
// higher -> misc. gory details of calculation

#include "Log.hxx"

class TransformedTriangleTest;
class TransformedTriangleIntersectTest;

namespace INTERP_UTILS
{

  /**
   * Class representing one of the faces of the triangulated source polyhedron after having been transformed
   * with the affine transform that takes the target tetrahedron to the unit tetrahedron. It contains the
   * logic for calculating the volume of intersection between the triangle and the unit tetrahedron.
   *
   * Reference : J. Grandy, "Conservative Remapping and Region Overlays by Intersecting Arbitrary Polyhedra", 
   *             Journal of Computational Physics (1999)
   */

  /* ! READ ME FIRST !
   * OVERVIEW of how the class works : (details can be found in the commentaries above each method)
   * 
   * Constructor : 
   * The constructor takes as arguments three pointers to double[3] vectors holding the transformed
   * coordinates of the corners of the triangle. It copies there coordinates and then proceeds to pre-calculating certain
   * entities used in the intersection calculation : the double products, triple products and the values of the function E
   * (Grandy, [53]).
   *
   * calculateIntersectionVolume() : 
   * This is the only method in the public interface. It calculates the volume under the intersection polygons
   * between the triangle and the unit tetrahedron, as described in Grandy, pp. 435-447. It does this by first calculating the
   * intersection polygons A and B, with the method calculateIntersectionPolygons(). It then calculates the barycenter of each
   * polygon in calculatePolygonBarycenter(), and sorts their points in a circular order around the barycenter in 
   * sortIntersecionPolygon(). The sorting is done with STL sort, using the order defined in the class 
   * ProjectedCentralCircularSortOrder. The volume under each polygon is then calculated with calculateVolumeUnderPolygon(), which
   * implements formula [34] in Grandy.
   *
   * calculateIntersectionPolygons() :
   * This method goes through all the possible ways in which the triangle can intersect the tetrahedron and tests for these 
   * types of intersections in accordance with the formulas described in Grandy. These tests are implemented in the test* - methods.
   * The formulas in the article are stated for one case each only, while the calculation must take into account all cases. 
   * To this end, a number of tables, implemented as static const arrays of different types, are used. The tables 
   * mainly contain values of the different enumeration types described at the beginning of the class interface. For example, 
   * the formula Grandy gives for the segment-halfstrip intersection tests ([30]) is for use with the halfstrip above the zx edge. 
   * For the other two halfstrips (above the xy and yz edges), other double products are used. 
   * These double products are stored in the table DP_FOR_HALFSTRIP_INTERSECTION, which permits to treat
   * all the edges equally, avoiding switch() - statements. It is the careful choice of order of the enumeration types that makes this
   * possible. Notably, there is a correspondance between the TetraEdge type and the DoubleProduct type (see Grandy, tatble III) that
   * is used throughout the code, permitting statements such as DoubleProduct(my_edge) to work.
   *    When an intersection point has been detected it is calculated with a corresponding calc* - method in the cases where it
   * is not known directly. It is then added to the polygon A and/or B as necessary.
   *
   *
   */
  class TransformedTriangle
  {

  public:


    friend class ::TransformedTriangleTest;
    friend class ::TransformedTriangleIntersectTest;


    /**
     * Enumerations representing the different geometric elements of the unit tetrahedron
     * and the triangle.
     */
    /// Corners of tetrahedron
    enum TetraCorner { O = 0, X, Y, Z, NO_TET_CORNER };

    /// Edges of tetrahedron
    enum TetraEdge { OX = 0, OY, OZ, XY, YZ, ZX, H01, H10, NO_TET_EDGE };

    /// Facets (faces) of tetrahedron
    enum TetraFacet { OYZ = 0, OZX, OXY, XYZ, NO_TET_FACET };

    /// Corners of triangle
    enum TriCorner { P = 0, Q, R, NO_TRI_CORNER };
    
    /// Segments (edges) of triangle
    enum TriSegment { PQ = 0, QR, RP, NO_TRI_SEGMENT };
    
    /// Polygons
    enum IntersectionPolygon{ A = 0, B, NO_INTERSECTION_POLYGONS };

    /// Double products
    /// NB : order corresponds to TetraEdges (Grandy, table III)
    enum DoubleProduct { C_YZ = 0, C_ZX, C_XY, C_ZH, C_XH, C_YH, C_01, C_10, NO_DP };

    TransformedTriangle(double* p, double* q, double* r); 
    ~TransformedTriangle();

    double calculateIntersectionVolume(); 

    void dumpCoords();

    
  private:
    
    ////////////////////////////////////////////////////////////////////////////////////
    /// High-level methods called directly by calculateIntersectionVolume()     ////////
    ////////////////////////////////////////////////////////////////////////////////////
 
    void calculateIntersectionPolygons(); 

    void calculatePolygonBarycenter(const IntersectionPolygon poly, double* barycenter); 

    void sortIntersectionPolygon(const IntersectionPolygon poly, const double* barycenter); 

    double calculateVolumeUnderPolygon(IntersectionPolygon poly, const double* barycenter); 

    ////////////////////////////////////////////////////////////////////////////////////
    /// Detection of (very) degenerate cases                                ////////////
    ////////////////////////////////////////////////////////////////////////////////////

    bool isTriangleInPlaneOfFacet(const TetraFacet facet);

    bool isTriangleBelowTetraeder();

    bool isPolygonAOnHFacet() const;


    ////////////////////////////////////////////////////////////////////////////////////
    /// Intersection test methods and intersection point calculations           ////////
    ////////////////////////////////////////////////////////////////////////////////////
 
    OPT_INLINE bool testSurfaceEdgeIntersection(const TetraEdge edge) const; 

    void calcIntersectionPtSurfaceEdge(const TetraEdge edge, double* pt) const;  

    OPT_INLINE bool testSegmentFacetIntersection(const TriSegment seg, const TetraFacet facet) const; 

    void calcIntersectionPtSegmentFacet(const TriSegment seg, const TetraFacet facet, double* pt) const;  

    bool testSegmentEdgeIntersection(const TriSegment seg, const TetraEdge edge) const; 
 
    void calcIntersectionPtSegmentEdge(const TriSegment seg, const TetraEdge edge, double* pt) const ; 

    bool testSegmentCornerIntersection(const TriSegment seg, const TetraCorner corner) const ;

    OPT_INLINE bool testSurfaceRayIntersection(const TetraCorner corner) const;

    bool testSegmentHalfstripIntersection(const TriSegment seg, const TetraEdge edg);

    void calcIntersectionPtSegmentHalfstrip(const TriSegment seg, const TetraEdge edge, double* pt) const;
    
    bool testSegmentRayIntersection(const TriSegment seg, const TetraCorner corner) const;

    OPT_INLINE bool testCornerInTetrahedron(const TriCorner corner) const;

    OPT_INLINE bool testCornerOnXYZFacet(const TriCorner corner) const;

    OPT_INLINE bool testCornerAboveXYZFacet(const TriCorner corner) const;

    ////////////////////////////////////////////////////////////////////////////////////
    /// Utility methods used in intersection tests                       ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
    
    bool testTriangleSurroundsEdge(const TetraEdge edge) const;

    bool testEdgeIntersectsTriangle(const TetraEdge edge) const;

    bool testFacetSurroundsSegment(const TriSegment seg, const TetraFacet facet) const;

    bool testSegmentIntersectsFacet(const TriSegment seg, const TetraFacet facet) const;

    bool testSegmentIntersectsHPlane(const TriSegment seg) const;

    bool testSurfaceAboveCorner(const TetraCorner corner) const;
    
    bool testTriangleSurroundsRay(const TetraCorner corner) const;

    ////////////////////////////////////////////////////////////////////////////////////
    /// Double and triple product calculations                           ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
    
    void preCalculateDoubleProducts(void);

    bool areDoubleProductsConsistent(const TriSegment seg) const;

    OPT_INLINE void resetDoubleProducts(const TriSegment seg, const TetraCorner corner);

    double calculateDistanceCornerSegment(const TetraCorner corner, const TriSegment seg) const;
    
    void preCalculateTripleProducts(void);

    double calculateAngleEdgeTriangle(const TetraEdge edge) const;

    OPT_INLINE double calcStableC(const TriSegment seg, const DoubleProduct dp) const;

    OPT_INLINE double calcStableT(const TetraCorner corner) const;

    OPT_INLINE double calcUnstableC(const TriSegment seg, const DoubleProduct dp) const;

    double calcTByDevelopingRow(const TetraCorner corner, const int row = 1, const bool project = false) const;

    ////////////////////////////////////////////////////////////////////////////////////
    /// Member variables                                                 ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
  private:

    // order : 
    // [ p_x, p_y, p_z, p_h, p_H, q_x, q_y, q_z, q_h, q_H, r_x, r_y, r_z, r_h, r_H ]
    double _coords[15];
    
    /// flags showing whether the double and triple products have been precalculated for this class
    bool _isDoubleProductsCalculated, _isTripleProductsCalculated; 

    /// array containing the 24 double products
    /// order : c^PQ_YZ, ... ,cPQ_10, ... c^QR_YZ, ... c^RP_YZ
    /// following order in enumeration DoubleProduct
    double _doubleProducts[24];

    /// array containing the 4 triple products
    /// order : t_O, t_X, t_Y, t_Z
    double _tripleProducts[4];

    /// arrays holding the points in the two intersection polygons A and B
    /// these points are allocated in calculateIntersectionPolygons() and liberated in the destructor
    std::vector<double*> _polygonA, _polygonB;
    
    /// vectors holding the coordinates of the barycenters of the polygons A and B
    /// these points are calculated in calculatePolygonBarycenter
    double _barycenterA[3], _barycenterB[3];

    // used for debugging
    bool _validTP[4];

#ifdef OPTIMIZE
    void preCalculateTriangleSurroundsEdge();     
    bool _triangleSurroundsEdgeCache[NO_TET_EDGE];
    bool _isOutsideTetra;
#endif


    ////////////////////////////////////////////////////////////////////////////////////
    /// Constants                                                      /////////////////
    ////////////////////////////////////////////////////////////////////////////////////

    // offsets : 0 -> x, 1 -> y, 2 -> z, 3 -> h, 4 -> H
    // corresponds to order of double products in DoubleProduct
    // so that offset[C_*] gives the right coordinate
    static const int DP_OFFSET_1[8];
    static const int DP_OFFSET_2[8];

    // the coordinates used in the expansion of triple products by a given row
    // in constellation (corner, row-1)
    // (0,1,2,3) <=> (x,y,z,h)
    static const int COORDINATE_FOR_DETERMINANT_EXPANSION[12];

    // contains the edge of the double product used when 
    // expanding the triple product determinant associated with each corner
    // by a given row
    static const DoubleProduct DP_FOR_DETERMINANT_EXPANSION[12];
    
    // values used to decide how imprecise the double products 
    // should be to set them to 0.0
    static const long double MACH_EPS;    // machine epsilon
    static const long double MULT_PREC_F; // precision of multiplications (Grandy : f)
    static const long double THRESHOLD_F; // threshold for zeroing (Grandy : F/f)

    static const double TRIPLE_PRODUCT_ANGLE_THRESHOLD;

    // correspondance facet - double product
    // Grandy, table IV
    static const DoubleProduct DP_FOR_SEG_FACET_INTERSECTION[12];

    // signs associated with entries in DP_FOR_SEGMENT_FACET_INTERSECTION
    static const double SIGN_FOR_SEG_FACET_INTERSECTION[12];
    
    // coordinates of corners of tetrahedron
    static const double COORDS_TET_CORNER[12];
    
    // indices to use in tables DP_FOR_SEG_FACET_INTERSECTION and SIGN_FOR_SEG_FACET_INTERSECTION
    // for the calculation of the coordinates (x,y,z) of the intersection points
    // for Segment-Facet and Segment-Edge intersections
    static const int TransformedTriangle::DP_INDEX[12];

    // correspondance edge - corners
    static const TetraCorner CORNERS_FOR_EDGE[12];

    // correspondance edge - facets
    // facets shared by each edge
    static const TetraFacet FACET_FOR_EDGE[12];

    // correspondance edge - corners
    static const TetraEdge EDGES_FOR_CORNER[12];
   
    // double products used in segment-halfstrip test
    static const DoubleProduct DP_FOR_HALFSTRIP_INTERSECTION[12];

    // double products used in segment - ray test
    static const DoubleProduct DP_SEGMENT_RAY_INTERSECTION[21];


    inline bool isTriangleOutsideTetra(void) const;

  };



  // include definitions of inline methods
#ifdef OPTIMIZE

#include "TransformedTriangle_inline.hxx"
  
#endif

};

#endif
