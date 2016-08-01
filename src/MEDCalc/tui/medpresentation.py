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
import MEDCALC, SALOME
from medcalc.medevents import notifyGui_addPresentation, notifyGui_removePresentation, notifyGui_error

__manager = medcalc.medcorba.factory.getPresentationManager()

def MakeMeshView(proxy,
                 viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                 meshMode=MEDCALC.MESH_MODE_DEFAULT):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.MeshViewParameters(proxy.id, meshMode)
  try:
    presentation_id = __manager.makeMeshView(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the mesh view:\n" + e.details.text)
    raise Exception(e.details.text)


def MakeScalarMap(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.ScalarMapParameters(proxy.id, displayedComponent, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makeScalarMap(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the scalar map:\n" + e.details.text)
    raise Exception(e.details.text)

def MakeContour(proxy,
                viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                nbContours=MEDCALC.NB_CONTOURS_DEFAULT
                ):
  params = MEDCALC.ContourParameters(proxy.id, scalarBarRange, colorMap, nbContours)
  try:
    presentation_id = __manager.makeContour(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the contour:\n" + e.details.text)
    raise Exception(e.details.text)

##
#
#def MakeVectorField(proxy,
#                    viewMode=MEDCALC.VIEW_MODE_DEFAULT
#                    ):
#  params = MEDCALC.VectorFieldParameters(proxy.id, viewMode)
#  presentation_id = __manager.makeVectorField(params)
#  notifyGui_addPresentation(proxy.id, presentation_id)
#  return presentation_id
##
#
def MakeSlices(proxy,
                viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                colorMap=MEDCALC.COLOR_MAP_DEFAULT,
                sliceOrientation=MEDCALC.SLICE_ORIENTATION_DEFAULT,
                nbSlices=MEDCALC.NB_SLICES_DEFAULT):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.SlicesParameters(proxy.id, displayedComponent,scalarBarRange, colorMap, 
                                    sliceOrientation, nbSlices)
  try:
    presentation_id = __manager.makeSlices(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the slices:\n" + e.details.text)
    raise Exception(e.details.text)
##
#
#def MakeDeflectionShape(proxy,
#                        viewMode=MEDCALC.VIEW_MODE_DEFAULT
#                        ):
#  params = MEDCALC.DeflectionShapeParameters(proxy.id, viewMode)
#  presentation_id = __manager.makeDeflectionShape(params)
#  notifyGui_addPresentation(proxy.id, presentation_id)
#  return presentation_id
##
#
#def MakePointSprite(proxy,
#                    viewMode=MEDCALC.VIEW_MODE_DEFAULT,
#                    displayedComponent=MEDCALC.DISPLAY_DEFAULT,
#                    scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
#                    colorMap=MEDCALC.COLOR_MAP_DEFAULT,
#                    ):
#  params = MEDCALC.PointSpriteParameters(proxy.id, viewMode, displayedComponent, scalarBarRange, colorMap)
#  presentation_id = __manager.makePointSprite(params)
#  notifyGui_addPresentation(proxy.id, presentation_id)
#  return presentation_id
#

def RemovePresentation(presentation_id):
  ok = __manager.removePresentation(presentation_id)
  if ok:
    notifyGui_removePresentation(presentation_id)
#

def GetGENERICParameters(tag, presentation_id):
  exec "params = __manager.get%sParameters(presentation_id)" % tag
  return params

GetMeshViewParameters = lambda pres_id: GetGENERICParameters("MeshView", pres_id)
GetScalarMapParameters = lambda pres_id: GetGENERICParameters("ScalarMap", pres_id)
GetContourParameters = lambda pres_id: GetGENERICParameters("Contour", pres_id)
GetSlicesParameters = lambda pres_id: GetGENERICParameters("Slices", pres_id)


def UpdateGENERIC(tag, presentation_id, params):
  exec "__manager.update%s(presentation_id, params)" % tag

UpdateMeshView = lambda pres_id, params: UpdateGENERIC("MeshView", pres_id, params)
UpdateScalarMap = lambda pres_id, params: UpdateGENERIC("ScalarMap", pres_id, params)
UpdateContour = lambda pres_id, params: UpdateGENERIC("Contour", pres_id, params)
UpdateSlices = lambda pres_id, params: UpdateGENERIC("Slices", pres_id, params)
