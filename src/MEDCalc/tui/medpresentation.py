# Copyright (C) 2011-2016  CEA/DEN, EDF R&D
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
from medcalc.medevents import notifyGui_addPresentation, notifyGui_removePresentation

__manager = medcalc.medcorba.factory.getPresentationManager()

def MakeScalarMap(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedInfo=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.ScalarBarRange(MEDCALC.SCALAR_BAR_RANGE_VAL_MIN_DEFAULT,
                                                        MEDCALC.SCALAR_BAR_RANGE_VAL_MAX_DEFAULT),
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.ScalarMapParameters(proxy.id, viewMode, displayedInfo, scalarBarRange, colorMap)
  presentation_id = __manager.makeScalarMap(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def MakeContour(proxy,
                viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                displayedInfo=MEDCALC.DISPLAY_DEFAULT,
                scalarBarRange=MEDCALC.ScalarBarRange(MEDCALC.SCALAR_BAR_RANGE_VAL_MIN_DEFAULT,
                                                      MEDCALC.SCALAR_BAR_RANGE_VAL_MAX_DEFAULT),
                colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                nbContours=MEDCALC.NB_CONTOURS_DEFAULT
                ):
  params = MEDCALC.ContourParameters(proxy.id, viewMode, displayedInfo, scalarBarRange, colorMap, nbContours)
  presentation_id = __manager.makeContour(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def MakeVectorField(proxy,
                    viewMode=MEDCALC.VIEW_MODE_DEFAULT
                    ):
  params = MEDCALC.VectorFieldParameters(proxy.id, viewMode)
  presentation_id = __manager.makeVectorField(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def MakeSlices(proxy,
               viewMode=MEDCALC.VIEW_MODE_DEFAULT,
               orientation=MEDCALC.SLICE_ORIENTATION_DEFAULT,
               nbSlices=MEDCALC.NB_SLICES_DEFAULT
               ):
  params = MEDCALC.SlicesParameters(proxy.id, viewMode, orientation, nbSlices)
  presentation_id = __manager.makeSlices(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def MakeDeflectionShape(proxy,
                        viewMode=MEDCALC.VIEW_MODE_DEFAULT
                        ):
  params = MEDCALC.DeflectionShapeParameters(proxy.id, viewMode)
  presentation_id = __manager.makeDeflectionShape(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def MakePointSprite(proxy,
                    viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                    displayedInfo=MEDCALC.DISPLAY_DEFAULT,
                    scalarBarRange=MEDCALC.ScalarBarRange(MEDCALC.SCALAR_BAR_RANGE_VAL_MIN_DEFAULT,
                                                          MEDCALC.SCALAR_BAR_RANGE_VAL_MAX_DEFAULT),
                    colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                    ):
  params = MEDCALC.PointSpriteParameters(proxy.id, viewMode, displayedInfo, scalarBarRange, colorMap)
  presentation_id = __manager.makePointSprite(params)
  notifyGui_addPresentation(proxy.id, presentation_id)
  return presentation_id
#

def RemovePresentation(presentation_id):
  ok = __manager.removePresentation(presentation_id)
  if ok:
    notifyGui_removePresentation(presentation_id)
#
