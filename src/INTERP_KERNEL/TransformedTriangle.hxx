#ifndef __TRANSFORMED_TRIANGLE_HXX__
#define __TRANSFORMED_TRIANGLE_HXX__

#include <vector>

#ifdef TESTING_INTERP_KERNEL
class TransformedTriangleTest;
class TransformedTriangleIntersectTest;
#endif

namespace INTERP_UTILS
{

  /**
   * Class representing one of the faces of the triangulated source polyhedron after having been transformed
   * with the affine transform that takes the target tetrahedron to the unit tetrahedron. It contains the
   * logic for calculating the volume of intersection between the triangle and the unit tetrahedron.
   *
   * Reference : J. Grandy, "Conservative Remapping and Region Overlays by Intersecting Arbitrary Polyhedra", 
   *             Journal of Computational Physics (1999)
   *
   */
  class TransformedTriangle
  {

  public:

#ifdef TESTING_INTERP_KERNEL
    friend class ::TransformedTriangleTest;
    friend class ::TransformedTriangleIntersectTest;
#endif

    /**
     * Enumerations representing the different geometric elements of the unit tetrahedron
     * and the triangle.
     */
    /// Corners of tetrahedron
    enum TetraCorner { O = 0, X, Y, Z };

    /// Edges of tetrahedron
    enum TetraEdge { OX = 0, OY, OZ, XY, YZ, ZX, H01, H10 };

    /// Facets (faces) of tetrahedron
    enum TetraFacet { OYZ = 0, OZX, OXY, XYZ };

    /// Corners of triangle
    enum TriCorner { P = 0, Q, R };
    
    /// Segments (edges) of triangle
    enum TriSegment { PQ = 0, QR, RP };
    
    /// Polygons
    enum IntersectionPolygon{ A = 0, B };

    /// Double products
    /// NB : order corresponds to TetraEdges (Grandy, table III)
    enum DoubleProduct { C_YZ = 0, C_ZX, C_XY, C_ZH, C_XH, C_YH, C_01, C_10 };

    TransformedTriangle(double* p, double* q, double* r); 
    ~TransformedTriangle();

    double calculateIntersectionVolume(); 
    
  private:
    
    ////////////////////////////////////////////////////////////////////////////////////
    /// High-level methods called directly by calculateIntersectionVolume()     ////////
    ////////////////////////////////////////////////////////////////////////////////////
 
    void calculateIntersectionPolygons(); 

    void calculatePolygonBarycenter(const IntersectionPolygon poly, double* barycenter); 

    void sortIntersectionPolygon(const IntersectionPolygon poly, const double* barycenter); 

    double calculateVolumeUnderPolygon(IntersectionPolygon poly, const double* barycenter); 


    ////////////////////////////////////////////////////////////////////////////////////
    /// Intersection test methods and intersection point calculations           ////////
    ////////////////////////////////////////////////////////////////////////////////////
 
    bool testSurfaceEdgeIntersection(const TetraEdge edge) const; 

    void calcIntersectionPtSurfaceEdge(const TetraEdge edge, double* pt) const;  

    bool testSegmentFacetIntersection(const TriSegment seg, const TetraFacet facet) const; 

    void calcIntersectionPtSegmentFacet(const TriSegment seg, const TetraFacet facet, double* pt) const;  

    bool testSegmentEdgeIntersection(const TriSegment seg, const TetraEdge edge) const; 
 
    void calcIntersectionPtSegmentEdge(const TriSegment seg, const TetraEdge edge, double* pt) const ; 

    bool testSegmentCornerIntersection(const TriSegment seg, const TetraCorner corner) const ;

    bool testSurfaceRayIntersection(const TetraCorner corner) const;

    bool testSegmentHalfstripIntersection(const TriSegment seg, const TetraEdge edg);

    void calcIntersectionPtSegmentHalfstrip(const TriSegment seg, const TetraEdge edge, double* pt) const;
    
    bool testSegmentRayIntersection(const TriSegment seg, const TetraCorner corner) const;

    bool testCornerInTetrahedron(const TriCorner corner) const;

    bool testCornerOnXYZFacet(const TriCorner corner) const;
    

    ////////////////////////////////////////////////////////////////////////////////////
    /// Utility methods used in intersection tests                       ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
    
    bool testTriangleSurroundsEdge(const TetraEdge edge) const;

    bool testEdgeIntersectsTriangle(const TetraEdge edge) const;

    bool testFacetSurroundsSegment(const TriSegment seg, const TetraFacet facet) const;

    bool testSegmentIntersectsFacet(const TriSegment seg, const TetraFacet facet) const;

    bool testSurfaceAboveCorner(const TetraCorner corner) const;
    
    bool testTriangleSurroundsRay(const TetraCorner corner) const;

    ////////////////////////////////////////////////////////////////////////////////////
    /// Double and triple product calculations                           ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
    
    void preCalculateDoubleProducts(void);
    
    void preCalculateTripleProducts(void);

    double calcStableC(const TriSegment seg, const DoubleProduct dp) const;

    double calcStableT(const TetraCorner corner) const;

    double calcUnstableC(const TriSegment seg, const DoubleProduct dp) const;

    double calcTByDevelopingRow(const TetraCorner corner, const int row = 1, const bool project = false) const;

    ////////////////////////////////////////////////////////////////////////////////////
    /// Member variables                                                 ///////////////
    ////////////////////////////////////////////////////////////////////////////////////
  private:

    // storage : 
    // [ p_x, p_y, p_z, p_h, p_H, q_x, q_y, q_z, q_h, q_H, r_x, r_y, r_z, r_h, r_H ]
    double _coords[15];

    bool _isDoubleProductsCalculated, _isTripleProductsCalculated;
    double _doubleProducts[24];
    double _tripleProducts[4];
    std::vector<double*> _polygonA, _polygonB;
    double _barycenterA[3], _barycenterB[3];

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
    static const double MACH_EPS;    // machine epsilon
    static const double MULT_PREC_F; // precision of multiplications (Grandy : f)
    static const double THRESHOLD_F; // threshold for zeroing (Grandy : F/f)

    static const double TRIPLE_PRODUCT_ANGLE_THRESHOLD;

    // correspondance facet - double product
    // Grandy, table IV
    static const DoubleProduct DP_FOR_SEG_FACET_INTERSECTION[12];

    // signs associated with entries in DP_FOR_SEGMENT_FACET_INTERSECTION
    static const double SIGN_FOR_SEG_FACET_INTERSECTION[12];
    

  };

};

#endif
