#include "TransformedTriangleIntersectTest.hxx"
#include <iostream>
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Intersection tests
// Each method in this file runs all the intersection tests with some triangle. The goal is to cover all
// the different types of intersections between a triangle and a tetrahedron. The table below gives a 
// a summary of what is being tested. Before each method, there is also a summary of what how the 
// triangle in the method intersects the unit tetrahedron.
// 
// Since performing all tests would require a large number of triangles, we have limited our coverage to 
// be such that each column and each row in the table below has at least one entry for each type of 
// intersection. The intersection forumlae are totally symmetric with respect to changing the segment
// (PQ, QR, or RP) of the triangle, so they only enter in a very simple way in the code. Testing 
//  all these cases is therefore of low priority.
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Intersections tested (number indicates first triangle which contains the intersection):
// -----------------------------------------------------------------------------------------------------
// CI  ->  P: 3      Q: 4     R: 7
// COH ->  P: 9      Q: 8     R: 10
// CAH ->  P: 4      Q: 10    R: 9
// -----------------------------------------------------------------------------------------------------
// SF  ->  (PQ, OZX) : 1   (PQ, OYZ) : 2   (PQ, OXY) : 1   (PQ, XYZ) : 3
//     ->  (QR, OZX) : 8   (QR, OYZ) : -   (QR, OXY) : 4   (QR, XYZ) : 7
//     ->  (RP, OZX) : 1   (RP, OYZ) : 3   (RP, OXY) : 7   (RP, XYZ) : 1
// -----------------------------------------------------------------------------------------------------
// SE  ->  (PQ, OX)  : 11  (PQ, OY)  : -   (PQ, OZ)  : 12  (PQ, XY)  : 2   (PQ, ZX)  : -  (PQ, YZ)  : 10
//     ->  (QR, OX)  : -   (QR, OY)  : -   (QR, OZ)  : -   (QR, XY)  : -   (QR, ZX)  : 9  (QR, YZ)  : -
//     ->  (RP, OX)  : -   (RP, OY)  : 12  (RP, OZ)  : -   (RP, XY)  : -   (RP, ZX)  : -  (RP, YZ)  : -
// -----------------------------------------------------------------------------------------------------
// SC  ->  (PQ, O)   : -   (PQ, X)   : -   (PQ, Y)   : 8   (PQ, Z)   : -
//     ->  (QR, O)   : -   (QR, X)   : 2   (QR, Y)   : -   (QR, Z)   : 13
//     ->  (RP, O)   : 11  (RP, X)   : -   (RP, Y)   : -   (RP, Z)   : -
// -----------------------------------------------------------------------------------------------------
// SHS ->  (PQ, XY)  : 3   (PQ, ZX)  : -   (PQ, YZ)  : 13
//     ->  (QR, XY)  : 3   (QR, ZX)  : 5   (QR, YZ)  : 3
//     ->  (RP, XY)  : 1   (RP, ZX)  : 4   (RP, YZ)  : -
// -----------------------------------------------------------------------------------------------------
// SR  ->  (PQ, X)   : 6   (PQ, Y)   : 5   (PQ, Z)   : -
//     ->  (QR, X)   : -   (QR, Y)   : -   (QR, Z)   : 6
//     ->  (RP, X)   : -   (RP, Y)   : -   (RP, Z)   : -
// -----------------------------------------------------------------------------------------------------
// TE  ->  OX : 4   OY : 7    OZ : 8     XY : 1     ZX : 4    YZ : 3
// -----------------------------------------------------------------------------------------------------
// TR  ->  X  : 7    Y : 6     Z : 5
// -----------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Key to triangle descriptions : 
// CI  = Triangle corner contained in tetrahedron 
// COH = Triangle corner on h = 0 face of tetrahedron
// CAH = Triangle corner above h = 0 face of tetrahedron in z-direction
// SF  = Segment - facet intersection
// SE  = Segment - edge intersection
// SC  = Segment - corner intersection
// SHS = Segment - halfstrip intersection
// SR  = Segment - ray intersection
// TE  = Tetrahedron edge intersects triangle (surface - edge intersection)
// TR  = Surface - ray intersection
//
// In the descriptions for each triangle, square brackets indicate superfluous but allowed intersections
// that arise as by-products of for instance segment-corner intersections.
// E.g. A segment - corner intersection can imply three surface - edge intersections
// Since these "extra" intersections arise under special circumstances, they are not counted in the 
// table above
////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

// Triangle 1 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     (PQ, OXY), (PQ, OZX), (RP, XYZ), (RP, OZX)
// SE     -
// SC     - 
// SHS    (RP, XY)
// SR     - 
// TE     XY
// TR     - 

void TransformedTriangleIntersectTest::testTriangle1()
{
  typedef TransformedTriangle TT;

  double coords[9] = 
    {
      0.4,-0.5, 0.5, // P
      0.4, 2.5,-1.0, // Q
      0.4, 2.5, 0.5  // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
  
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::XY));

   // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;

}

// Triangle 2 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     (PQ, OYZ)
// SE     (PQ, XY)
// SC     (QR, X)
// SHS    -
// SR     - 
// TE     [OX, OZ, ZX]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle2()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
     -0.5, 0.5, 0.25, // P
      1.5, 0.5,-0.25, // Q
     -0.5,-1.5, 0.75  // R
    };
  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 3 has the following intersections
// CI     P
// COH    -
// CAH    -
// SF     (PQ, XYZ), (RP, OYZ)
// SE     -
// SC     -
// SHS    (PQ, XY), (QR, YZ), (QR, XY)
// SR     - 
// TE     YZ
// TR     - 

