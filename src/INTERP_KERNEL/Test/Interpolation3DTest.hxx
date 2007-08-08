#ifndef __TU_INTERPOLATION_3D_HXX__
#define __TU_INTERPOLATION_3D_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "../Interpolation3D.hxx"

#define ERR_TOL 1.0e-8

using MEDMEM::Interpolation3D;

class Interpolation3DTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( Interpolation3DTest );

  // single - element
  CPPUNIT_TEST( tetraReflexiveUnit );
  CPPUNIT_TEST( tetraReflexiveGeneral );
  CPPUNIT_TEST( tetraNudgedSimpler );
  CPPUNIT_TEST( tetraNudged );
  CPPUNIT_TEST( tetraCorner );
  CPPUNIT_TEST( tetraSimpleIncluded );
  CPPUNIT_TEST( tetraDegenEdge );
  CPPUNIT_TEST( tetraDegenFace );
  CPPUNIT_TEST( tetraHalfstripOnly );
  CPPUNIT_TEST( tetraHalfstripOnly2 );
  CPPUNIT_TEST( tetraSimpleHalfstripOnly );
  CPPUNIT_TEST( generalTetra );

  // multi - element
  CPPUNIT_TEST( tetraComplexIncluded );
  CPPUNIT_TEST( dividedUnitTetraSimplerReflexive );
  CPPUNIT_TEST( dividedUnitTetraReflexive );
  CPPUNIT_TEST( nudgedDividedUnitTetra );
  CPPUNIT_TEST( nudgedDividedUnitTetraSimpler );
  CPPUNIT_TEST( dividedGenTetra );
  //CPPUNIT_TEST( boxReflexive );
  CPPUNIT_TEST( tetraBoxes );

  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

  void tearDown();

  // tests

  void tetraReflexiveUnit()
  {
    intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/UnitTetra.med", "UnitTetra", 1.0/6.0);
  }

  void tetraReflexiveGeneral()
  {
    intersectMeshes("meshes/GeneralTetra.med", "GeneralTetra", "meshes/GeneralTetra.med", "GeneralTetra", 0.428559);
  }

  void tetraNudgedSimpler()
  {
    intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/NudgedSimpler.med", "NudgedSimpler", 0.152112);
  }

  void tetraNudged()
  {
    intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/NudgedTetra.med", "NudgedTetra", 0.142896);
  }

  void tetraCorner()
  {
    intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/CornerTetra.med", "CornerTetra", 0.0135435);
  }

  void tetraSimpleIncluded()
  {
    intersectMeshes("meshes/SimpleIncludedTetra.med", "SimpleIncludedTetra", "meshes/SimpleIncludingTetra.med", "SimpleIncludingTetra", 17.0156, 1.0e-4);
  }

  void tetraComplexIncluded()
  {
    intersectMeshes("meshes/ComplexIncludedTetra.med", "ComplexIncludedTetra", "meshes/ComplexIncludingTetra.med", "ComplexIncludingTetra", 17.0156, 1.0e-4);
  }

  void tetraHalfstripOnly()
  {
    // NB this test is not completely significant : we should also verify that 
    // there are triangles on the element that give a non-zero volume
    intersectMeshes("meshes/HalfstripOnly.med", "HalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", 0.0);
  }

   void tetraHalfstripOnly2()
  {
    // NB this test is not completely significant : we should also verify that 
    // there are triangles on the element that give a non-zero volume
     intersectMeshes("meshes/HalfstripOnly2.med", "HalfstripOnly2", "meshes/UnitTetra.med", "UnitTetra", 0.0);
  }
  
  void tetraSimpleHalfstripOnly()
  {
    // NB this test is not completely significant : we should also verify that 
    // there are triangles on the element that give a non-zero volume
    intersectMeshes("meshes/SimpleHalfstripOnly.med", "SimpleHalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", 0.0);
  }

  void generalTetra()
  {
    intersectMeshes("meshes/GenTetra1.med", "GenTetra1", "meshes/GenTetra2.med", "GenTetra2", 4.91393, 1.0e-5);
  }

  void tetraDegenEdge()
  {
    intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenEdgeXY.med", "DegenEdgeXY", 0.0);
  }

  void tetraDegenFace()
  {
    intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenFaceXYZ.med", "DegenFaceXYZ", 0.0);
  }

  void dividedUnitTetraReflexive()
  {
    intersectMeshes("meshes/DividedUnitTetra.med", "DividedUnitTetra", "meshes/DividedUnitTetra.med", "DividedUnitTetra", 0.1666667);
  }

  void dividedUnitTetraSimplerReflexive()
  {
    intersectMeshes("meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", "meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", 0.1666667);
  }

  void nudgedDividedUnitTetra()
  {
    intersectMeshes("meshes/NudgedDividedUnitTetra.med", "NudgedDividedUnitTetra", "meshes/DividedUnitTetra.med", "DividedUnitTetra", 0.150191);
  }

  void nudgedDividedUnitTetraSimpler()
  {
    intersectMeshes("meshes/NudgedDividedUnitTetraSimpler.med", "NudgedDividedUnitTetraSimpler", "meshes/DividedUnitTetraSimpler.med", "DividedUnitTetraSimpler", 0.150191);
  }

  void dividedGenTetra()
  {
    intersectMeshes("meshes/DividedGenTetra1.med", "DividedGenTetra1", "meshes/DividedGenTetra2.med", "DividedGenTetra2", 0.546329);
  }

  void boxReflexive()
  {
    intersectMeshes("meshes/Box3.med", "Box3", "meshes/Box3.med", "Box3", 13.9954,  1.0e-4);
  }

  void tetraBoxes()
  {
    intersectMeshes("meshes/Box1.med", "Box1", "meshes/Box2.med", "Box2", 124.197, 1.0e-3);
  }

  
  
