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
                                               const MEDCALC::MEDPresentationViewMode viewMode) :
        MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange),
        _params(params)
{
  setIntProperty(MEDPresentationSlices::PROP_NB_SLICES, params.nbSlices);
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, params.orientation);
}

std::string
MEDPresentationSlices::getNbSlicesCommand() const
{
  std::ostringstream oss1;
  // TODO
//  oss1 << _objVar << ".SliceType.Normal = " << norm << ";";
  return oss1.str();
}

std::string
MEDPresentationSlices::getOrientationCommand() const
{
  std::ostringstream oss1;
  std::string norm;

  switch(_params.orientation)
  {
    case MEDCALC::SLICE_NORMAL_TO_X:
      norm = "[1.0, 0.0, 0.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_Y:
      norm = "[0.0, 1.0, 0.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_Z:
      norm = "[0.0, 0.0, 1.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_XY:
      norm = "[1.0, 1.0, 0.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_XZ:
      norm = "[1.0, 0.0, 1.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_YZ:
      norm = "[0.0, 1.0, 1.0]";
      break;
    case MEDCALC::SLICE_NORMAL_TO_XYZ:
      norm = "[1.0, 1.0, 1.0]";
      break;
    default:
      const char * mes = "Unexpected getSliceOrientationCommand() error!";
      STDLOG(mes);
      throw KERNEL::createSalomeException(mes);
  }

  oss1 << _objVar << ".SliceType.Normal = " << norm << ";";
  return oss1.str();
}


void
MEDPresentationSlices::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss_o, oss;
  std::string view(getRenderViewVar());

  oss << _srcObjVar << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Populate internal array of available components:
  fillAvailableFieldComponents();
  pushAndExecPyLine( getRenderViewCommand() ); // instanciate __viewXXX
  oss << _objVar << " = pvs.Slice(Input=" << _srcObjVar << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _objVar << ".SliceType = 'Plane';";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Set slice orientation
  pushAndExecPyLine(getOrientationCommand());

  oss << _dispVar << " = pvs.Show(" << _objVar << ", " << view << ");";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << "pvs.ColorBy(" << _dispVar << ", ('" << _fieldType << "', '" << _fieldName << "'));";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _dispVar <<  ".SetScalarBarVisibility(" << view << ", True);";
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << getRescaleCommand();
  pushAndExecPyLine(oss.str()); oss.str("");
  oss << _lutVar << " = pvs.GetColorTransferFunction('" << _fieldName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");
  pushAndExecPyLine(getColorMapCommand()); oss.str("");
  pushAndExecPyLine(getResetCameraCommand());
  pushAndExecPyLine("pvs.Render();");
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
    std::string cmd = getNbSlicesCommand();
    pushAndExecPyLine(cmd);
    pushAndExecPyLine("pvs.Render();");
  }
}

void
MEDPresentationSlices::updateOrientation(const MEDCALC::MEDPresentationSliceOrientation orientation)
{
  _params.orientation = orientation;

  // GUI helper:
  setIntProperty(MEDPresentationSlices::PROP_SLICE_ORIENTATION, orientation);

  // Update the pipeline:
  {
    MEDPyLockWrapper lock;
    std::string cmd = getOrientationCommand();
    pushAndExecPyLine(cmd);
    pushAndExecPyLine("pvs.Render();");
  }
}

