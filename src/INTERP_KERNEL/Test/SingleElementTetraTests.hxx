#ifndef __SINGLE_ELEMENT_TETRA_TESTS_HXX_
#define __SINGLE_ELEMENT_TETRA_TESTS_HXX_

#include "Interpolation3DTestSuite.hxx"

namespace INTERP_TEST 
{
  /**
   * Class testing algorithm by intersecting simple meshes having only one element each. This serves mainly to verify that
   * the volume calculations between elements is correct.
   *
   */
  class SingleElementTetraTests : public Interpolation3DTestSuite
  {
    CPPUNIT_TEST_SUITE( SingleElementTetraTests );

    CPPUNIT_TEST( tetraReflexiveUnit );
    CPPUNIT_TEST( tetraReflexiveGeneral );
    CPPUNIT_TEST( tetraNudgedSimpler );
    CPPUNIT_TEST( tetraNudged );
    CPPUNIT_TEST( tetraCorner );
    CPPUNIT_TEST( tetraSimpleIncluded );
    CPPUNIT_TEST( tetraDegenEdge );
    CPPUNIT_TEST( tetraDegenFace );
    CPPUNIT_TEST( tetraDegenTranslatedInPlane );
    CPPUNIT_TEST( tetraHalfstripOnly );
    CPPUNIT_TEST( tetraHalfstripOnly2 );
    CPPUNIT_TEST( tetraSimpleHalfstripOnly );
    CPPUNIT_TEST( generalTetra );
    CPPUNIT_TEST( trickyTetra1 );
    CPPUNIT_TEST( inconsistentTetra );

    CPPUNIT_TEST_SUITE_END();

  public:

    /// Unit tetrahedron mesh intersecting itself
    /// Status : pass
    void tetraReflexiveUnit()
    {
      _testTools->intersectMeshes("UnitTetra", "UnitTetra", 1.0/6.0);
    }

    /// Tetrahedron mesh with itself
    /// Status : pass
    void tetraReflexiveGeneral()
    {
      _testTools->intersectMeshes("GeneralTetra", "GeneralTetra", 0.428559);
    }

    /// Unit tetrahedron mesh intersecting slightly displaced copy of itself
    /// Status : pass
    void tetraNudged()
    {
      _testTools->intersectMeshes("UnitTetra", "NudgedTetra", 0.142896);
    }

    /// Single-element unit tetrahedron mesh intersecting even slightly displaced (along one axis only) copy of itself
    /// Status : pass
    void tetraNudgedSimpler()
    {
      _testTools->intersectMeshes("UnitTetra", "NudgedSimpler", 0.152112);
    }

    /// Tetrahedron intersecting unit tetrahedron with in non-degenerate way around corner O
    /// Status : pass
    void tetraCorner()
    {
      _testTools->intersectMeshes("UnitTetra", "CornerTetra", 0.0135435);
    }

    /// Tetrahedron situated totally inside another
    /// Status : pass
    void tetraSimpleIncluded()
    {
      _testTools->intersectMeshes("SimpleIncludedTetra", "SimpleIncludingTetra", 17.0156);
    }

    /// Displaced unit tetrahedron intersecting another unit tetrahedron with which it shares an edge
    /// Status : pass
    void tetraDegenEdge()
    {
      _testTools->intersectMeshes("UnitTetraDegenT", "DegenEdgeXY", 0.0);
    }

    /// Displaced unit tetrahedron intersecting another unit tetrahedron with which it shares a face
    /// Status : pass
    void tetraDegenFace()
    {
      _testTools->intersectMeshes("UnitTetraDegenT", "DegenFaceXYZ", 0.0);
    }

    /// Displaced unit tetrahedron intersecting another unit tetrahedron with which it shares a part of the face XYZ
    /// Status : pass
    void tetraDegenTranslatedInPlane()
    {
      _testTools->intersectMeshes("UnitTetraDegenT", "DegenTranslatedInPlane", 0.0571667);
    }

    /// Tetrahedron having only half-strip intersections with the unit tetrahedron
    /// Staus : pass, but does not really test what it should - does not check that the intersections are detected. No longer needed.
    void tetraHalfstripOnly()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("HalfstripOnly", "UnitTetra", 0.0);
    }

    /// Tetrahedron having only half-strip intersections with the unit tetrahedron
    /// Staus : pass, but does not really test what it should - does not check that the intersections are detected. No longer needed.
    void tetraHalfstripOnly2()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("HalfstripOnly2", "UnitTetra", 0.0);
    }
  
    /// Tetrahedron having only half-strip intersections with the unit tetrahedron
    /// Staus : pass, but does not really test what it should - does not check that the intersections are detected. No longer needed.
    void tetraSimpleHalfstripOnly()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("SimpleHalfstripOnly", "UnitTetra", 0.0);
    }

    /// Two intersecting tetrahedra situated in a general position in space
    /// Status : pass
    void generalTetra()
    {
      _testTools->intersectMeshes("GenTetra1", "GenTetra2", 4.91393);
    }

    /// Tetrahedron which is in a tricky position relative to unit tetrahedron.
    /// Status : pass
    void trickyTetra1()
    {
      _testTools->intersectMeshes("UnitTetra", "TrickyTetra1", 0.0);
    }

    /// Two large tetrahedra which nearly share part of an edge and intersect at the origin. Created with goal of getting the as-of-yet uncovered "consistency" test
    /// part of the correction of double products covered. However, it does not succeed with this.
    /// Status : fails, but is quite far-fetched as far as typical use cases are concerned
    void inconsistentTetra()
    {
      _testTools->intersectMeshes("LargeUnitTetra.med", "LargeUnitTetra", "LargeInconsistentTetra.med", "LargeInconsistent", 7.86231e7);
    }

  };
}
#endif
