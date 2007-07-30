#ifndef __TU_INTERPOLATION_3D_HXX__
#define __TU_INTERPOLATION_3D_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "../Interpolation3D.hxx"

#define ERR_TOL 1.0e-8

using MEDMEM::Interpolation3D;

class Interpolation3DTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( Interpolation3DTest );
  CPPUNIT_TEST( reflexiveTetra );
  CPPUNIT_TEST( tetraTetraTransl );
  CPPUNIT_TEST( tetraTetraScale );
  //  CPPUNIT_TEST( box1 );
  // CPPUNIT_TEST( cyl1 );
  CPPUNIT_TEST( tetra1 );
  // CPPUNIT_TEST( tetra3 );
  
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

  void tearDown();

  // tests
  
  void reflexiveTetra();

  void tetraTetraTransl();

  void tetraTetraScale();

  void cyl1();

  void box1();

  void tetra1();

  void tetra3();
  
private:

  Interpolation3D* interpolator;

  double sumVolume(IntersectionMatrix m);
  
  bool isIntersectionConsistent(IntersectionMatrix m);
  
  void dumpIntersectionMatrix(IntersectionMatrix m);

};

#endif
