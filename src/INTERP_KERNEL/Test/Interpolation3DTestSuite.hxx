#ifndef __TU_INTERPOLATION_3D_TEST_SUITE_HXX__
#define __TU_INTERPOLATION_3D_TEST_SUITE_HXX__

#include "MeshTestToolkit.hxx"

#include <cppunit/extensions/HelperMacros.h>

namespace INTERP_TEST
{

  /**
   * \brief Base class for mesh intersection test suites.
   * 
   */
  class Interpolation3DTestSuite : public CppUnit::TestFixture
  {

  public:
    /**
     * Sets up the test suite.
     * Creates the MeshTestToolkit object used by the tests.
     *
     */
    void setUp()
    {
      _testTools = new MeshTestToolkit();
    }

    /**
     * Cleans up after the test suite.
     * Liberates the MeshTestToolkit object used by the tests.
     */
    void tearDown()
    {
      delete _testTools;
    }

    

  protected:
    /// MeshTestToolkit object to which the tests are delegated
    MeshTestToolkit* _testTools; 

  };
}
#endif
