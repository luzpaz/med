import MEDCouplingCorbaSwigTest
from libMEDCouplingCorba_Swig import *

## for i in range(50):
##     f=testField()
##     fCorbaIOR=MEDCouplingFieldDoubleServant._this(f)
##     print fCorbaIOR
##     fCorbaIOR.Destroy()
##     pass

## for i in range(50):
##     mesh=testMesh()
##     fCorbaIOR=MEDCouplingUMeshServant._this(mesh)
##     print fCorbaIOR
##     fCorbaIOR.Destroy()
##     pass

f=MEDCouplingCorbaSwigTest.testField()
fCorbaIOR=MEDCouplingFieldDoubleServant._this(f)
import CORBA
orb=CORBA.ORB_init([''])
s=orb.object_to_string(fCorbaIOR)
fi=file(MEDCouplingCorbaSwigTest.FileIOR1,"w")
fi.write(s)
fi.close()
mesh=MEDCouplingCorbaSwigTest.testMesh()
meshCorba=MEDCouplingUMeshServant._this(mesh)
s=orb.object_to_string(meshCorba)
fi=file(MEDCouplingCorbaSwigTest.FileIOR2,"w")
fi.write(s)
fi.close()
orb.run()
