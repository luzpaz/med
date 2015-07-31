# Copyright (C) 2011-2015  CEA/DEN, EDF R&D
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

import medcalc
import MEDCALC

__manager = medcalc.medcorba.factory.getPresentationManager()

def MakeScalarMap(proxy, viewMode=MEDCALC.VIEW_MODE_REPLACE):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements

  print "In MakeScalarMap (Python)"

  print "viewMode:", viewMode, " [", type(viewMode), "]"

  params = MEDCALC.ScalarMapParameters(proxy.id, viewMode)
  __manager.makeScalarMap(params)

#

def MakeIsoSurface():
  print "Not implemented yet"
#

def MakeVectorField():
  print "Not implemented yet"
#

def MakeSlices():
  print "Not implemented yet"
#

def MakeDeflectionShape():
  print "Not implemented yet"
#

def MakePointSprite():
  print "Not implemented yet"
#