#if 0
  void tetraHalfstripOnly()
  {
    std::cout << std::endl << std::endl << "=============================" << std::endl;
    IntersectionMatrix matrix;
    // we want no transformation - unit tetra is target
    calcIntersectionMatrix("meshes/HalfstripOnly.med", "HalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", matrix);

    // check that the total volume is zero
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, sumVolume(matrix), 1.0e-6);

    // check that we have non-zero volumes that cancel
    bool allVolumesZero = true;

    for(IntersectionMatrix::const_iterator iter = matrix.begin() ; iter != matrix.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  allVolumesZero = allVolumesZero && (iter2->second == 0.0);
	}
    }

    CPPUNIT_ASSERT_EQUAL(false, allVolumesZero);
    
  }

  void tetraHalfstripOnly2()
  {
    std::cout << std::endl << std::endl << "=============================" << std::endl;
    IntersectionMatrix matrix;
    // we want no transformation - unit tetra is target
    calcIntersectionMatrix("meshes/HalfstripOnly2.med", "HalfstripOnly2", "meshes/UnitTetra.med", "UnitTetra", matrix);

    // check that the total volume is zero
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, sumVolume(matrix), 1.0e-6);

    // check that we have non-zero volumes that cancel
    bool allVolumesZero = true;

    for(IntersectionMatrix::const_iterator iter = matrix.begin() ; iter != matrix.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  allVolumesZero = allVolumesZero && (iter2->second == 0.0);
	}
    }

    CPPUNIT_ASSERT_EQUAL(false, allVolumesZero);
    
  }

   void tetraSimpleHalfstripOnly()
  {
    std::cout << std::endl << std::endl << "=============================" << std::endl;
    IntersectionMatrix matrix;
    // we want no transformation - unit tetra is target
    calcIntersectionMatrix("meshes/SimpleHalfstripOnly.med", "SimpleHalfstripOnly", "meshes/UnitTetra.med", "UnitTetra", matrix);

    // check that the total volume is zero
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, sumVolume(matrix), 1.0e-6);

    // check that we have non-zero volumes that cancel
    bool allVolumesZero = true;

    for(IntersectionMatrix::const_iterator iter = matrix.begin() ; iter != matrix.end() ; ++iter)
    {
      for(map<int, double>::const_iterator iter2 = iter->begin() ; iter2 != iter->end() ; ++iter2)
	{
	  allVolumesZero = allVolumesZero && (iter2->second == 0.0);
	}
    }
    CPPUNIT_ASSERT_EQUAL(false, allVolumesZero);
    
  }
#endif
  void reflexiveTetra();

  void tetraTetraScale();

  void cyl1();

  void box1();

  void tetra1();

  void tetra3();
  
private:

  Interpolation3D* interpolator;

  double sumVolume(IntersectionMatrix m);

  bool Interpolation3DTest::isReflexive(IntersectionMatrix m1, IntersectionMatrix m2);
  
  bool isIntersectionConsistent(IntersectionMatrix m);
  
  void dumpIntersectionMatrix(IntersectionMatrix m);

  void intersectMeshes(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, const double correctVol, const double prec = 1.0e-6);

  void calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m);

};

#endif
