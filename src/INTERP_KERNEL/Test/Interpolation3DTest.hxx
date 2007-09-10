#ifndef __TU_INTERPOLATION_3D_HXX__
#define __TU_INTERPOLATION_3D_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "../Interpolation3D.hxx"

#define ERR_TOL 1.0e-8

using MEDMEM::Interpolation3D;
class MEDMEM::MESH;

class Interpolation3DTest : public CppUnit::TestFixture
{

  // single - element
  CPPUNIT_TEST_SUITE( Interpolation3DTest );
#if 0
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


  // multi - element  
 
  CPPUNIT_TEST( tetraComplexIncluded );

  CPPUNIT_TEST( dividedUnitTetraSimplerReflexive );
  CPPUNIT_TEST( dividedUnitTetraReflexive );

  CPPUNIT_TEST( nudgedDividedUnitTetra );
  CPPUNIT_TEST( nudgedDividedUnitTetraSimpler );
  CPPUNIT_TEST( dividedGenTetra );
  CPPUNIT_TEST( boxReflexive );
  CPPUNIT_TEST( boxReflexiveModerate );
  CPPUNIT_TEST( tetraBoxes );
  CPPUNIT_TEST( moderateBoxes );
#endif
  CPPUNIT_TEST( moderateBoxesSmaller );
  CPPUNIT_TEST( moderateBoxSmallReflexive );
#if 0
  CPPUNIT_TEST( moderateBoxEvenSmallerReflexive );
  CPPUNIT_TEST( tinyBoxReflexive );

  CPPUNIT_TEST( simpleHexaBox );
#endif
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
    intersectMeshes("meshes/SimpleIncludedTetra.med", "SimpleIncludedTetra", "meshes/SimpleIncludingTetra.med", "SimpleIncludingTetra", 17.0156);
  }

  void tetraComplexIncluded()
  {
    intersectMeshes("meshes/ComplexIncludedTetra.med", "ComplexIncludedTetra", "meshes/ComplexIncludingTetra.med", "ComplexIncludingTetra", 17.0156);
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
    intersectMeshes("meshes/GenTetra1.med", "GenTetra1", "meshes/GenTetra2.med", "GenTetra2", 4.91393);
  }

  void trickyTetra1()
  {
    intersectMeshes("meshes/UnitTetra.med", "UnitTetra", "meshes/TrickyTetra1.med", "TrickyTetra1", 0.0);
  }

  void inconsistentTetra()
  {
    intersectMeshes("meshes/LargeUnitTetra.med", "LargeUnitTetra", "meshes/LargeInconsistentTetra.med", "LargeInconsistent", 7.86231e7);
  }


  void tetraDegenEdge()
  {
    intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenEdgeXY.med", "DegenEdgeXY", 0.0);
  }

  void tetraDegenFace()
  {
    intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenFaceXYZ.med", "DegenFaceXYZ", 0.0);
  }

  void tetraDegenTranslatedInPlane()
  {
    intersectMeshes("meshes/UnitTetraDegenT.med", "UnitTetraDegenT", "meshes/DegenTranslatedInPlane.med", "DegenTranslatedInPlane", 0.0571667);
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
    intersectMeshes("meshes/Box3.med", "Box3", "meshes/Box3.med", "Box3", 13.9954);
  }

  void boxReflexiveModerate()
  {
    intersectMeshes("meshes/Box1Moderate.med", "Box1Moderate", "meshes/Box1Moderate.med", "Box1Moderate", 1.0e6);
  }

  void tetraBoxes()
  {
    intersectMeshes("meshes/Box1.med", "Box1", "meshes/Box2.med", "Box2", 124.197);
  }

  void moderateBoxes()
  {
    intersectMeshes("meshes/Box1Moderate.med", "Box1Moderate", "meshes/Box2Moderate.med", "Box2Moderate", 376856);
  }

  void moderateBoxesSmaller()
  {
    intersectMeshes("meshes/BoxModSmall1.med", "BoxModSmall1", "meshes/BoxModSmall2.med", "BoxModSmall2", 321853);
  }

  void moderateBoxSmallReflexive()
  {
    intersectMeshes("meshes/BoxModSmall1.med", "BoxModSmall1", "meshes/BoxModSmall1.med", "BoxModSmall1", 1.44018e6);
  }

  void moderateBoxEvenSmallerReflexive()
  {
    intersectMeshes("meshes/BoxEvenSmaller1.med", "BoxEvenSmaller1", "meshes/BoxEvenSmaller1.med", "BoxEvenSmaller1", 1.44018e6);
  }

  void tinyBoxReflexive()
  {
    intersectMeshes("meshes/TinyBox.med", "TinyBox", "meshes/TinyBox.med", "TinyBox", 979200);
  }

  void simpleHexaBox()
  {
    intersectMeshes("meshes/BoxHexa1.med", "BoxHexa1", "meshes/BoxTetra2.med", "BoxTetra2", 65250, 1.0e-5, false);
  }

private:

  Interpolation3D* interpolator;

  double sumRow(const IntersectionMatrix& m, int i) const;

  double sumCol(const IntersectionMatrix& m, int i) const;

  void getVolumes( MEDMEM::MESH& mesh,const double*& tab) const;

  bool testVolumes(const IntersectionMatrix& m,  MEDMEM::MESH& sMesh,  MEDMEM::MESH& tMesh) const;

  double sumVolume(const IntersectionMatrix& m) const;

  bool areCompatitable( const IntersectionMatrix& m1,  const IntersectionMatrix& m2) const;

  bool testSymmetric(const IntersectionMatrix& m1, const IntersectionMatrix& m2) const;

  bool testDiagonal(const IntersectionMatrix& m) const;
  
  void dumpIntersectionMatrix(const IntersectionMatrix& m) const;

  // 1.0e-5 here is due to limited precision of "correct" volumes calculated in Salome
  void intersectMeshes(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, const double correctVol, const double prec = 1.0e-5, bool doubleTest = true) const;

  void calcIntersectionMatrix(const char* mesh1path, const char* mesh1, const char* mesh2path, const char* mesh2, IntersectionMatrix& m) const;


};

#endif
