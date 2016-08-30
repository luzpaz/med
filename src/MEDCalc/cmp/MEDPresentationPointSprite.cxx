// Copyright (C) 2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDPyLockWrapper.hxx"

#include "MEDPresentationPointSprite.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationPointSprite::TYPE_NAME = "MEDPresentationPointSprite";

MEDPresentationPointSprite::MEDPresentationPointSprite(const MEDCALC::PointSpriteParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationPointSprite::initFieldMeshInfos()
{
  MEDPresentation::initFieldMeshInfos();
  _colorByType = "POINTS";
}

void
MEDPresentationPointSprite::scaleBallRadius()
{
  std::ostringstream oss;
  oss << _dispVar << ".RadiusVectorComponent = " << _selectedComponentIndex << ";";
  pushAndExecPyLine(oss.str()); oss.str("");
}


void
MEDPresentationPointSprite::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  setOrCreateRenderView(); // instanciate __viewXXX
  createSource();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  int nbCompo = getIntProperty(MEDPresentation::PROP_NB_COMPONENTS);

  // Point sprite needs point data:
  applyCellToPointIfNeeded();

  pushAndExecPyLine(_objVar + " = " + _srcObjVar);

  showObject(); // needs to be done before cell size computation to be sure ParaView has computed bounds, etc ...

  // Compute cell average size in the mesh to have a nice scaling ratio:
  std::ostringstream oss;
  oss << "__avgSize = medcalc.ComputeCellAverageSize(" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");

  colorBy();    // see initFieldInfo() - necessarily POINTS like in Contour

  // Set point sprite:
  oss << _dispVar << ".SetRepresentationType('Point Sprite');";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".PointSpriteDefaultsInitialized = 1;";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".RadiusArray = ['POINTS', '" << _fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".RadiusMode = 'Scalar';";
  pushAndExecPyLine(oss.str()); oss.str("");

  scaleBallRadius();
  oss << _dispVar << ".RadiusIsProportional = 0 ;";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".RadiusTransferFunctionEnabled = 1 ;";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".MaxPixelSize = 30 ;";   // Avoid too big balls
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar << ".RadiusRange = [0.0, __avgSize];";
  pushAndExecPyLine(oss.str()); oss.str("");

  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
}

void
MEDPresentationPointSprite::updatePipeline(const MEDCALC::PointSpriteParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    {
      updateComponent<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(std::string(params.displayedComponent));
      scaleBallRadius();
      pushAndExecPyLine("pvs.Render();");
    }
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationPointSprite, MEDCALC::PointSpriteParameters>(params.colorMap);
}
