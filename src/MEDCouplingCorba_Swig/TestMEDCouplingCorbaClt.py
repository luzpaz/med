#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest
import CORBA
import MEDCouplingCorbaServantTest_idl
import MEDCouplingCorbaSwigTest
from libMEDCouplingClient_Swig import *
import MEDCouplingCorbaServant_idl
import threading

class MEDCouplingCorbaServBasicsTestClt(unittest.TestCase):
    def testBaseCorbaFetching(self):
        self.assertTrue(not CORBA.is_nil(self._objC))
        pass

    def testContentOfFetched1DMesh(self):
        meshPtr=self._objC.get1DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        self.assertTrue(_mesh_from_distant.getSpaceDimension()==3);
        self.assertTrue(_mesh_from_distant.getMeshDimension()==1);
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build1DMesh()
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass

    def testCorbaFetching2D(self):
        meshPtr=self._objC.get2DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        self.assertTrue(_mesh_from_distant.getSpaceDimension()==2);
        self.assertTrue(_mesh_from_distant.getMeshDimension()==2);
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build2DMesh()
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched2DMesh(self):
        pass
    
    def testMultiFetchingToTestMemoryManagement(self):
        for i in xrange(1000):
            meshPtr=self._objC.get2DMesh();
            _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
            meshPtr.Destroy();
            test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
            meshRef=test.build2DMesh();
            self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
            pass
        pass
    
    def testCorbaFetching3D(self):
        meshPtr=self._objC.get3DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(3==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build3DMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched3DMesh(self):
        pass
    
    def testCorbaFetching3DSurf(self):
        meshPtr=self._objC.get3DSurfMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(2==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build3DSurfMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testContentOfFetched3DSurfMesh(self):
        pass
    
    def testCorbaFetching0D(self):
        meshPtr=self._objC.get0DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        self.assertTrue(3==_mesh_from_distant.getSpaceDimension());
        self.assertTrue(0==_mesh_from_distant.getMeshDimension());
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.build0DMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass
    
    def testCorbaFetchingM1D(self):
        meshPtr=self._objC.getM1DMesh();
        _mesh_from_distant=MEDCouplingUMeshClient.New(meshPtr);
        meshPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildM1DMesh();
        self.assertTrue(-1==_mesh_from_distant.getMeshDimension());
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12));
        pass

    def testCorbaFetchingExtruded(self):
        meshPtr=self._objC.getExtrudedMesh();
        _mesh_from_distant=MEDCouplingExtrudedMeshClient.New(meshPtr);
        meshPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildExtrudedMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass

    def testCorbaFetchingCMesh(self):
        meshPtr=self._objC.getCMesh();
        _mesh_from_distant=MEDCouplingCMeshClient.New(meshPtr);
        meshPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        meshRef=test.buildCMesh();
        self.assertTrue(_mesh_from_distant.isEqual(meshRef,1e-12))
        pass
    
    def testCorbaField2DNTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn2DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField2DNTMultiFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        fieldCpp1=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldCpp2=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        self.assertTrue(fieldCpp1.isEqual(fieldCpp2,1.e-12,1.e-15))
        pass

    def corbaField2DNTMFMTThread(self,i,fieldPtr,li):
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        li[i]=fieldCpp
        pass
    
    def testCorbaField2DNTMultiFetchingMT(self):
        li=8*[None]
        th=8*[None]
        fieldPtr=self._objC.getFieldScalarOn2DNT();
        for i in xrange(8):
            th[i]=threading.Thread(None,self.corbaField2DNTMFMTThread,"Test"+str(i),(i,fieldPtr,li))
            th[i].start()
            pass
        for i in xrange(8):
            th[i].join()
            pass
        for i in xrange(8-1):
            self.assertTrue(li[i].isEqual(li[i+1],1.e-12,1.e-15));
            pass
        fieldPtr.Destroy()
        pass
    
    def testCorbaFieldNode2DNTFetching(self):
        fieldPtr=self._objC.getFieldNodeScalarOn2DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldNodeScalarOn2DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DNTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DNT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DNT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DSurfWTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DSurfWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DSurfWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaField3DSurfCOTIFetching(self):
        fieldPtr=self._objC.getFieldScalarOn3DSurfCOTI();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn3DSurfCOTI();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaField2DLTFetching(self):
        fieldPtr=self._objC.getFieldScalarOn2DLT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldScalarOn2DLT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldGaussPt2DWTFetching(self):
        fieldPtr=self._objC.getFieldGaussPt2DWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldGaussPt2DWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testCorbaFieldGaussPtNE2DWTFetching(self):
        fieldPtr=self._objC.getFieldGaussPtNE2DWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldGaussPtNE2DWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldVectorOnExtrudedWT(self):
        fieldPtr=self._objC.getFieldVectorOnExtrudedWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldVectorOnExtrudedWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass

    def testCorbaFieldVectorOnCMeshWT(self):
        fieldPtr=self._objC.getFieldVectorOnCMeshWT();
        fieldCpp=MEDCouplingFieldDoubleClient.New(fieldPtr);
        fieldPtr.Destroy();
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        refField=test.buildFieldVectorOnCMeshWT();
        self.assertTrue(fieldCpp.isEqual(refField,1.e-12,1.e-15));
        pass
    
    def testShutdownServer(self):
        self._objC.shutdownOrb()
        pass
    
    def setUp(self):
        self._orb=CORBA.ORB_init([''])      
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        f=file(test.buildFileNameForIOR(),"r")
        ior=f.read()
        self._objC=self._orb.string_to_object(ior);
        pass
    pass

unittest.main()
