import CORBA
import MEDCouplingCorbaSwigTest
import MEDCouplingCorbaSwigTestServ

orb=CORBA.ORB_init(['']);
poa=orb.resolve_initial_references("RootPOA");
mgr=poa._get_the_POAManager();
mgr.activate();
compo=MEDCouplingCorbaSwigTestServ.MEDCouplingMeshFieldFactoryComponentPy(orb)
compoPtr=compo._this()
ior=orb.object_to_string(compoPtr)
test=MEDCouplingCorbaSwigTest.MEDCouplingCorbaServBasicsTest()
f=file(test.buildFileNameForIOR(),"w")
f.write(ior)
f.close()
orb.run()

