#ifndef __TU_INTERPOLATION_3D_TEST_SUITE_HXX__
#define __TU_INTERPOLATION_3D_TEST_SUITE_HXX__

#include "MeshTestToolkit.hxx"

#include <cppunit/extensions/HelperMacros.h>

namespace INTERP_UTILS
{

  class Interpolation3DTestSuite : public CppUnit::TestFixture
  {

  public:
    void setUp()
    {
      _testTools = new MeshTestToolkit();
    }

    void tearDown()
    {
      delete _testTools;
    }

    

  protected:

    MeshTestToolkit* _testTools; 

  };
}
#endif
