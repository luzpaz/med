import MEDCouplingCorbaSwigTest
import MEDCouplingCorbaServantTest_idl

from libMEDCouplingCorba_Swig import *

class MEDCouplingMeshFieldFactoryComponentPy(MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory):
    def __init__(self,orb):
        self._orb=orb
        pass
    
    def destroyFactory(self):
        oid = self._default_POA().servant_to_id(self);
        MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory._default_POA().deactivate_object(self,oid);
        MEDCouplingCorbaServantTest_idl._0_SALOME_TEST__POA.MEDCouplingMeshFieldFactory._remove_ref(self);
        
    def shutdownOrb(self):
        self._orb.shutdown(0)
    
    def get2DMesh(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        mesh=test.build2DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get3DMesh(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        mesh=test.build3DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get3DSurfMesh(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        mesh=test.build3DSurfMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def get0DMesh(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        mesh=test.build0DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def getM1DMesh(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        mesh=test.buildM1DMesh()
        return MEDCouplingUMeshServant._this(mesh)
    
    def getFieldScalarOn2DNT(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        field=test.buildFieldScalarOn2DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldNodeScalarOn2DNT(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        field=test.buildFieldNodeScalarOn2DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DNT(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        field=test.buildFieldScalarOn3DNT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DSurfWT(self):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        field=test.buildFieldScalarOn3DSurfWT()
        return MEDCouplingFieldDoubleServant._this(field)
    
    def getFieldScalarOn3DSurfCOTI(selfx):
        test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
        field=test.buildFieldScalarOn3DSurfCOTI()
        return MEDCouplingFieldDoubleServant._this(field)
    pass


## f=MEDCouplingCorbaSwigTest.testField()
## fCorbaIOR=MEDCouplingFieldDoubleServant._this(f)
## import CORBA
## orb=CORBA.ORB_init([''])
## s=orb.object_to_string(fCorbaIOR)
## fi=file(MEDCouplingCorbaSwigTest.FileIOR1,"w")
## fi.write(s)
## fi.close()
## mesh=MEDCouplingCorbaSwigTest.testMesh()
## meshCorba=MEDCouplingUMeshServant._this(mesh)
## s=orb.object_to_string(meshCorba)
## fi=file(MEDCouplingCorbaSwigTest.FileIOR2,"w")
## fi.write(s)
## fi.close()
## orb.run()
