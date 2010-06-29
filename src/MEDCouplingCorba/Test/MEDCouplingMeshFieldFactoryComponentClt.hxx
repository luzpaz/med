//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTCLT_HXX__
#define __MEDCOUPLINGCORBAFIELDFACTORYCOMPONENTCLT_HXX__

#include "SALOMEconfig.h"

#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServantTest)

#include <cppunit/extensions/HelperMacros.h>

namespace ParaMEDMEM
{
  class MEDCouplingUMesh;
}

namespace SALOME_TEST
{
  class MEDCouplingCorbaServBasicsTestClt : public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE(MEDCouplingCorbaServBasicsTestClt);
    CPPUNIT_TEST( checkBaseCorbaFetching );
    CPPUNIT_TEST( checkCorbaFetching2D );
    CPPUNIT_TEST( checkContentOfFetched2DMesh );
    CPPUNIT_TEST( checkMultiFetchingToTestMemoryManagement );
    CPPUNIT_TEST( checkCorbaFetching3D );
    CPPUNIT_TEST( checkContentOfFetched3DMesh );
    CPPUNIT_TEST( checkCorbaFetching3DSurf );
    CPPUNIT_TEST( checkContentOfFetched3DSurfMesh );
    CPPUNIT_TEST( checkCorbaFetching0D );
    CPPUNIT_TEST( checkCorbaFetchingM1D );
    CPPUNIT_TEST( checkCorbaField2DNTFetching );
    CPPUNIT_TEST( checkCorbaField2DNTMultiFetching );
    CPPUNIT_TEST( checkCorbaField2DNTMultiFetchingMT );
    CPPUNIT_TEST( checkCorbaFieldNode2DNTFetching );
    CPPUNIT_TEST( checkCorbaField3DNTFetching );
    CPPUNIT_TEST( checkCorbaField3DSurfWTFetching );
    CPPUNIT_TEST( checkCorbaField3DSurfCOTIFetching );
    CPPUNIT_TEST( checkCorbaField2DLTFetching );
    CPPUNIT_TEST( shutdownServer );
    CPPUNIT_TEST_SUITE_END();
  public:
    void checkBaseCorbaFetching();
    void checkCorbaFetching2D();
    void checkContentOfFetched2DMesh();
    void checkMultiFetchingToTestMemoryManagement();
    void checkCorbaFetching3D();
    void checkContentOfFetched3DMesh();
    void checkCorbaFetching3DSurf();
    void checkContentOfFetched3DSurfMesh();
    void checkCorbaFetching0D();
    void checkCorbaFetchingM1D();
    void checkCorbaField2DNTFetching();
    void checkCorbaField2DNTMultiFetching();
    void checkCorbaField2DNTMultiFetchingMT();
    void checkCorbaFieldNode2DNTFetching();
    void checkCorbaField3DNTFetching();
    void checkCorbaField3DSurfWTFetching();
    void checkCorbaField3DSurfCOTIFetching();
    void checkCorbaField2DLTFetching();
    void shutdownServer();
  private:
    static void *checkCorbaField2DNTMultiFetchingMTStatic(void *stack);
  private:
    static SALOME_TEST::MEDCouplingMeshFieldFactory_ptr _objC;
    static ParaMEDMEM::MEDCouplingUMesh *_mesh_from_distant;
  public:
    static int _argc;
    static char **_argv;
    static CORBA::ORB_var _orb;
  };
}

#endif
