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

# This script can be used to test the event listener. Note that the
# event listener is a CORBA object created inside the XMED GUI and
# that it could be reached if and only if the XMED GUI is loaded
# first.
#
# Then two tests are defined:
# - First you can try to import and use this script in the python
#   console while the XMED GUI is NOT loaded. Then you will check that
#   the script detects that the event listener is not available.
# - Second you can try to import this script AFTER having load the
#   XMED GUI. Then the test will be completed.
#

import xmed
xmed.connectEventListener()

if xmed.eventListener is None:
    raise RuntimeError("The test must stop because the event listener is not defined")

import MEDOP

# We can first try to retrieve the IOR using the data manager (just
# for internal test because it's not required in the use cases)
print xmed.dataManager.getEventListenerIOR()
print xmed.dataManager.getEventListenerIOR()
print xmed.dataManager.getEventListenerIOR()
# Make it several time to check the memory releases.

# We can then test the standard use case of the event listener
fieldHandler = MEDOP.FieldHandler(id = 0,
                                  fieldname = "testfield",
                                  meshname  = "testmesh",
                                  type      = 0,
                                  iteration = 0,
                                  order     = 0,
                                  source    = "test//:test_medEventListener.py")

medEvent = MEDOP.MedEvent(type    = MEDOP.EVENT_UPDATE_FIELD,
                          fieldid = fieldHandler.id)

print fieldHandler
print medEvent

xmed.eventListener.processMedEvent(medEvent)
print "Test completed"
