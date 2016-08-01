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

#include "MEDPresentationScalarMap.hxx"
#include "MEDPresentationException.hxx"

#include <SALOME_KernelServices.hxx>
#undef LOG  // should be fixed in KERNEL - double definition
#include <Basics_Utils.hxx>

#include <sstream>

const std::string MEDPresentationScalarMap::TYPE_NAME = "MEDPresentationScalarMap";

MEDPresentationScalarMap::MEDPresentationScalarMap(const MEDCALC::ScalarMapParameters& params,
                                                   const MEDCALC::ViewModeType viewMode) :
    MEDPresentation(params.fieldHandlerId, TYPE_NAME, viewMode, params.colorMap, params.scalarBarRange), _params(params)
{
}

void
MEDPresentationScalarMap::internalGeneratePipeline()
{
  MEDPresentation::internalGeneratePipeline();

  MEDPyLockWrapper lock;

  std::ostringstream oss;
  std::string view(getRenderViewVar());

  pushAndExecPyLine( getRenderViewCommand() ); // instanciate __viewXXX

  oss << _srcObjVar << " = pvs.MEDReader(FileName='" << _fileName << "');";
  pushAndExecPyLine(oss.str()); oss.str("");

  // Populate internal array of available components:
  fillAvailableFieldComponents();
//  dumpIntProperties();
//  dumpStringProperties();

  pushAndExecPyLine(_objVar + " = " + _srcObjVar);

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

  // Retrieve Python object for internal storage:
//  PyObject* p_obj = getPythonObjectFromMain(obj.c_str());
//  PyObject* p_disp = getPythonObjectFromMain(disp.c_str());
//  pushPyObjects(std::make_pair(_objId, p_obj), std::make_pair(disp_id, p_disp));
}

void
MEDPresentationScalarMap::updatePipeline(const MEDCALC::ScalarMapParameters& params)
{
  if (params.fieldHandlerId != _params.fieldHandlerId)
    throw KERNEL::createSalomeException("Unexpected updatePipeline error! Mismatching fieldHandlerId!");

  if (std::string(params.displayedComponent) != std::string(_params.displayedComponent))
    updateComponent<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(std::string(params.displayedComponent));
  if (params.scalarBarRange != _params.scalarBarRange)
    updateScalarBarRange<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.scalarBarRange);
  if (params.colorMap != _params.colorMap)
    updateColorMap<MEDPresentationScalarMap, MEDCALC::ScalarMapParameters>(params.colorMap);
}

