#ifndef __SINGLE_ELEMENT_TETRA_TESTS_HXX_
#define __SINGLE_ELEMENT_TETRA_TESTS_HXX_

#include "Interpolation3DTestSuite.hxx"

namespace INTERP_UTILS 
{
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
    CPPUNIT_TEST( tetraComplexIncluded );
    CPPUNIT_TEST( tetraHalfstripOnly );
    CPPUNIT_TEST( tetraHalfstripOnly2 );
    CPPUNIT_TEST( tetraSimpleHalfstripOnly );
    CPPUNIT_TEST( generalTetra );
    CPPUNIT_TEST( trickyTetra1 );
    CPPUNIT_TEST( inconsistentTetra );

    CPPUNIT_TEST_SUITE_END();

  public:

    void tetraReflexiveUnit()
    {
      _testTools->intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/UnitTetra.med", "UnitTetra", 1.0/6.0);
    }

    void tetraReflexiveGeneral()
    {
      _testTools->intersectMeshes("meshes/GeneralTetra.med", "GeneralTetra", "meshes/GeneralTetra.med", "GeneralTetra", 0.428559);
    }

    void tetraNudgedSimpler()
    {
      _testTools->intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/NudgedSimpler.med", "NudgedSimpler", 0.152112);
    }

    void tetraNudged()
    {
      _testTools->intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/NudgedTetra.med", "NudgedTetra", 0.142896);
    }

    void tetraCorner()
    {
      _testTools->intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/CornerTetra.med", "CornerTetra", 0.0135435);
    }

    void tetraSimpleIncluded()
    {
      _testTools->intersectMeshes("meshes/SimpleIncludedTetra.med", "SimpleIncludedTetra", "meshes/SimpleIncludingTetra.med", "SimpleIncludingTetra", 17.0156);
    }

    void tetraDegenEdge()
    {
      _testTools->intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenEdgeXY.med", "DegenEdgeXY", 0.0);
    }

    void tetraDegenFace()
    {
      _testTools->intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenFaceXYZ.med", "DegenFaceXYZ", 0.0);
    }

    void tetraDegenTranslatedInPlane()
    {
      _testTools->intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenTranslatedInPlane.med", "DegenTranslatedInPlane", 0.0571667);
    }

    void tetraComplexIncluded()
    {
      _testTools->intersectMeshes("meshes/ComplexIncludedTetra.med", "ComplexIncludedTetra", "meshes/ComplexIncludingTetra.med", "ComplexIncludingTetra", 17.0156);
    }

    void tetraHalfstripOnly()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("meshes/HalfstripOnly.med", "HalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", 0.0);
    }

    void tetraHalfstripOnly2()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("meshes/HalfstripOnly2.med", "HalfstripOnly2", "meshes/UnitTetra.med", "UnitTetra", 0.0);
    }
  
    void tetraSimpleHalfstripOnly()
    {
      // NB this test is not completely significant : we should also verify that 
      // there are triangles on the element that give a non-zero volume
      _testTools->intersectMeshes("meshes/SimpleHalfstripOnly.med", "SimpleHalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", 0.0);
    }

    void generalTetra()
    {
      _testTools->intersectMeshes("meshes/GenTetra1.med", "GenTetra1", "meshes/GenTetra2.med", "GenTetra2", 4.91393);
    }

    void trickyTetra1()
    {
      _testTools->intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/TrickyTetra1.med", "TrickyTetra1", 0.0);
    }

    void inconsistentTetra()
    {
      _testTools->intersectMeshes("meshes/LargeUnitTetra.med", "LargeUnitTetra", "meshes/LargeInconsistentTetra.med", "LargeInconsistent", 7.86231e7);
    }

  };
}
#endif
