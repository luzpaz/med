import os

# import salome

BASE = os.environ["MED_ROOT_DIR"] + '/share/salome/resources/'

fileName = BASE + 'pointe.med'
fileName = BASE + 'test_hydro_darcy1a_out.med'
fileName = BASE + 'carre_en_quad4_seg2.med'


from omniORB import CORBA
from LifeCycleCORBA import *
from libSALOME_Swig import *
from libMEDClient import *

orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA(orb)

C = lcc.FindOrLoadComponent("FactoryServerPy", "Compo1Py")

print "ok"
C.Initialise(fileName)

mDistant = C.Calcul1()
print mDistant
mDistantCopy = MESHClient(mDistant)
print mDistantCopy

print "###################################"
for i in xrange(10):
    print mDistantCopy.getName()
    
for i in xrange(1):
    print "-"
    

print
print "All tests passed"
