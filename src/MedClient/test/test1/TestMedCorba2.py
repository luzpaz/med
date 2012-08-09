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
BASE = os.environ["MED_ROOT_DIR"]
BASE = os.path.join( BASE, 'share', 'salome', 'resources', 'med' )

fileName = os.path.join( BASE, 'pointe.med' )
fileName = os.path.join( BASE, 'carre_en_quad4_seg2.med' )

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

# Compare local and distant copies

def ecart(x, y):
    s = 0.
    if (len(x) != len(y)):
        return 1.
    
    for i in xrange(len(x)):
        s = s + abs(x[i] - y[i]);
    return s

def compare(x, y):
    if (len(x) != len(y)):
        return 0
    for i in xrange(len(x)):
        if x[i] != y[i]:
            return 0
    return 1

def transpose(x, n, m):
    y = range(len(x));
    for i in xrange(n):
        for j in xrange(m):
            y[i + j*n] = x[i*m + j];
    return y

m = mDistant.getSpaceDimension();
print "SpaceDimension    : ", m
n = mDistant.getNumberOfNodes();
print "NumberOfNodes     : ", n

x1 = mDistant.getCoordinates(MED_FULL_INTERLACE);
x2 = mDistant.getCoordinates(MED_NO_INTERLACE);
x3 = mDistant.getCoordinates(MED_FULL_INTERLACE);

print "Coordinates (MED_FULL_INTERLACE) : ", x1
print "Coordinates (MED_NO_INTERLACE) :   ", x2

if (ecart(x1, x3) > 1e-7):
    raise RuntimeError, "getCoordinates : INTERLACE"

x4 = transpose(x1, n, m);
print "Coordinates (MED_NO_INTERLACE) :   ", x4

if (ecart(x2, x4) > 1e-7):
    raise RuntimeError, "getCoordinates : NO_INTERLACE"

print
print "All tests passed"
