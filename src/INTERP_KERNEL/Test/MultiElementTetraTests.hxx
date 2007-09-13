#ifndef __MULTI_ELEMENT_TETRA_TESTS_HXX_
#define __MULTI_ELEMENT_TETRA_TESTS_HXX_

#include "Interpolation3DTestSuite.hxx"

namespace INTERP_UTILS
{
  class MultiElementTetraTests : public Interpolation3DTestSuite
  {
    CPPUNIT_TEST_SUITE( MultiElementTetraTests );

    CPPUNIT_TEST( dividedUnitTetraSimplerReflexive );
    CPPUNIT_TEST( dividedUnitTetraReflexive );
    CPPUNIT_TEST( nudgedDividedUnitTetra );
    CPPUNIT_TEST( nudgedDividedUnitTetraSimpler );
    CPPUNIT_TEST( dividedGenTetra );
    CPPUNIT_TEST( boxReflexive );
    CPPUNIT_TEST( boxReflexiveModerate );
    CPPUNIT_TEST( tetraBoxes );
    CPPUNIT_TEST( moderateBoxes );
    CPPUNIT_TEST( moderateBoxesSmaller );
    CPPUNIT_TEST( moderateBoxSmallReflexive );
    CPPUNIT_TEST( moderateBoxEvenSmallerReflexive );
    CPPUNIT_TEST( tinyBoxReflexive );

    CPPUNIT_TEST_SUITE_END();
  public:
    void dividedUnitTetraSimplerReflexive()
    {
      _testTools->intersectMeshes("meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", "meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", 0.1666667);
    }

    void dividedUnitTetraReflexive()
    {
      _testTools->intersectMeshes("meshes/DividedUnitTetra.med", "DividedUnitTetra", "meshes/DividedUnitTetra.med", "DividedUnitTetra", 0.1666667);
    }

  
    void nudgedDividedUnitTetra()
    {
      _testTools->intersectMeshes("meshes/NudgedDividedUnitTetra.med", "NudgedDividedUnitTetra", "meshes/DividedUnitTetra.med", "DividedUnitTetra", 0.150191);
    }

    void nudgedDividedUnitTetraSimpler()
    {
      _testTools->intersectMeshes("meshes/NudgedDividedUnitTetraSimpler.med", "NudgedDividedUnitTetraSimpler", "meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", 0.150191);
    }

    void dividedGenTetra()
    {
      _testTools->intersectMeshes("meshes/DividedGenTetra1.med", "DividedGenTetra1", "meshes/DividedGenTetra2.med", "DividedGenTetra2", 0.546329);
    }

    void boxReflexive()
    {
      _testTools->intersectMeshes("meshes/Box3.med", "Box3", "meshes/Box3.med", "Box3", 13.9954);
    }

    void boxReflexiveModerate()
    {
      _testTools->intersectMeshes("meshes/Box1Moderate.med", "Box1Moderate", "meshes/Box1Moderate.med", "Box1Moderate", 1.0e6);
    }

    void tetraBoxes()
    {
      _testTools->intersectMeshes("meshes/Box1.med", "Box1", "meshes/Box2.med", "Box2", 124.197);
    }

    void moderateBoxes()
    {
      _testTools->intersectMeshes("meshes/Box1Moderate.med", "Box1Moderate", "meshes/Box2Moderate.med", "Box2Moderate", 376856);
    }

    void moderateBoxesSmaller()
    {
      _testTools->intersectMeshes("meshes/BoxModSmall1.med", "BoxModSmall1", "meshes/BoxModSmall2.med", "BoxModSmall2", 321853);
    }

    void moderateBoxSmallReflexive()
    {
      _testTools->intersectMeshes("meshes/BoxModSmall1.med", "BoxModSmall1", "meshes/BoxModSmall1.med", "BoxModSmall1", 1.44018e6);
    }

    void moderateBoxEvenSmallerReflexive()
    {
      _testTools->intersectMeshes("meshes/BoxEvenSmaller1.med", "BoxEvenSmaller1", "meshes/BoxEvenSmaller1.med", "BoxEvenSmaller1", 1.44018e6);
    }

    void tinyBoxReflexive()
    {
      _testTools->intersectMeshes("meshes/TinyBox.med", "TinyBox", "meshes/TinyBox.med", "TinyBox", 979200);
    }
  };
}

#endif