void TransformedTriangleIntersectTest::testTriangle3()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      0.35, 0.15, 0.1, // P
      0.8, 0.8, 0.8,  // Q
     -0.4, 0.3, 0.9   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 4 has the following intersections
// CI     Q
// COH    -
// CAH    P
// SF     (PQ, XYZ), (QR, OXY)
// SE     -
// SC     -
// SHS    (RP, ZX)
// SR     - 
// TE     (OX, ZX)
// TR     - 

void TransformedTriangleIntersectTest::testTriangle4()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      0.3, 0.3, 1.8,  // P
      0.75, 0.1, 0.1,  // Q
      0.2, -1.3, -1.4   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 5 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     -
// SE     -
// SC     -
// SHS    (QR, ZX), (QR, XY)
// SR     (PQ, Y)
// TE     -
// TR     Z

void TransformedTriangleIntersectTest::testTriangle5()
{
std::cout << std::endl << "+++++++ Testing triangle 5" << std::endl;
    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -0.5, 0.5, 2.3,  // P
      0.5, 1.5, 2.8,  // Q
      0.5, -2.6, 1.3   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 6 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     -
// SE     -
// SC     -
// SHS    -
// SR     (PQ, X), (QR, Z) 
// TE     -
// TR     Y 

void TransformedTriangleIntersectTest::testTriangle6()
{
  std::cout << std::endl << "+++++++ Testing triangle 6" << std::endl;

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      1.5, 0.5, 1.35,  // P
      0.5, -0.5, 2.1,  // Q
      -3.0, 3.0, -0.5   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 7 has the following intersections
// CI     R
// COH    -
// CAH    -
// SF     (RP, OXY),(QR,XYZ)
// SE     -
// SC     -
// SHS    (QR, XY)
// SR     - 
// TE     OX, ZX
// TR     X 

void TransformedTriangleIntersectTest::testTriangle7()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -2.3, -1.5, -2.5,  // P
      3.1, 0.15, 0.8,  // Q
      0.3, 0.4, 0.2   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 8 has the following intersections
// CI     [Q] 
// COH    Q
// CAH    -
// SF     (QR, OZX), [ (QR, XYZ) ]
// SE     -
// SC     (PQ,Y)
// SHS    -
// SR     - 
// TE     OZ, [YZ,OY,XY]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle8()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -0.75, 3.25, -1.5,  // P
      0.25, 0.25, 0.5,  // Q
      -0.1, -0.4, 0.9   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 9 has the following intersections
// CI     [P]
// COH    P
// CAH    R
// SF     (PQ, OZX), [(PQ, XYZ), (RP,XYZ)]
// SE     (QR, ZX)
// SC     -
// SHS    -
// SR     - 
// TE     [ZX]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle9()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      0.6, 0.2, 0.2,  // P
      0.3, -0.2, 0.8,  // Q
      0.1, 0.2, 0.8   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 10 has the following intersections
// CI     [R]
// COH    R
// CAH    Q
// SF     (RP, OYZ), [ (RP, XYZ), (QR, XYZ) ]
// SE     (PQ, YZ)
// SC     -
// SHS    -
// SR     - 
// TE     [YZ]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle10()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -0.1, 0.3, 0.6,  // P
      0.1, 0.1, 1.0,  // Q
      0.4, 0.3, 0.3   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}

// Triangle 11 has the following intersections
// CI     Q, R
// COH    -
// CAH    -
// SF     -
// SE     (PQ, OX)
// SC     (RP, O)
// SHS    -
// SR     - 
// TE     [OY, OZ]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle11()
{
  
  typedef TransformedTriangle TT;
  
  double coords[9] =
    {
      -0.2, -0.2, -0.2,  // P
      0.2, 0.1, 0.1,  // Q
      0.3, 0.3, 0.3   // R
    };
  
  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(true , tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}


// Triangle 12 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     (QR, OXY), (QR, OZX)
// SE     (RP, OY), (PQ, OZ)
// SC     -
// SHS    -
// SR     - 
// TE     [OY], [OZ]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle12()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -0.2, 0.2, 0.2,  // P
      0.2, -0.2, 0.3,  // Q
      0.6, 0.6, -0.6   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}


// Triangle 13 has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     (QR, OYZ), (PQ, OXY), (PQ, XYZ)
// SE     -
// SC     (QR, Z)
// SHS    (PQ, YZ)
// SR     - 
// TE     [OZ, YZ, ZX]
// TR     - 

void TransformedTriangleIntersectTest::testTriangle13()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      -0.2, 0.3, 5.0,  // P
      0.2, 0.1, -1.0,  // Q
      -0.2, -0.1, 3.0   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}










///// TEMPLATE ///////////////////////////////



#if 0
// Triangle x has the following intersections
// CI     -
// COH    -
// CAH    -
// SF     -
// SE     -
// SC     -
// SHS    -
// SR     - 
// TE     -
// TR     - 

void TransformedTriangleIntersectTest::testTriangleX()
{

    typedef TransformedTriangle TT;

    double coords[9] =
    {
      0.0, 0.0, 0.0,  // P
      0.0, 0.0, 0.0,  // Q
      0.0, 0.0, 0.0   // R
    };

  TransformedTriangle* tri = new TransformedTriangle(&coords[0], &coords[3], &coords[6]);

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerOnXYZFacet(TT::R));  

  // corner above XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri->testCornerAboveXYZFacet(TT::R));  

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentCornerIntersection(TT::RP, TT::Z));
    
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSegmentRayIntersection(TT::RP, TT::Z));

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceEdgeIntersection(TT::XY));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri->testSurfaceRayIntersection(TT::Z));

  delete tri;
}
#endif
