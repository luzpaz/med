import os
BASE = os.environ["MED_ROOT_DIR"] + '/share/salome/resources/'

fileName = BASE + 'pointe.med'
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

# MED Mesh recieved from a distant component (via CORBA)

from omniORB import CORBA
from LifeCycleCORBA import *
from libSALOME_Swig import *

orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA(orb)

C = lcc.FindOrLoadComponent("FactoryServerPy", "Compo1Py")
C.Initialise(fileName)

mDistant = C.Calcul1()


print "Name              : ", mDistant.getName()


for i in [MED_CELL,
          MED_FACE,
          MED_EDGE,
          MED_NODE,
          MED_ALL_ENTITIES ]:

    n1 = mLocal.getNumberOfTypes(i);
    n2 = mDistant.getNumberOfTypes(i);

    if (n1 != n2):
        raise RuntimeError, "MESH::getNumberOfTypes()"

    if ((n1 > 0) & (i != MED_NODE)):
        T1 = mLocal.getTypes(i);
        T2 = mDistant.getTypes(i);
 
        if (n2 !=  len(T2)):
            raise RuntimeError, \
                  "len(MESH::getTypes()) <> MESH::getNumberOfTypes()"

        for j in range(n2):
            if (T1[j] != T2[j]):
                raise RuntimeError, "MESH::getTypes()"

            c1 = mLocal.getConnectivity(MED_FULL_INTERLACE, MED_NODAL, \
                                        i, T1[j]);
            c2 = mDistant.getConnectivity(MED_FULL_INTERLACE, MED_NODAL, \
                                        i, T2[j]);

            print "connectivity (local)   : ", T1[j], " : ", c1;
            print "connectivity (distant) : ", T2[j], " : ", c2;

            if (compare(c1, c2) == 0):
                raise RuntimeError, "MESH::getConnectivity()"
        
print
print "All tests passed"
