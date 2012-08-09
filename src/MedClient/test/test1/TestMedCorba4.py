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
        print "types (local)   : ", i, " : ", T1;
        print "types (distant) : ", i, " : ", T2;

        if (n2 !=  len(T2)):
            raise RuntimeError, \
                  "len(MESH::getTypes()) <> MESH::getNumberOfTypes()"

        for j in range(n2):
            if (T1[j] != eval(T2[j].__str__())):
                raise RuntimeError, "MESH::getTypes()"

            e1 = mLocal.getNumberOfElements(i, T1[j]);
            e2 = mDistant.getNumberOfElements(i, T2[j]);

            print "elements (local)   : ", T1[j], " : ", e1;
            print "elements (distant) : ", T2[j], " : ", e2;

            if (e1 != e2):
                raise RuntimeError, "MESH::getNumberOfElements"
        
print
print "All tests passed"
