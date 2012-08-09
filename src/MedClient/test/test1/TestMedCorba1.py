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
fileName = os.path.join( BASE, 'test_hydro_darcy1a_out.med' )

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
#CCRTfrom libSALOME_Swig import *

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

print "Name              : ", mDistant.getName()
if (mLocal.getName() != mDistant.getName()):
    raise RuntimeError, "MESH::getName()"

print "SpaceDimension    : ", mDistant.getSpaceDimension()
if (mLocal.getSpaceDimension() != mDistant.getSpaceDimension()):
    raise RuntimeError, "MESH::getSpaceDimension()"

print "MeshDimension     : ", mDistant.getMeshDimension()
if (mLocal.getMeshDimension() != mDistant.getMeshDimension()):
    raise RuntimeError, "MESH::getMeshDimension()"

print "CoordinatesSystem : ", mDistant.getCoordinatesSystem()
if (mLocal.getCoordinatesSystem() != mDistant.getCoordinatesSystem()):
    raise RuntimeError, "MESH::getCoordinatesSystem()"

print "NumberOfNodes     : ", mDistant.getNumberOfNodes()
if (mLocal.getNumberOfNodes() != mDistant.getNumberOfNodes()):
    raise RuntimeError, "MESH::getNumberOfNodes()"


print
print "All tests passed"
