import os

BASE = os.environ["MED_ROOT_DIR"] + '/share/salome/resources/'

fileName = BASE + 'pointe.med'
fileName = BASE + 'test_hydro_darcy1a_out.med'
fileName = BASE + 'carre_en_quad4_seg2.med'

def compare(x, y):
    if (len(x) != len(y)):
        return 0
    for i in xrange(len(x)):
        if x[i] != y[i]:
            return 0
    return 1

# MED Mesh read from a (local) file

from libMEDMEM_Swig import *

md = MED()

mdDriver = MED_MED_RDONLY_DRIVER(fileName, md)

mdDriver.open()
mdDriver.readFileStruct()
mdDriver.close()

mLocal = md.getMesh(md.getMeshName(0))
mLocal.read()

# MED Mesh received from a distant component (via CORBA)

from omniORB import CORBA
from LifeCycleCORBA import *
from libSALOME_Swig import *
from libMEDClient import *

orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA(orb)

C = lcc.FindOrLoadComponent("FactoryServerPy", "Compo1Py")
C.Initialise(fileName)

mDistant = C.Calcul1()
mDistantCopy = MESHClient(mDistant)

from libCompo2 import *
C2 = Compo2()

for i in xrange(10):
    print "-"

print "Local       "
print C2.Calcul(mLocal)

for i in xrange(10):
    print "-"
    
print "DistantCopy "
s = C2.Calcul(mDistantCopy)
print 

##print "ok ici"
##for i in xrange(10):
##    print "-"

##try:
##    sDistant = C.Calcul2()
##except:
##    print "erreur"

##sDistantCopy = SUPPORTClient(sDistant)
##print sDistant
print
print "All tests passed"
