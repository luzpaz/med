#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

# This file is a set of basic use case to test (from the python
# context) the functions developped in the MEDOP engine and the
# associated MEDOP CORBA interface (MEDDataManager and
# MEDCalaculator).
#
# (gboulant - 16/6/2011)
#

#
# ===============================================================
# Initializing some CORBA stuff
# ===============================================================
#

#
# componentName = Name of the component (a library libMEDOPEngine
# should exist with a C function named MEDOPEngine_factory, and the
# component MEDOP should be defined in the catalog).
#
# corbaModule = Name of the corba module that contains the IDL
# specifications of the component (name as defined in the idl file)
#
# containerType = Name of the container factory
#
componentName = "MEDOPFactory"
corbaModule   = "MEDOP"
containerType = "FactoryServer"

import salome
if salome.lcc is None:
    salome.salome_init()
__import__(corbaModule)
factory=salome.lcc.FindOrLoadComponent(containerType,componentName)
# This is not the main CORBA component of the SALOME module MED
# (i.e. the engine associated to the active study), but the CORBA
# entry point for MED fields operations (i.e. a CORBA component
# reachable throught the LifeCycleCORBA). This entry point is used to
# get the other SALOME CORBA components required for MED field
# operations, in particular the dataManager and the calculator

#
# ==================================================
# Simple use cases of the MEDDataManager
# ==================================================
#
def TEST_getDataManager():
    dataManager = factory.getDataManager()
    if "addDatasource" not in dir(dataManager):
        return False
    return True

#
# =============================================================
# Unit tests runner
# =============================================================
#
import unittest
from salome.kernel import pyunittester
class MyTestSuite(unittest.TestCase):
    def test_getDataManager(self):
        self.assertTrue(TEST_getDataManager())

def myunittests():
    pyunittester.run(MyTestSuite)

if __name__ == "__main__":
    myunittests()
