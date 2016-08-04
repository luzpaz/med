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
from medcalc.medevents import notifyGui_addPresentation, notifyGui_removePresentation, notifyGui_error, notifyGui_modifyPresentation

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

#

def MakeVectorField(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.VectorFieldParameters(proxy.id, displayedComponent, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makeVectorField(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the vector field:\n" + e.details.text)
    raise Exception(e.details.text)

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
def MakePointSprite(proxy,
                  viewMode=MEDCALC.VIEW_MODE_DEFAULT,
                  displayedComponent=MEDCALC.DISPLAY_DEFAULT,
                  scalarBarRange=MEDCALC.SCALAR_BAR_RANGE_DEFAULT,
                  colorMap=MEDCALC.COLOR_MAP_DEFAULT
                  ):
  # Create the presentation instance in CORBA engine
  # The engine in turn creates the ParaView pipeline elements
  params = MEDCALC.PointSpriteParameters(proxy.id, displayedComponent, scalarBarRange, colorMap)
  try:
    presentation_id = __manager.makePointSprite(params, viewMode)
    notifyGui_addPresentation(proxy.id, presentation_id)
    return presentation_id
  except SALOME.SALOME_Exception as e:
    notifyGui_error("An error occured while creating the point sprite:\n" + e.details.text)
    raise Exception(e.details.text)

def RemovePresentation(presentation_id):
  ok = __manager.removePresentation(presentation_id)
  if ok:
    notifyGui_removePresentation(presentation_id)
#

def __GetGENERICParameters(tag, presentation_id):
  exec "params = __manager.get%sParameters(presentation_id)" % tag
  return params

GetMeshViewParameters = lambda pres_id: __GetGENERICParameters("MeshView", pres_id)
GetScalarMapParameters = lambda pres_id: __GetGENERICParameters("ScalarMap", pres_id)
GetContourParameters = lambda pres_id: __GetGENERICParameters("Contour", pres_id)
GetSlicesParameters = lambda pres_id: __GetGENERICParameters("Slices", pres_id)
GetPointSpriteParameters = lambda pres_id: __GetGENERICParameters("PointSprite", pres_id)
GetVectorFieldParameters = lambda pres_id: __GetGENERICParameters("VectorField", pres_id)
#GetDeflectionShapeParameters = lambda pres_id: __GetGENERICParameters("DeflectionShape", pres_id)


def __UpdateGENERIC(tag, presentation_id, params):
  exec "__manager.update%s(presentation_id, params)" % tag
  notifyGui_modifyPresentation(presentation_id)

UpdateMeshView = lambda pres_id, params: __UpdateGENERIC("MeshView", pres_id, params)
UpdateScalarMap = lambda pres_id, params: __UpdateGENERIC("ScalarMap", pres_id, params)
UpdateContour = lambda pres_id, params: __UpdateGENERIC("Contour", pres_id, params)
UpdateSlices = lambda pres_id, params: __UpdateGENERIC("Slices", pres_id, params)
UpdateVectorField = lambda pres_id, params: __UpdateGENERIC("VectorField", pres_id, params)
UpdatePointSprite = lambda pres_id, params: __UpdateGENERIC("PointSprite", pres_id, params)
#UpdateDeflectionShape = lambda pres_id, params: __UpdateGENERIC("DeflectionShape", pres_id, params)

def ComputeCellAverageSize(obj):
  """
  @return the average cell size
  """
  bb, nCells = obj.GetDataInformation().GetBounds(), obj.GetDataInformation().GetNumberOfCells()
  bb = zip(bb[::2], bb[1::2])
  deltas = [x[1]-x[0] for x in bb]
  vol = reduce(lambda x,y:x*y, deltas, 1.0) 
  cellSize = (vol/nCells)**(1.0/3.0)  # necessarily 3D in ParaView
  return cellSize

def GetDomainCenter(obj):
  """
  @return the center of the domain as the central point of the bounding box
  """
  bb = obj.GetDataInformation().GetBounds()
  bb = zip(bb[::2], bb[1::2])
  mids = [x[0] + 0.5*(x[1]-x[0]) for x in bb]
  return mids

# Taken from paraview.simple from CEA ParaView's version:
def SetDefaultScaleFactor(active=None):
    """Provides a good display to the bounding box of the mesh and min / max of the field.
    This method available for filters: Warp By Vector, Glyph and Tensor Glyph"""
    if not active:
        active = GetActiveSource()
    if not active:
        return
    name = active.__class__.__name__
    if (name == 'WarpByVector'  or name == 'TensorGlyph') and hasattr(active, 'ScaleFactor'):
      import math
      datainfo = active.GetProperty("Input").SMProperty.GetProxy(0).GetDataInformation()
      if not datainfo:
          active.ScaleFactor = 1.0
          return
      nbcells = datainfo.GetNumberOfCells()
      nbpoints = datainfo.GetNumberOfPoints()
      nbelem = nbcells
      if nbelem == 0:
          nbelem = nbpoints
      abounds = datainfo.GetBounds()
      volume = 1
      vol = 0
      idim = 0
      i = 0
      eps = 1.0e-38
      while i < 6 :
          vol = math.fabs( abounds[i+1] - abounds[i] )
          if vol > 0:
            idim += 1
            volume *= vol
          i += 2
      if nbelem == 0 or math.fabs(idim) < eps:
          active.ScaleFactor = 0.0
          return
      volume /= nbelem
      scalefactor = pow( volume, 1.0 / idim )
      maximum = 1.0
      property = active.GetProperty('ScaleFactor')
      domain = None
      if property.GetDomain('vector_range').__class__.__name__ == 'vtkSMArrayRangeDomain':
          domain = property.GetDomain('vector_range')
      if property.GetDomain('tensor_range').__class__.__name__ == 'vtkSMArrayRangeDomain':
          domain = property.GetDomain('tensor_range')
      if not domain is None:
          if domain.GetMaximumExists(3):
            maximum = domain.GetMaximum(3)
      if math.fabs(maximum) > eps:
          scalefactor /= maximum
      active.ScaleFactor = scalefactor
      return
    if name == 'Glyph' and hasattr(active, 'ScaleFactor') and hasattr(active, 'UseCellsCenter') and hasattr(active, 'ScaleMode'):
      import math
      scaledextent = 1.0
      property = active.GetProperty('ScaleFactor')
      bounds_domain = property.GetDomain('bounds')
      if bounds_domain.__class__.__name__ == 'vtkSMBoundsDomain':
          if bounds_domain.GetMaximumExists(0):
            scaledextent = bounds_domain.GetMaximum(0)
      usecellscenter = active.GetProperty('UseCellsCenter').GetData()
      sdomain = "cells_"
      if usecellscenter == 0:
          sdomain = "points_"
      divisor = 1.0
      scalemode_domain = active.GetProperty('ScaleMode')
      scalemode = scalemode_domain.ConvertValue(scalemode_domain.GetData())
      if scalemode == 0:
          sdomain += "scalar_range"
          domain = property.GetDomain(sdomain)
          if domain.__class__.__name__ == 'vtkSMArrayRangeDomain':
            if domain.GetMaximumExists(0):
                divisor = domain.GetMaximum(0)
      if scalemode == 1 or scalemode == 2:
          sdomain += "vector_range"
          domain = property.GetDomain(sdomain)
          if domain.__class__.__name__ == 'vtkSMArrayRangeDomain':
            if domain.GetMaximumExists(3):
                divisor = domain.GetMaximum(3)
      divisor = math.fabs(divisor)
      if divisor < 0.000000001:
          divisor = 1
      scalefactor = scaledextent / divisor 
      active.ScaleFactor = scalefactor
      return

def GetSliceOrigins(obj, nbSlices, normal):
  """
  Compute all origin points for the position of the slices.
  @param normal is a list of 3 floats either 0 or 1 indicating the normal vector of the slices
  """
  from math import sqrt
  bb = obj.GetDataInformation().GetBounds()
  bb = zip(bb[::2], bb[1::2])
  origin = [x[0] + 0.5*(x[1]-x[0]) for x in bb]
  deltas = [x[1]-x[0] for x in bb]
  # Compute extent of slices:
  l = [normal[i]*deltas[i]**2 for i in range(3)]   # either the X extend, or the XY diagonal, or the XYZ diagonal
  plus = lambda x,y: x+y
  extent = sqrt(reduce(plus, l, 0.0))
  norm = sqrt(reduce(plus, normal, 0.0))
  normal = [normal[i]/norm for i in range(3)]
  origins = []
  step = extent/nbSlices
  for j in range(nbSlices):
    orig_j = [origin[i]+normal[i]*(-0.5*extent + step*(0.5+j)) for i in range(3)]
    origins.append(orig_j)
  return origins

