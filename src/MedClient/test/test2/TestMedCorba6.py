#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import os

## ne fonctionne pas ?
## import salome

BASE = os.environ["MED_ROOT_DIR"]
BASE = os.path.join( BASE, 'share', 'salome', 'resources', 'med' )

fileName = os.path.join( BASE, 'pointe.med' )
fileName = os.path.join( BASE, 'carre_en_quad4_seg2.med' )
 

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

print "Local       ", C2.Calcul(mLocal)
print "DistantCopy ", C2.Calcul(mDistantCopy)

testDistant = 0;
try:
    print "Distant     ", C2.Calcul(mDistant)
except:
    print "an exception has been received, it's the normal behaviour"
    testDistant = 1

if (testDistant == 0):
    raise RuntimeError, "no exception has been received, test failed"


print
print "All tests passed"
