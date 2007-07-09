#ifndef __TU_TRANSFORMED_TRIANGLE_INTERSECT_HXX__
#define __TU_TRANSFORMED_TRIANGLE_INTERSECT_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include "../TransformedTriangle.hxx"

#define ERR_TOL 1.0e-8

using INTERP_UTILS::TransformedTriangle;

class TransformedTriangleIntersectTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( TransformedTriangleIntersectTest );
  CPPUNIT_TEST( testTriangle1 );
  CPPUNIT_TEST( testTriangle2 );
  CPPUNIT_TEST_SUITE_END();

  typedef INTERP_UTILS::TransformedTriangle::TriSegment TriSegment;
  typedef INTERP_UTILS::TransformedTriangle::DoubleProduct DoubleProduct;

public:
  void setUp();

  void tearDown();

  void testTriangle1();
  
  void testTriangle2();

private:
  TransformedTriangle* tri1;
  TransformedTriangle* tri2;

};








#endif
