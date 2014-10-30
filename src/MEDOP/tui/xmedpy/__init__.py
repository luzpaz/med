# -*- coding: iso-8859-1 -*-
# Copyright (C) 2011-2014  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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
# Author : Guillaume Boulant (EDF)

#
# ===============================================================
# This functions are to be used to notify the USER of some events
# arising on the field operation. It is NOT to be used for logging
# purpose
# ===============================================================
#
def inf(msg):
    print "INF: "+str(msg)

def wrn(msg):
    print "WRN: "+str(msg)

def err(msg):
    print "ERR: "+str(msg)

def dbg(msg):
    print "DBG: "+str(msg)

#
# ===============================================================
# Initializing some CORBA stuff
# ===============================================================
#

# Remember SALOME definitions:
# ---------------------------
#
# componentName = Name of the component (a library lib<componentName>Engine
# should exist with a C function named <componentName>Engine_factory, and the
# component should be registered in the catalog MEDCatalog.xml).
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
factory = salome.lcc.FindOrLoadComponent(containerType,componentName)
# The factory is not the main CORBA component of the SALOME module MED
# (i.e. the engine associated to the active study), but the CORBA
# entry point for MED fields operations (i.e. a CORBA component
# reachable throught the LifeCycleCORBA). This entry point is used to
# get the other SALOME CORBA components required for MED field
# operations, in particular the dataManager and the calculator

#
# ===============================================================
# Initializing the MED components required for MED fields operations
# ===============================================================
#
dataManager = factory.getDataManager()
calculator  = factory.getCalculator()

#
# ===============================================================
# Initializing the notification system (events from components to GUI)
# ===============================================================
#

# The MEDEventListener is created in the GUI (WorkspaceController) and
# a reference is transmitted to the python context throw its IOR. The
# transmission consists in initializing the variable below from the
# GUI (who knows the IOR) when importing xmed in the python console
# (see WorkspaceController)
eventListener = None
import SALOME
def connectEventListener():
    global eventListener
    try:
        eventListenerIOR = dataManager.getEventListenerIOR()
        eventListener = salome.orb.string_to_object(eventListenerIOR)
    except SALOME.SALOME_Exception, e:
        wrn("The event listener is not running yet")
        msg ="When you'll have loaded the MED GUI, "
        msg+="call explicitely \"xmed.connectEventListener()\" "
        msg+="to connect the GUI event listener"
        inf(msg)
        eventListener = None
    except Exception, e:
        err("An unknown error occurs. Check if this ior=%s is valid."%eventListenerIOR)

def eventListenerIsRunning():
    global eventListener
    if eventListener is not None:
        return True

    # Try to define the event listener
    connectEventListener()
    if eventListener is None:
        # it definitly does not work
        wrn("the GUI is not loaded yet and will not be notified of the modification")
        return False

    return True

# One can try to connect to the eventListener, but it will fail if the
# MED GUI is not loaded. That does not matter, because the event
# listener is used only to notify the GUI of some event. If the GUI is
# not loaded, there is no use of notification.
connectEventListener()

#
# ===============================================================
# Automatic import of some elements of the package
# ===============================================================
#
from fieldtools import load, get, put, dup, ls, la, save, view, remove, clean
from fieldguide import doc
from cmdtools import cls, wipe

#
# ===============================================================
# Some helper functions to deal with the fields and meshes
# ===============================================================
#
import MEDCoupling
__mapTypeOfFieldLabel={
    MEDCoupling.ON_CELLS:"ON_CELLS",
    MEDCoupling.ON_NODES:"ON_NODES",
    MEDCoupling.ON_GAUSS_PT:"ON_GAUSS_PT",
    MEDCoupling.ON_GAUSS_NE:"ON_GAUSS_NE"}

def typeOfFieldLabel(typeOfField):
    # A field name could identify several MEDCoupling fields, that
    # differ by their spatial discretization on the mesh (values on
    # cells, values on nodes, ...). This spatial discretization is
    # specified (at least) by the TypeOfField that is an integer value
    # in this list:
    # 0 = ON_CELLS
    # 1 = ON_NODES
    # 2 = ON_GAUSS_PT
    # 3 = ON_GAUSS_NE
    try:
        return __mapTypeOfFieldLabel[typeOfField]
    except IndexError, e:
        return "UNCKNOWN"

#-----
# This function is to be called from the working python console to
# specify the globals() dictionnary (used in fieldtools for stat analysis)
#
# >>> xmed.setConsoleGlobals(globals())
#
import fieldtools
def setConsoleGlobals(pyConsoleGlobals):
    fieldtools.pyConsoleGlobals = pyConsoleGlobals

#
# ===================================================================
# unit test functions
# ===================================================================
#
def TEST_typeOfFieldLabel():
    print typeOfFieldLabel(0)
    print typeOfFieldLabel(5)

# ===================================================================
if __name__ == "__main__":
    TEST_typeOfFieldLabel()
