# Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
#           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either 
# version 2.1 of the License.
# 
# This library is distributed in the hope that it will be useful 
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

# import salome

BASE = os.environ["MED_ROOT_DIR"]
BASE = os.path.join( BASE, 'share', 'salome', 'resources', 'med' )

fileName = os.path.join( BASE, 'pointe.med' )
fileName = os.path.join( BASE, 'test_hydro_darcy1a_out.med' )
fileName = os.path.join( BASE, 'carre_en_quad4_seg2.med' )


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
