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

#include "MEDPresentationSlices.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationSlices::TYPE_NAME = "MEDPresentationSlices";
const std::string MEDPresentationSlices::PROP_NB_SLICES = "nbSlices";
const std::string MEDPresentationSlices::PROP_SLICE_ORIENTATION = "slicesOrientation";

MEDPresentationSlices::MEDPresentationSlices(const MEDCALC::SlicesParameters& params,
                                               const MEDCALC::ViewModeType viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationSlices::PROP_NB_SLICES, params.nbSlices);
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, params.orientation);
}

void
MEDPresentationSlices::generateSlices()
{
  std::ostringstream oss;
  int nbSlices = getIntProperty(MEDPresentationSlices::PROP_NB_SLICES);
  std::string normal = getNormalVector();

  oss << "__origins = medcalc.GetSliceOrigins(" << _srcObjVar << ", " << nbSlices << ", " << normal << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine("__objLst = [];");
  oss << "for sliceNum in range(" << nbSlices << "):\n";
  oss << "  obj = pvs.Slice(Input=" << _srcObjVar << ")\n";
  oss << "  obj.SliceType = 'Plane'\n";
  oss << "  obj.SliceType.Normal = " << normal << "\n";
  oss << "  obj.SliceType.Origin = __origins[sliceNum]\n";
  oss << "  __objLst.append(obj)\n\n";
  pushAndExecPyLine(oss.str()); oss.str("");

  oss << _objVar << " = pvs.GroupDatasets(Input=__objLst);";
  pushAndExecPyLine(oss.str()); oss.str("");
}

void
MEDPresentationSlices::generateAndDisplay()
{
  generateSlices();
  showObject();

  colorBy(_pvFieldType);
  showScalarBar();
  selectColorMap();
  rescaleTransferFunction();
  resetCameraAndRender();
}


void
MEDPresentationSlices::clearPreviousSlices()
{
  std::ostringstream oss;

  pushAndExecPyLine("for sliceNum, _ in enumerate(__objLst):\n  pvs.Delete(__objLst[sliceNum]);");
  oss <<            "pvs.Delete(" << _objVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
}

std::string
MEDPresentationSlices::getNormalVector() const
{
  switch(_params.orientation)
  {
    case MEDCALC::SLICE_NORMAL_TO_X:
      return "[1.0, 0.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_Y:
      return "[0.0, 1.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_Z:
      return "[0.0, 0.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_XY:
      return "[1.0, 1.0, 0.0]";
    case MEDCALC::SLICE_NORMAL_TO_XZ:
      return "[1.0, 0.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_YZ:
      return "[0.0, 1.0, 1.0]";
    case MEDCALC::SLICE_NORMAL_TO_XYZ:
      return "[1.0, 1.0, 1.0]";
    default:
      const char * mes = "Unexpected getSliceOrientationCommand() error!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }
  return ""; // never happens
}

void
MEDPresentationSlices::selectSliceOrientation(const std::string & obj)
{
  std::ostringstream oss1;
  oss1 << obj << ".SliceType.Normal = " << getNormalVector() << ";";
  pushAndExecPyLine(oss1.str()); oss1.str("");
}

void
MEDPresentationSlices::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss_o, oss;

  createSource();

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  setOrCreateRenderView(); // instanciate __viewXXX

  // Now create the initial number of slices
  generateAndDisplay();
}

void
MEDPresentationSlices::updatePipeline(const MEDCALC::SlicesParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    updateComponent<MEDPresentationSlices, MEDCALC::SlicesParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationSlices, MEDCALC::SlicesParameters>(params.colorMap);

  if (params.orientation != _params.orientation)
    updateOrientation(params.orientation);
  if (params.nbSlices != _params.nbSlices)
    updateNbSlices(params.nbSlices);
}

void
MEDPresentationSlices::updateNbSlices(const int nbSlices)
{
  _params.nbSlices = nbSlices;

  // GUI helper:
  setIntProperty(MEDPresentationSlices::PROP_NB_SLICES, nbSlices);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    clearPreviousSlices();
    generateAndDisplay();
  }
}

void
MEDPresentationSlices::updateOrientation(const MEDCALC::SliceOrientationType orientation)
{
  _params.orientation = orientation;

  // GUI helper:
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, static_cast<int>(orientation));

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;

    clearPreviousSlices();
    generateAndDisplay();
  }
}

