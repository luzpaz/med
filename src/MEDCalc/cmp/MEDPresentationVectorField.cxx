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

#include "MEDPresentationVectorField.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationVectorField::TYPE_NAME = "MEDPresentationVectorField";

MEDPresentationVectorField::MEDPresentationVectorField(const MEDCALC::VectorFieldParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationVectorField::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  setOrCreateRenderView();
  createSource();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  if (getIntProperty(MEDPresentation::PROP_NB_COMPONENTS) <= 1)
    {
      const char * msg = "Vector field presentation does not work for scalar  field!"; // this message will appear in GUI too
      STDLOG(msg);
      throw KERNEL::createSalomeException(msg);
    }

  std::ostringstream oss;
  oss << _objVar << " = pvs.Glyph(Input=" << _srcObjVar << ", GlyphType='Arrow');";
  pushAndExecPyLine(oss.str()); oss.str("");

  showObject(); // to be done first so that the scale factor computation properly works

  oss << _objVar << ".Scalars = ['"<< _fieldType << "', 'None'];";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".Vectors = ['"<< _fieldType << "', '" << _fieldName << "'];";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".ScaleMode = 'vector';";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".GlyphMode = 'All Points';";
  pushAndExecPyLine(oss.str()); oss.str("");
//  oss << _objVar << "GlyphTransform = 'Transform2';";  // not sure this is really needed?
//  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".ScaleFactor = 0.1;";
  pushAndExecPyLine(oss.str()); oss.str("");
  // Auto-tuning of the scale factor:
//  oss << "import medcalc; medcalc.SetDefaultScaleFactor(active=" << _objVar << ");";
//  pushAndExecPyLine(oss.str()); oss.str("");

  colorBy("POINTS");
  showScalarBar();
  rescaleTransferFunction();
  selectColorMap();
  resetCameraAndRender();
}

void
MEDPresentationVectorField::updatePipeline(const MEDCALC::VectorFieldParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    updateComponent<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationVectorField, MEDCALC::VectorFieldParameters>(params.colorMap);
}
