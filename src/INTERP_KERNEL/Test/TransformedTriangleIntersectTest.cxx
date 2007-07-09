#include "TransformedTriangleIntersectTest.hxx"
#include <iostream>

void TransformedTriangleIntersectTest::setUp()
{
  double coords1[9] = 
    {
      0.4,-0.5, 0.5, // P
      0.4, 2.5,-1.0, // Q
      0.4, 2.5, 0.5  // R
    };

  tri1 = new TransformedTriangle(&coords1[0], &coords1[3], &coords1[6]);

  double coords2[9] =
    {
     -0.5, 0.5, 0.25, // P
      1.5, 0.5,-0.25, // Q
     -0.5,-1.5, 0.75  // R
    };
  tri2 = new TransformedTriangle(&coords2[0], &coords2[3], &coords2[6]);
  
}

void TransformedTriangleIntersectTest::tearDown()
{
  delete tri1;
  delete tri2;
}

// -- Tests intersections for triangle 1
// These are of type surface-edge (SE), segment-facet (SF), and segment-halfstrip (SHs) according to the 
// following tables : 
// 
// (SE) : 
// Edge  Intersection 
// OX        no
// OY        no
// OZ        no
// YZ        no
// ZX        no
// XY        yes
//
// (SF) : 
// Segment / Facet  OZX   OYZ  OXY   XYZ
// PQ               yes   no   yes   no
// QR               no    no   no    no
// RP               yes   no   no    yes
//
// (SHs) : 
// Segment / Halfstrip    YZ  ZX  XY
// PQ                     no  no  no
// QR                     no  no  no
// RP                     no  no  yes

void TransformedTriangleIntersectTest::testTriangle1()
{
  typedef TransformedTriangle TT;

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri1->testSurfaceEdgeIntersection(TT::XY));

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri1->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(true , tri1->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(true , tri1->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(true , tri1->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentCornerIntersection(TT::RP, TT::Z));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSurfaceRayIntersection(TT::Z));
  
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri1->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testSegmentRayIntersection(TT::RP, TT::Z));

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri1->testCornerOnXYZFacet(TT::R));

}


// -- Tests intersections for triangle 2
// The positive tests are for intersections of type segment-edge (SgE), segment-corner (SC),
// surface-edge (SE), and segment-facet (SF) according to the tables :
//
// (SgE) :
// Segment / Edge   OX  OY  OZ  YZ  ZX  XY  
// PQ               no  no  no  no  no  yes
// QR               no  no  no  no  no  no 
// RP               no  no  no  no  no  no
//
// (SC) : 
// Segment / Corner O   X   Y   Z
// PQ               no  no  no  no
// QR               no  yes no  no
// RP               no  no  no  no
//
// (SE) : 
// Edge  Intersection 
// OX        yes       <-- it seems this is ok according to Grandy, p. 439 (one triple product is zero)
// OY        no
// OZ        yes
// YZ        no
// ZX        yes       <-- it seems this is ok according to Grandy, p. 439 (one triple product is zero)
// XY        no
//
// (SF) : 
// Segment / Facet  OZX   OYZ  OXY   XYZ
// PQ               no    yes  no    no
// QR               no    no   no    no
// RP               no    no   no    no
//



void TransformedTriangleIntersectTest::testTriangle2()
{

    typedef TransformedTriangle TT;

  // run all intersection tests and ensure that the ones
  // listed with yes in the tables above return true and 
  // that the ones listed with no or not listed at all return false

  // surface-edge (6 possibilities)
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSurfaceEdgeIntersection(TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceEdgeIntersection(TT::OY));
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSurfaceEdgeIntersection(TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceEdgeIntersection(TT::YZ));
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSurfaceEdgeIntersection(TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceEdgeIntersection(TT::XY));

  // segment-facet (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::PQ, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSegmentFacetIntersection(TT::PQ, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::PQ, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::PQ, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::QR, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::QR, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::QR, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::QR, TT::XYZ));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::RP, TT::OZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::RP, TT::OYZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::RP, TT::OXY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentFacetIntersection(TT::RP, TT::XYZ));

  // segment-edge (18 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::PQ, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::PQ, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::PQ, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSegmentEdgeIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::OX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::OY));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::OZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentEdgeIntersection(TT::RP, TT::XY));

  // segment - corner (12 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::PQ, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::PQ, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::QR, TT::O));
  CPPUNIT_ASSERT_EQUAL(true , tri2->testSegmentCornerIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::RP, TT::O));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentCornerIntersection(TT::RP, TT::Z));

  // surface-ray (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceRayIntersection(TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceRayIntersection(TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSurfaceRayIntersection(TT::Z));
  
  // segment-halfstrip (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::PQ, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::PQ, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::PQ, TT::XY));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::QR, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::QR, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::QR, TT::XY));
  
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::RP, TT::YZ));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::RP, TT::ZX));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentHalfstripIntersection(TT::RP, TT::XY));
  
  // segment-ray (9 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::PQ, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::PQ, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::PQ, TT::Z));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::QR, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::QR, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::QR, TT::Z));

  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::RP, TT::X));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::RP, TT::Y));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testSegmentRayIntersection(TT::RP, TT::Z));

  // corner in tetrahedron (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerInTetrahedron(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerInTetrahedron(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerInTetrahedron(TT::R));
  
  // corner on XYZ facet (3 possibilities)
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerOnXYZFacet(TT::P));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerOnXYZFacet(TT::Q));
  CPPUNIT_ASSERT_EQUAL(false, tri2->testCornerOnXYZFacet(TT::R));

}
