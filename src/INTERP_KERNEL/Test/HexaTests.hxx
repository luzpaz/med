#ifndef __HEXA_TESTS_HXX_
#define __HEXA_TESTS_HXX_

#include "Interpolation3DTestSuite.hxx"

namespace INTERP_UTILS
{
  class HexaTests : public Interpolation3DTestSuite
  {
    CPPUNIT_TEST_SUITE( HexaTests );

    CPPUNIT_TEST( simpleHexaBox );
    CPPUNIT_TEST( reflexiveHexaBox );
    CPPUNIT_TEST( hexaBoxes );
    CPPUNIT_TEST( hexaBoxesMoved );

    CPPUNIT_TEST_SUITE_END();

  public:
    void simpleHexaBox()
    {
      _testTools->intersectMeshes("meshes/BoxHexa1.med", "BoxHexa1", "meshes/BoxTetra2.med", "BoxTetra2", 65250, 1.0e-5);
    }

    void reflexiveHexaBox()
    {
      _testTools->intersectMeshes("meshes/BoxHexa1.med", "BoxHexa1", "meshes/BoxHexa1.med", "BoxHexa1", 204000);
    }

    void hexaBoxes()
    {
      _testTools->intersectMeshes("meshes/BoxHexa1.med", "BoxHexa1", "meshes/BoxHexa2.med", "BoxHexa2", 65250);
    }

    void hexaBoxesMoved()
    {
      _testTools->intersectMeshes("meshes/MovedHexaBox1.med", "MovedHexaBox1", "meshes/MovedHexaBox2.med", "MovedHexaBox2", 65250);
    }

  };
}
#endif
