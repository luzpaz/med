import MEDCouplingCorbaSwigTest
from libMEDCouplingClient_Swig import *

import MEDCouplingCorbaServant_idl
import CORBA

orb=CORBA.ORB_init([''])
f=file(MEDCouplingCorbaSwigTest.FileIOR1,"r")
ior=f.read()
f=MEDCouplingCorbaSwigTest.testField()
fCorbaIOR=orb.string_to_object(ior)
print fCorbaIOR

for i in range(50):
    fCpy=MEDCouplingFieldDoubleClient.New(fCorbaIOR)
    print fCpy.isEqual(f,1e-12,1e-12)
    pass
fCorbaIOR.Destroy()

f=file(MEDCouplingCorbaSwigTest.FileIOR2,"r")
ior=f.read()
m=MEDCouplingCorbaSwigTest.testMesh()
mCorbaIOR=orb.string_to_object(ior)
for i in range(50):
    mCpy=MEDCouplingUMeshClient.New(mCorbaIOR)
    print mCpy.isEqual(m,1e-12)
    pass
mCorbaIOR.Destroy()

